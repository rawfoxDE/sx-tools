/************************************************************************/
/*                                                                      */
/*      fichier         : map_function.c                                */
/*      projet          : amiglobe                                      */
/*      date création   : 1993                                          */
/*      commentaire     : fonctions de dessin de la carte               */
/*      révision        : $VER: map_function.c 1.010 (14 Oct 1994)
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*      $HISTORY:                                                */
/*                                                              */
/*                     1993 : 0.01 : version initiale           */   
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*      includes                                                        */
/************************************************************************/
        
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <libraries/dos.h>
#include <intuition/intuition.h>
#include <graphics/gfxmacros.h>
#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>
#include <graphics/displayinfo.h>
#include <graphics/display.h>
#include <graphics/gfxbase.h>
#include <proto/dos.h>  /* pour DOSBase*/
#include <libraries/fastsincos.h>
#include <libraries/locale.h>
#include "iff/jiff.h"

/* Prototypes */
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/macros.h>
#include <clib/datatypes_protos.h>

/* ANSI C */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stat.h>
#include <math.h>

//extern  struct Library *IntuitionBase;
extern  struct GfxBase  *GfxBase;
extern  struct Library *DataTypeBase;
extern  struct Library *FastsincosBase;
extern struct Locale * MyLocale;

#define REG(x) register __ ## x
#define ASM    __asm
#define SAVEDS __saveds

#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>

#include "amiglobe_types.h"
#include "database_types.h"
#include "map_function_protos.h"
#include "map_function_types.h"
#include "main_protos.h"
#include "database_protos.h"
#include "3d_types.h"
#include "3d_protos.h"
#include "flag_protos.h"
#include "iff_fct_protos.h"
#include "divers.h"
#include "projection.h"

/************************************************************************/
/*      defines                                                         */
/************************************************************************/


/* Resolution minimum sans chargement */

#define MIN_RESO        3

/*
#define DRAW
*/
#define COLOR_BLEU              0
#define COLOR_SELECT_PAYS       1
#define COLOR_SELECT_SEG        2
#define COLOR_COTE              1
#define COLOR_TEXTE             3
#define COLOR_LATLON            1
#define COLOR_FOND_TEXTE        1
#define COLOR_FOND_TEXTE_INFO   -1
#define COLOR_TEXTE_INFO        -2
#define COLOR_FRONTIERE         5
#define COLOR_AFF_NOM_PAYS 6

#define COLOR_COAST             -1
#define COLOR_COUNTRY           4
#define COLOR_STATE             4
#define COLOR_ISLANDS           2
#define COLOR_LAKES             3
#define COLOR_RIVERS            2

int     tab_look_up[]={ 
                0,
                COLOR_COAST , 
                COLOR_COUNTRY , 
                COLOR_COUNTRY , 
                COLOR_STATE , 
                COLOR_ISLANDS, 
                COLOR_LAKES,
                COLOR_RIVERS};
int     nb_colors_free=12;
int     num_first_color=4;

/************************************************************************/
/*      variables externes                                              */
/************************************************************************/

/*
extern  struct IntuitionBase *IntuitionBase;
*/
/*UWORD palette_glob[256];*/
extern  struct  Screen  *sC;

extern  struct  Screen  *sC_Image;
extern  BOOL continent_sel[6];


Object  *Object_Fond=NULL;

static  E_Type_Graph    Glob_Type;
/**********************************/
/* Tableau des noms de continents */
/**********************************/

char    *cont_name[]={"Europe","Africa","Asia","Oceania",
            "South America","North America",NULL};


CLIP    clip_max;
CLIP    clip_proj;

int     cx=320;
int     cy=100;

int     max_border=0;

/* Tableau de pointeurs sur les vecteurs */
        
BORDER  *tab_vect;
COUNTRY *tab_country;
TRAJECTOIRE ** tab_trajectoire;
/* Pour le buffer circulaire */

int     *Tab_Seg_Loaded;
int     Max_Loaded_Vect=100;
int     Num_Cur=0;
BPTR    bptr_big=NULL;

/*****************************/

#define DMAX    10

int     max_country=0;          /* Nombre max de pays       */
int max_elem=0;

struct TextFont *Titre_Font_Glob=NULL;
struct TextFont *Text_Font_Glob=NULL;

struct  ViewPort        *vp_glob;

/* Variables fournies de maniere externe...*/
extern PREFERENCE Pref;

/*****************************************/
/* Type de donnee pour le graphe courant */
/*****************************************/

E_Type_Data     Type_Data_Glob;

/********************************************************************/
/*      Implémentation                                              */
/********************************************************************/


/****************************************************************************/
char    *get_pays_name(int num_pays)
{
        return(tab_country[num_pays].name);
}

/****************************************************************************/

int     
get_max_country(void)
{
        return(max_country);
}

/****************************************************************************/

/****************************************************************************/

int     
get_max_elem(void)
{
        return(max_elem);
}

/****************************************************************************/

void    
Map_Give_Type_Proj(E_TYPE_PROJ Type)
{
        Pref.Type_Proj=Type;
}

/****************************************************************************/

void    
Map_Give_Type_Trace(E_TYPE_TRACE Type)
{
        Pref.Type_Trace=Type;
}

/****************************************************************************/
/* Determine la meilleure profondeur en mode automatique, */
/* ou retourne la profondeur choisie en mode non auto    */
int     Find_Profondeur(int Prof)
{
    int     Prof_Ret;
    int sx=(Pref.clip_cur.maxx-Pref.clip_cur.minx);

    /* En mode non automatique, on retourne direct */
    if (Prof!=0)
    {
        Prof_Ret=Prof;
    }
    else
    {
        Prof_Ret= 2+ log((float)(sx/1000));
    }
    if (Prof_Ret<1)
        Prof_Ret=1;
    if (Prof_Ret>5)
        Prof_Ret=5;
    /*printf("Set Prof:%d\n",Prof_Ret);*/
    return(Prof_Ret);
}

/* Suite au chargement d'un nouveau border. libere un ancien    */
/* border dans un buffer circulaire                             */
void Free_Border(int In_Num_Vect)
{
    int     Old_Num;

    Old_Num=Tab_Seg_Loaded[Num_Cur];
    if(Old_Num!=-1)
    {
        /*printf("  désallocation border n°%d",Old_Num);*/
        FreeMem(tab_vect[Old_Num].p_vect_maxres,
            (tab_vect[Old_Num].nb_vect_bis*sizeof(VECT)));
        /*free(tab_vect[Old_Num].p_vect_maxres);*/
        tab_vect[Old_Num].p_vect_maxres=NULL;
    }
    Tab_Seg_Loaded[Num_Cur]=In_Num_Vect;
    Num_Cur=(Num_Cur+1)%Max_Loaded_Vect;
    /*printf("  num_cur=%d\n",Num_Cur);*/
}

/*********************************************************** check_border() ***/

/* Verifie si un border est charge (en tenant compte de la */
/* resolution demandee) . Si le border n'est pas charge,   */
/* on le charge.                                                                                   */

void check_border(int num_vect,int *Out_Nb_Vect,VECT **Out_P_Vect)
{
    BORDER  *current;
    short   int     nb_vect;

    
    current=&tab_vect[num_vect];

    *Out_Nb_Vect=current->nb_vect;
    *Out_P_Vect=current->p_vect;

    if(Pref.prof<MIN_RESO)
    {
        /* il faut charger le border */
        if(current->p_vect_maxres==NULL)
        {
            /* Si le segment n'avait jamais ete charge */
            if(current->nb_vect_bis==-1)
            {
                    Seek(bptr_big,current->offset,OFFSET_BEGINNING);
                    Read(bptr_big,&nb_vect,sizeof(short int));
                    current->nb_vect_bis=nb_vect;
            }
            else
            {
                    Seek(bptr_big,current->offset+sizeof(short int),OFFSET_BEGINNING);
                    nb_vect=current->nb_vect_bis;
            }
            current->p_vect_maxres=(VECT *)AllocMem(sizeof(VECT)*nb_vect,MEMF_CLEAR);
            /*current->p_vect_maxres=(VECT *)malloc(sizeof(VECT)*nb_vect);*/
            if(current->p_vect_maxres!=NULL)
            {
                    Read(bptr_big,current->p_vect_maxres,sizeof(VECT)*nb_vect);
                    current->nb_vect_bis=nb_vect;
                    Free_Border(num_vect);
            }
            else
                printf("ERROR: not enough memory\n");
        }
        if(current->p_vect_maxres!=NULL)
        {
                *Out_Nb_Vect=current->nb_vect_bis;
                *Out_P_Vect=current->p_vect_maxres;
        }
    }
}

/****************************************************************************/
/* Converti un angle entier de -18000 à 18000 en un angle en radians */
float   Map_Convert_Angle(int angle)
{
        float   f_angle;

        f_angle=((float)angle)*2*PI/((float)(clip_max.maxx-clip_max.minx));
        return(f_angle);
}

/****************************************************************************/

void    map_set_prof(int        profondeur)
{
        /*printf("profondeur:%d\n",profondeur);*/
        Pref.prof_real=profondeur;
        Pref.prof=Find_Profondeur(Pref.prof_real);
}

/****************************************************************************/
void init_map_datas(void)
{
        int i;
        tab_vect=(BORDER *)malloc(sizeof(BORDER)*MAX_BORDER);
        tab_country=(COUNTRY *)malloc(sizeof(COUNTRY)*MAX_COUNTRY);
        Tab_Seg_Loaded=(int *)malloc(sizeof(int)*Max_Loaded_Vect);
        tab_trajectoire=(TRAJECTOIRE **)malloc(sizeof(TRAJECTOIRE *)*MAX_TRAJECTOIRE);
        tab_trajectoire[0]=NULL;
        for(i=0;i<Max_Loaded_Vect;i++)Tab_Seg_Loaded[i]=-1;
}

void    free_map_datas(void)
{
        int i;
        if(bptr_big)Close(bptr_big);
        for (i=0;i<max_country;i++)
                delete_element(tab_country[i].P_Elem_Rem);
}

/****************************************************************************/
void    give_size_window(int sx,int sy)
{
        Pref.win_width=sx;
        Pref.win_height=sy;
    /*printf("changement taille fenêtre:x=%d  y=%d \n",sx,sy);*/
}


/****************************************************************************/

void Map_Put_Million(char *In_Buffer,char *P_Format,int data)
/* met dans In_Buffer P_Format avec le %s remplace par la string data */
{
   char Buffer[80];
   int n,data_temp,nb;

   data_temp=data;
   nb=log10(data)+1;
/*   while (data_temp!=0)
   {
            nb++;
            data_temp=data_temp/10;
   }*/
   nb += (nb) / 3 - 1;
   Buffer[nb+1]=0;
   Buffer[0]=' ';
   n=3;

   while (data!=0)
   {
     Buffer[nb]='0'+data%10;
     data=data/10;
     nb--;
     n--;
     if( (n==0) && (data!=0) )
     {
            if (MyLocale!=NULL)
                Buffer[nb]=MyLocale->loc_GroupSeparator[0];
            else
                Buffer[nb]=',';
            nb--;
            n=3;
     }
   }
   sprintf(In_Buffer,P_Format,Buffer);
}

/****************************************************************************/

void IPrint_NoAtt(
        struct  RastPort *rpG,
        int     x,
        int     y,
        char    *p_texte
        )
{
        if(
                (x>0)&&
                (x<Pref.win_width)&&
                (y>0)&&
                (y<Pref.win_height))
        {
                Move(rpG,x,y);
                Text(rpG,p_texte,strlen(p_texte));
        }
}

/****************************************************************************/


void IPrintFond(
        struct  RastPort *rpG,
        int     x,
        int     y,
        char    *p_texte,
        int     In_Color
        )
{
        if(In_Color==-1)In_Color=COLOR_TEXTE;
        SetAPen(rpG,In_Color+num_first_color);
        SetDrMd(rpG,JAM1);
        Move(rpG,x,y);
        Text(rpG,p_texte,strlen(p_texte));
}

void IPrint(
        struct  RastPort *rpG,
        int     x,
        int     y,
        char    *p_texte,
        int     In_Color
        )
{
        if(In_Color==-1)In_Color=-num_first_color+1;
        SetAPen(rpG,In_Color+num_first_color);
        SetBPen(rpG,0L);
        SetDrMd(rpG,JAM2);
        Move(rpG,x,y);
        Text(rpG,p_texte,strlen(p_texte));
/*      IPrint_NoAtt(rpG,x,y,p_texte);*/
}

/****************************************************************************/

/*void TPrint(
        struct  RastPort *rpG,
        int     x,
        int     y,
        char    *p_texte,
        int couleur_texte,
        int couleur_fond
        )
{
        SetAPen(rpG,couleur_texte);
        SetBPen(rpG,couleur_fond);
        SetDrMd(rpG,JAM2);
        Move(rpG,x,y);
        Text(rpG,p_texte,strlen(p_texte));
}*/

/****************************************************************************/

void aff_country(int    num_pays,struct RastPort *rpG)
{
        CLIP    old_clip;
/*      float   sx,sy;
        float   h1,w1;*/

/*      char buffer[100];*/

        old_clip = Pref.clip_cur;

        Pref.clip_cur.minx=tab_country[num_pays].c_clip.minx-50;
        Pref.clip_cur.miny=tab_country[num_pays].c_clip.miny-50;
        Pref.clip_cur.maxx=tab_country[num_pays].c_clip.maxx+50;
        Pref.clip_cur.maxy=tab_country[num_pays].c_clip.maxy+50;

        /*************************/
        /* Affichage de la carte */
        /*************************/
/*      sx=(float)(Pref.clip_cur.maxx-Pref.clip_cur.minx);
        sy=(float)(Pref.clip_cur.maxy-Pref.clip_cur.miny);

        h1=  Pref.win_width*sx*(clip_max.maxy-clip_max.miny)/
            ((clip_max.maxx-clip_max.minx)*sy);
        w1= Pref.win_height*sy*(clip_max.maxx-clip_max.minx)/((clip_max.maxy-clip_max.miny)*sx);

        if (h1<=Pref.win_height)
    {
                Pref.win_width=(Pref.win_width*h1)/Pref.win_height;
        }
    else
    {
                Pref.win_height=(Pref.win_height*w1)/Pref.win_width;
        }
*/
        SetRast(rpG,COLOR_BLEU+num_first_color);
        WaitBlit();
        /*Map_Aff_LatLon(rpG);*/
        fill_country (num_pays,COLOR_STATE,rpG);
/*      IPrint(rpG,50,20,buffer); */  /* ????? */

        Pref.clip_cur=old_clip;
}


/****************************************************************************/

void
aff_info_page1(int      num_pays,struct RastPort *rpG)
/* ,int cartex,int cartey*/
{
    char buffer[100];
    ELEM_REM * nom_capitale;
    int     posx,posy;

    /* On recupere la position de la carte */
    if (Pref.win_width*sC->Height>sC->Width*Pref.win_height)
    {
        posx=5;
        posy=Pref.win_height+10;
    }
    else
    {
        posx=Pref.win_width+5;
        posy=15;
    }
        SetAPen(rpG,1L);        /*COLOR_TEXTE_INFO+num_first_color);*/
        SetBPen(rpG,0L);    /*COLOR_FOND_TEXTE_INFO+num_first_color);*/
        SetDrMd(rpG,JAM1);
    if (Titre_Font_Glob)
        SetFont(rpG,Titre_Font_Glob);
    SetSoftStyle(rpG,(FSF_BOLD | FSF_UNDERLINED | FSF_ITALIC),(FSF_BOLD ));
        /* lecture des donnees */
        sprintf(buffer,"Official Name:%s",tab_country[num_pays].off_name);
        IPrint(rpG,posx,posy,buffer,-1);
        if (Text_Font_Glob)
        SetFont(rpG,Text_Font_Glob);
                Map_Put_Million(buffer,"Population: %s",tab_country[num_pays].population);
        IPrint(rpG,posx,posy+Text_Font_Glob->tf_YSize+2,buffer,-1);

        sprintf(buffer,"Continent: %s",cont_name[tab_country[num_pays].continent]);
        IPrint(rpG,posx,posy+2*Text_Font_Glob->tf_YSize+2,buffer,-1);
        Map_Put_Million(buffer,"Area: %s Km²",tab_country[num_pays].superficie);
        IPrint(rpG,posx,posy+3*Text_Font_Glob->tf_YSize+2,buffer,-1);
        if (tab_country[num_pays].superficie!=0)
        {
                sprintf(buffer, "Density: %d inh./km²",
                  (tab_country[num_pays].population)/
                        (tab_country[num_pays].superficie));
                IPrint(rpG,posx,posy+4*Text_Font_Glob->tf_YSize+2,buffer,-1);
        }
        nom_capitale=capitale(num_pays);
        if (nom_capitale!=NULL)
        {
                sprintf(buffer,"Capital: %s (%d hab)",nom_capitale->Nom,
                                nom_capitale->caracteristique);
                IPrint(rpG,posx,posy+5*Text_Font_Glob->tf_YSize+2,buffer,-1);
        }
        Map_Put_Million(buffer,"Gross nat. product: M$%s",
                tab_country[num_pays].PIB);
        IPrint(rpG,posx,posy+6*Text_Font_Glob->tf_YSize+2,buffer,-1);
        if(tab_country[num_pays].population!=0)
        {
                Map_Put_Million(buffer,"GNP per capita:$ %s",
                  (int)(((float)tab_country[num_pays].PIB*1000000)/
                            (tab_country[num_pays].population)));
                IPrint(rpG,posx,posy+7*Text_Font_Glob->tf_YSize+2,buffer,-1);
        }
}

/******************************************************** aff_info() ******/

void
aff_info(int num_pays,struct RastPort *rpG,struct Window * window1)
{
    CLIP        My_Clip;
    static char Buffer[90];
    char Filename[50];
    int clipx,clipy; /*dimension du clip du pays sélectionné*/        
    int *P_Tab_Drapeau;
    int Flg_Width,Flg_Height;
    PREFERENCE Old_Pref=Pref;
    Application_Sleep(TRUE);
    /* Fenêtre dans laquelle se dessine le pays*/
    if (num_pays!=-1)
    {
        sprintf(Buffer,"Info about:%s",tab_country[num_pays].name);
        SetWindowTitles(window1,Buffer,(UBYTE *)~0);
        clipx=tab_country[num_pays].c_clip.maxx-
                        tab_country[num_pays].c_clip.minx;
        clipy=tab_country[num_pays].c_clip.maxy-
                        tab_country[num_pays].c_clip.miny;
        if ( (float)(clipx / clipy) > 1.2)
        {
            if ( (float)(clipx / clipy) > 2.4)
            {
                /* pays très large*/
                /*printf("pays très large  %d  %d\n",clipx,clipy);*/
                give_size_window((int)(window1->Width*0.8),
                                (int)(window1->Height*0.4));
            }
            else
            {
                /* pays plus large que haut*/
                /*printf("pays plus large que haut  %d  %d\n",clipx,clipy);*/
                give_size_window((int)(window1->Width*0.7),
                                (int)(window1->Height*0.5));
            }
        }
        else
        {
            if ( (float)(clipx / clipy) < 0.6)
            {
                /*printf("pays très haut  %d  %d\n",clipx,clipy);*/
                give_size_window((int)(window1->Width*0.4),
                             (int)(window1->Height*0.8));
            }
            else
            {
                /*printf("pays plus haut que large  %d  %d\n",clipx,clipy);*/
                give_size_window((int)(window1->Width*0.5),
                                (int)(window1->Height*0.7));
            }
        }
        /* affichage de la carte                                     */
        Pref.prof=Find_Profondeur(2+(int)log((float)(clipx*.3/Pref.win_width)));
        Pref.Flg_Aff_Elem_Rem=FALSE;
        aff_country (num_pays,rpG);
        /* la carte s'affiche en bleu et le reste est rempli en gris */
        /* s'il y a des rivières qui s'"échappent" par exemple       */
        SetAPen(rpG,0);
        RectFill(rpG,Pref.win_width,0,
                window1->Width-window1->BorderRight-window1->BorderLeft,
                window1->Height-window1->BorderBottom-window1->BorderTop);
        RectFill(rpG,0,Pref.win_height,Pref.win_width,
                window1->Height-window1->BorderTop-window1->BorderBottom);
        /* cadre noir autour de la carte                             */
        SetAPen(rpG,1);
        Move(rpG,0,Pref.win_height);
        Draw(rpG,Pref.win_width,Pref.win_height);
        Draw(rpG,Pref.win_width,0);
        /* affichage des infos (texte)                               */
        aff_info_page1 (num_pays,rpG);
        Pref.prof = Find_Profondeur(Pref.prof_real);
    }
    sprintf(Filename,"flags/%s.flag",tab_country[num_pays].name);
    /* Affichage du drapeau du pays, s'il y en a un */
    if(Flag_Read(Filename,&P_Tab_Drapeau))
    {
        Flg_Width  =window1->Width*0.2;
        /* garde le meme aspect en pixels carres/pas carres */
        Flg_Height =Flg_Width*2/3*640*sC->Height/(512*sC->Width);
        
        My_Clip.minx=window1->Width-Flg_Width*1.1-20;
        My_Clip.maxx=window1->Width-Flg_Width*.1-20;
        My_Clip.miny=window1->Height-Flg_Height*1.1-20;
        My_Clip.maxy=window1->Height-Flg_Height*.1-20;

        IPrint(rpG,My_Clip.minx,My_Clip.miny-Text_Font_Glob->tf_YSize-2,
                                "Flag",1);
        WaitBlit();
        Flag_Draw(rpG,&My_Clip,P_Tab_Drapeau,window1->WScreen->ViewPort.ColorMap);

        free(P_Tab_Drapeau);        
    }
    Application_Sleep(FALSE);
    Pref=Old_Pref;
}

/******************************************************** print_pays() ****/

void print_pays(struct  RastPort *rpG)
{
        int     i,posx,posy;
        posx=0;
        posy=0;
        for (i=0;i<max_country;i++)
    {
                if (posy>8)
        {
                        posy=0;
                        posx++;
                }
                Move(rpG,10+posx*100,20+posy*10);
                Text(rpG,tab_country[i].name,strlen(tab_country[i].name));
                posy++;
        }
}


/* typedef pour les MyArea....                                  */      
/* indique si le dernier point était sorti de   */
/* l'écran et qu'il a fallu le projeter                 */
typedef enum {AUCUN,HAUT,BAS,GAUCHE,DROITE}     CHECKXY;
CHECKXY check=AUCUN;

/****************************************************************************/
void check_xy( 
        int * x,
        int * y)
{
        if ((*x)>Pref.clip_cur.maxx)
                (*x)=Pref.clip_cur.maxx;
        if ((*x)<Pref.clip_cur.minx)
                (*x)=Pref.clip_cur.minx;
        if ((*y)>Pref.clip_cur.maxy)
                (*y)=Pref.clip_cur.maxy;
        if ((*y)<Pref.clip_cur.miny)
                (*y)=Pref.clip_cur.miny;
}

/****************************************************************************/
CHECKXY check_xy_win(
        int * x,
        int * y)
{
        CHECKXY renvoi=AUCUN;
        if((*x)<0)
        {
                *x=0;
                /*printf("g");*/
                renvoi=GAUCHE;
        }
        if((*y)<0)
        {
                *y=0;
                /*printf("h");*/
                renvoi=HAUT;
        }
        if((*x)>Pref.win_width-1 )
        {
                *x=Pref.win_width-1;
                /*printf("d");*/
                renvoi=DROITE;
        }
        if((*y)>Pref.win_height-1)
        {
                *y=Pref.win_height-1;
                /*printf("b");*/
                renvoi=BAS;
        }
        return renvoi;
}
        
/****************************************************************************/
void MyDraw(
        struct  RastPort *rpG,
        int     x,
        int     y,
        int     code
        )
{

        if (code>=Pref.prof)
    {
                conv_xy(&x,&y);
                Draw(rpG,x,y);
        }
}

/****************************************************************************/

void
MyMove(struct  RastPort *rpG,int x,int y)
{
        conv_xy(&x,&y);
        Move(rpG,x,y);
}

/****************************************************************************/
void 
MyAreaDraw(struct  RastPort *rpG,int x,int y,int code)
{
    if(code>=Pref.prof)
    {
        CHECKXY aux;
        /*check_xy(&x,&y);*/
        conv_xy(&x,&y);
        aux=check_xy_win(&x,&y);
        if (aux==AUCUN)
        {
            AreaDraw(rpG,x,y);
        }
        else
        {
            switch (check)
                        {
                        case HAUT:
                                switch(aux)
                                {
                                case GAUCHE:
                                        AreaDraw(rpG,0,0);
                                        break;
                                case DROITE:
                                        AreaDraw(rpG,Pref.win_width,0);
                                        break;
                                default:
                                    break;
                                }
                        break;
                        case BAS:
                                switch(aux)
                                {
                                case GAUCHE:
                                        AreaDraw(rpG,0,Pref.win_height);
                                        break;
                                case DROITE:
                                        AreaDraw(rpG,Pref.win_width,Pref.win_height);
                                        break;
                                default:
                                    break;  
                                }
                        break;
                        case GAUCHE:
                                switch(aux)
                                {
                                case HAUT:
                                        AreaDraw(rpG,0,0);
                                        break;
                                case BAS:
                                        AreaDraw(rpG,0,Pref.win_height);
                                        break;
                                default:
                                    break;  
                                }
                        break;
                        case DROITE:
                                switch(aux)
                                {
                                case HAUT:
                                        AreaDraw(rpG,Pref.win_width,0);
                                        break;
                                case BAS:
                                        AreaDraw(rpG,Pref.win_width,Pref.win_height);
                                        break;
                                default:
                                    break;  
                                }
                        break;
                        default:
                        break;
                        }
                        AreaDraw(rpG,x,y);
                }
                check=aux;
        }
}

/****************************************************************************/

void MyAreaMove(
        struct  RastPort *rpG,
        int     x,
        int     y
        )
{               
/*      check_xy(&x,&y);*/
        conv_xy(&x,&y);
        check_xy_win(&x,&y);
        AreaMove(rpG,x,y);
        check=AUCUN;
}


/****************************************************************************/

void 
MyAreaEnd(struct  RastPort *rpG)
{
    AreaEnd(rpG);
    /*printf("\n");*/
}

        
        
/****************************************************************************/

void
map_draw_rect(struct  RastPort *rpG,int x,int y,int sx,int sy)
{
        Move(rpG,x,y);
        Draw(rpG,x+sx,y);
        Draw(rpG,x+sx,y+sy);
        Draw(rpG,x,y+sy);
        Draw(rpG,x,y);
}



/****************************************************************************/

int
find_country(int x,int y)
{
    int i,num_country;
    COUNTRY *p_c;
    int     current_size,size;
    current_size =
        (clip_max.maxx-clip_max.minx)*(clip_max.maxy-clip_max.miny);
    num_country=-1;
    conv_inv_xy(&x,&y);
    for (i=0;i<max_country;i++)
    {
        p_c=&tab_country[i];
        /* On cherche le pays le plus petit inclus dans son rectangle*/
        if ( (x<p_c->c_clip.maxx) && (x>p_c->c_clip.minx) &&
            (y<p_c->c_clip.maxy) && (y>p_c->c_clip.miny))
        {
            size=(p_c->c_clip.maxx-p_c->c_clip.minx)*
                (p_c->c_clip.maxy-p_c->c_clip.miny) ;

            if (size<current_size)
            {
                num_country=i;
                current_size=size;
            }
        }
    }
    return(num_country);
}

/*************************************************************************/
/* Cherche un segment dans la table des vecteurs. Le parametre deep permet*/
/* de definier une "profondeur de recherche, c.a.d. de trouver un segment*/
/* sous un autre.                                                                                    */
/*************************************************************************/

int map_find_segment(
        int     x,
        int     y
        )
{
        int i,num_segment;
        BORDER *p_b;
        int     current_size,size;
        int     errx,erry;
        int     x0,y0;
        int     x1,y1;

        num_segment=-1;
        current_size=(clip_max.maxx-clip_max.minx)
            *(clip_max.maxy-clip_max.miny);
        conv_inv_xy(&x,&y);

        x0=0;y0=0;
        x1=1;y1=1;
        conv_inv_xy(&x0,&y0);
        conv_inv_xy(&x1,&y1);
        errx=x1-x0;
        erry=y1-y0;

        for (i=0;(i<max_border);i++)
    {
                p_b=&tab_vect[i];
                if( ( x >= p_b->b_clip.minx-errx ) &&
                    ( x <= p_b->b_clip.maxx+errx ) &&
                    ( y >= p_b->b_clip.miny-erry ) &&
                    ( y <= p_b->b_clip.maxy+erry ) )
        {

                        size=   (p_b->b_clip.maxx-p_b->b_clip.minx)*
                                (p_b->b_clip.maxy-p_b->b_clip.miny) ;

                        if (size<current_size)
            {
                                num_segment=i;
                                current_size=size;
                        }
                }
        }
        return(num_segment);
}

/****************************************************************************/

void map_draw_segment(
        struct  RastPort        *rpG,
        int     num_seg,
        int     color
        )
{
        int j;
        int nb_vect;
        VECT    *v;

        SetAPen(rpG,color+num_first_color);

        check_border(num_seg,&nb_vect,&v);              
/* tab_vect[num_seg].p_vect[j].xxx sont les coordonnees des segments */
        MyMove(rpG,v[0].x,v[0].y);
        for (j=1;j<nb_vect;j++)
    {
                MyDraw(rpG,v[j].x,v[j].y,v[j].Code);
        }

}

/****************************************************************************/
int
tst_end( FILE    *input)
{
        char c;
        int     code_ret;

        /* lecture du prochain caractere */
        do
    {
                c=getc(input);
                if(c==EOF)return(TRUE);
                if(c=='#')
        {
                        do
            {
                                c=getc(input);
                        }
              while ((c!=0x0a)&&(c!=EOF));
                        c=getc(input);
                }

        }
      while((!isalnum(c))&&(c!='E'));
        if ((!isalpha(c))||(c!='E'))
    {
                ungetc(c,input);
                code_ret=FALSE;
        }
    else
    {
                code_ret=TRUE;
        }
        return(code_ret);
}

/****************************************************************************/

void tstpt(
        int     x,
        int     y,
        int     *pmx,
        int     *pmy,
        int     *pMX,
        int     *pMY
        )
{
        if (x<*pmx)
    {
                *pmx=x;
        }
        if (x>*pMX)
    {
                *pMX=x;
        }
        if (y<*pmy)
    {
                *pmy=y;
        }
        if (y>*pMY)
    {
                *pMY=y;
        }
}
/****************************************************************************/

int     
tstclip( int mx, int my,int  MX, int MY, CLIP *p_clip)
{
    if (Pref.Type_Proj==PROJ_GLOBE)
    {
        float centrex=Map_Convert_Angle((mx+MX)/2);
        float centrey=Map_Convert_Angle((my+MY)/2);
        float largeur=Map_Convert_Angle((clip_proj.maxx-clip_proj.minx)/2);
        //float hauteur=Map_Convert_Angle((clip_proj.maxy-clip_proj.miny)/2);
        float minx=Map_Convert_Angle(p_clip->minx);
        float maxx=Map_Convert_Angle(p_clip->maxx);
        float miny=Map_Convert_Angle(p_clip->miny);
        float maxy=Map_Convert_Angle(p_clip->maxy);
        if (largeur>PI/2)
            largeur=PI/2;
        if (FastsincosBase!=NULL)
        {
            if (acos(fastsin(miny)*fastsin(centrey)+
                fastcos(miny)*fastcos(centrey)*fastcos(minx-centrex))<largeur)
                return TRUE;
            else
            if (acos(fastsin(miny)*fastsin(centrey)+
                fastcos(miny)*fastcos(centrey)*fastcos(maxx-centrex))<largeur)
                return TRUE;
            else
            if (acos(fastsin(maxy)*fastsin(centrey)+
                fastcos(maxy)*fastcos(centrey)*fastcos(minx-centrex))<largeur)
                return TRUE;
            else
            if (acos(fastsin(maxy)*fastsin(centrey)+
                fastcos(maxy)*fastcos(centrey)*fastcos(maxx-centrex))<largeur)
                return TRUE;
            else
                return FALSE;
        }
        else
        {
            if (acos(sin(miny)*sin(centrey)+
                cos(miny)*cos(centrey)*cos(minx-centrex))<largeur)
                return TRUE;
            else
            if (acos(sin(miny)*sin(centrey)+
                cos(miny)*cos(centrey)*cos(maxx-centrex))<largeur)
                return TRUE;
            else
            if (acos(sin(maxy)*sin(centrey)+
                cos(maxy)*cos(centrey)*cos(minx-centrex))<largeur)
                return TRUE;
            else
            if (acos(sin(maxy)*sin(centrey)+
                cos(maxy)*cos(centrey)*cos(maxx-centrex))<largeur)
                return TRUE;
            else
                return FALSE;
         }
    }
    else
    if ((mx>p_clip->maxx)||(MX<p_clip->minx)||
        (my>p_clip->maxy)||(MY<p_clip->miny))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


/***************************************************** reset_bord() *****/

/************************************************/
/* Reinitialise les drapeaux indiquant qu'un    */
/* bord a ete trace                             */
/************************************************/

void reset_bord(void)
{
        int     i;
        for(i=0;i<max_border;i++){
                tab_vect[i].flg_draw=FALSE;
        }
}

/******************************************* map_aff_country_name() *****/

void
map_aff_country_name(COUNTRY * p_count,struct RastPort * rpG)
{
    int sx=(p_count->c_clip).maxx-(p_count->c_clip).minx;
    int SX=Pref.clip_cur.maxx-Pref.clip_cur.minx;
    int posx,posy;
    if (sx>0)
    {
        if ((SX/sx)<4)
        {
            posx=sx/2+(p_count->c_clip).minx;
            posy=(p_count->c_clip).maxy/2+(p_count->c_clip).miny/2;
            conv_xy(&posx,&posy);
            posx-=4*strlen(p_count->name);
            posy-=4;

            IPrint(rpG,posx,posy,p_count->name,
                    COLOR_AFF_NOM_PAYS+num_first_color);
        }
    }
    else
        {}
        /*printf("pays %s :sx nul\n",p_count->name);*/
}

/***************************************************** clear_map() *****/
void clear_map(struct RastPort *rpG)
{
    SetRast(rpG,COLOR_BLEU+num_first_color);
    if(Object_Fond!=NULL)
    {
        Iff_Copy_Fond(Object_Fond,rpG);
    }
}

/***************************************************** redraw_map() *****/

void 
redraw_map_fil(int     num_continent,struct  RastPort *rpG)
{
    struct RastPort *r;
    int     num_country;
    int     i,j;
    /* Vecteur courant */
    int     num_vect;
    COUNTRY *p_count;
    VECT    *V;
    BOOL    Flg_Draw;
    int     code;
    int     nb_vect;

    
    r = rpG;
    conv_clip();
    clear_map(rpG);
    SetBPen(r,COLOR_COTE+num_first_color);
    SetDrMd(r,JAM1);

    reset_bord();
    Map_Aff_LatLon(rpG);

    /********************************************/
    /* Pour tout les pays de la base de donnees */
    /********************************************/

    for (num_country=0;num_country<max_country;num_country++)
    {

        if (continent_sel[tab_country[num_country].continent]==TRUE)
        {
            /****************************************************/
            /* Pour un pays, on connait le nom, et le nombre de */
            /* bords                                            */
            /****************************************************/

            p_count=&tab_country[num_country];
            if (tstclip(        clip_proj.minx,clip_proj.miny,
                        clip_proj.maxx,clip_proj.maxy,&(p_count->c_clip)))
            {
                Flg_Draw=TRUE;
                for (i=0;i<p_count->nb_borders;i++)
                {
            
                    /*********************************************/
                    /* Pour chaque bord, on affiche les vecteurs */
                    /* correspondants a ce bord                  */
                    /*********************************************/
                    num_vect=(p_count->p_borders)[i];
                    if (tstclip(        clip_proj.minx,clip_proj.miny,
                                clip_proj.maxx,clip_proj.maxy,
                                &(tab_vect[num_vect].b_clip)))
                    {
                        if (tab_vect[num_vect].flg_draw==FALSE)
                        {
                            tab_vect[num_vect].flg_draw=TRUE;

                            if (tab_vect[num_vect].p_vect[0].Code>1000)
                            {
                                    code=tab_vect[num_vect].p_vect[0].Code/1000;
                            }
                            if (((code==ID_LAC)&&(Pref.Flg_Draw_Lac==FALSE))
                                    ||((code==ID_RIVIERES)&&(Pref.Flg_Draw_Riv==FALSE)))
                            {}
                            else
/*
                            {
                                    Flg_Draw=FALSE;
                            }
                            if ((code==ID_RIVIERES)&&(Pref.Flg_Draw_Riv==FALSE))
                            {
                                    Flg_Draw=FALSE;
                            }
                              }
                            if (Flg_Draw)*/
                            {
                                check_border(num_vect,&nb_vect,&V);
                                switch (code)
                                {
                                case ID_LAC:
                                    SetAPen(r,COLOR_LAKES+num_first_color);
                                    break;
                                case ID_RIVIERES:
                                    SetAPen(r,COLOR_RIVERS+num_first_color);
                                    break;
                                case ID_ISLAND:
                                case ID_COTE:
                                    SetAPen(r,COLOR_COAST+num_first_color);
                                    break;
                                default:
                                    SetAPen(r,COLOR_FRONTIERE+num_first_color);
                                }       
                                MyMove(r,V[0].x,V[0].y);
                                for (j=1;j<nb_vect;j++)
                                {
                                    MyDraw(r,V[j].x,V[j].y,V[j].Code);
                                }
                            }
                        }
                    }
/*                  Flg_Draw=TRUE;*/
                }/* fin du for sur les borders      */
                if (Pref.Flg_Aff_Country_Name==TRUE)
                {
                    map_aff_country_name(p_count,rpG);
                }
            }
        }
    }/* Fin du for sur les country          */
    reselect(rpG); /* remplissage du pays sélectionné*/
}

/****** Trace de la carte, mais en plein pour les pays */

void 
redraw_map_plein(int num_continent,struct  RastPort *rpG)
{
    int     i,max;

    max=get_max_country();
    clear_map(rpG);
    Map_Aff_LatLon(rpG);
    for (i=0;i<max;i++)
    {
    if (tab_country[i].continent==num_continent
        || num_continent==-1)
        fill_country(i,COLOR_STATE,rpG);
    }
}

/***************************************************** redraw_map() *****/

void 
redraw_map( int  num_continent, struct  RastPort  *rpG)
{
    /* fait dodo, application! */
    Application_Sleep(TRUE);
    Update_New_Clip();
    if(Pref.Type_Trace==TRACE_FIL)
    {
        redraw_map_fil(num_continent,rpG);
    }
    else
    {
        redraw_map_plein(num_continent,rpG);
    }
    /* il est l'heure de se lever */
    Application_Sleep(FALSE);
}

/***************************************************** redraw_map() *****/

void redraw_map_complete(
        struct  RastPort        *rpG
        )
{
        struct RastPort *r;
        int     i,j;
        /* Vecteur courant */
        VECT    *V;

        r = rpG;
        SetRast(r,COLOR_BLEU+num_first_color);
        if(sC_Image!=NULL){
                ClipBlit( r, 0L, 0L,
                      sC_Image->FirstWindow->RPort, 0L, 0L,
                      Pref.win_width,
                      Pref.win_height,
                      (long)COPY_MINTERM);
        }
        SetAPen(r,num_first_color+COLOR_SELECT_SEG);
        SetBPen(r,COLOR_COTE+num_first_color);
        SetDrMd(r,JAM1);

        for(i=0;i<max_border;i++){
                /*********************************************/
                /* Pour chaque bord, on affiche les vecteurs */
                /* correspondants a ce bord                  */
                /*********************************************/
                if(tstclip(     clip_proj.minx,clip_proj.miny,
                                clip_proj.maxx,clip_proj.maxy,
                                &(tab_vect[i].b_clip))){

                        V=tab_vect[i].p_vect;
                        SetAPen(r,tab_look_up[V[0].Code/1000]+num_first_color);
                        MyMove(r,V[0].x,V[0].y);
                        for(j=1;j<tab_vect[i].nb_vect;j++){
                                MyDraw(r,V[j].x,V[j].y,V[j].Code);
                        }
                }
        }
}

/*****************************************************  *****/

BOOL    
Est_Pres(int x1,int x2)
{
        
        if(ABS(x2-x1)<=20){
                return( TRUE );
        }else{
                return( FALSE );
        }
}

/*****************************************************  *****/

int     
Dist(int x1,int x2,int y1,int y2)
{
        
        return(ABS(x2-x1)+ABS(y2-y1));
}

/***************************************************** fill_country() *****/

void 
fill_country(int  num_country,int color,struct  RastPort  *rpG)
{
   struct RastPort *r;
   long lastx=0,lasty=0;        
   long firstx,firsty;
   int  first,dir;
   BOOL new_area;
   int  i,j;
   int  code,couleur;
   BOOL Flg_Draw;
   /* Vecteur courant */
   VECT *V;
   int  num_vect,nb_vect;
   COUNTRY      *p_count;
   int  Dist_Deb,Dist_Fin;

    code=0;

    p_count=&tab_country[num_country];
    if (tstclip(        Pref.clip_cur.minx,Pref.clip_cur.miny,
                Pref.clip_cur.maxx,Pref.clip_cur.maxy,&(p_count->c_clip)))
    {
        r = rpG;
        SetAPen(r,(long)color+num_first_color);
        SetBPen(r,COLOR_COTE+num_first_color);

/*      SetOPen(r,COLOR_COAST+num_first_color);*/

        SetDrMd(r,JAM1);

        /****************************************************/
        /* Pour un pays, on connait le nom, et le nombre de */
        /* bords                                            */
        /****************************************************/

        new_area=TRUE;

        for (i=0;i<p_count->nb_borders;i++)
        {
            Flg_Draw=TRUE;
        
            /*********************************************/
            /* Pour chaque bord, on affiche les vecteurs */
            /* correspondants a ce bord                  */
            /*********************************************/

            /* nombre de vecteurs du bord */
            num_vect=(p_count->p_borders)[i];

            V=tab_vect[num_vect].p_vect;
            nb_vect=tab_vect[num_vect].nb_vect;

            if(V[0].Code>1000)
            {
                code=V[0].Code/1000;
                /* les lacs ne sont pas remplis */
                if (code==ID_LAC)
                    Flg_Draw=FALSE;
                /* les rivières seront dessinées après*/
                if (code==ID_RIVIERES)
                    Flg_Draw=FALSE;
            }
            check_border(num_vect,&nb_vect,&V);
            if (new_area==TRUE)
            {
                /* border suivant (s'il existe)*/
                VECT    *V_Suivant;
                int     num_vect1,nb_vect1;
                if (i+1<p_count->nb_borders)
                {
                    num_vect1=(p_count->p_borders)[i+1];
                    check_border(num_vect1,&nb_vect1,&V_Suivant);                           /* SI il y a plus d'un vecteur, on regarde le cas du */
                }
                dir=1;  /* direction dans laquelle on parcourt les vecteurs*/
                first=0;/* numero du vecteur par lequel on commence        */
                new_area=FALSE;
                /* premier point                                                     */
        
                if ((nb_vect>1)&&( i+1 < p_count->nb_borders))
                {

/*                  nb_vect1=tab_vect[num_vect1].nb_vect;
                    V_Suivant=tab_vect[num_vect1].p_vect;*/

                    /* Si le dernier point du premier segment*/
                    /* egal au premier pt du segment suivant */
                    /* ou si le dernier pt du premier seg.   */
                    /* egal au dernier pt segmet suivant,    */
                    /* on parcours le premier segment en montant */
                    /* sinon on le parcours en descendant    */


                    if((Est_Pres( V[nb_vect-1].x,V_Suivant[0].x )&&
                        Est_Pres( V[nb_vect-1].y,V_Suivant[0].y ) ) ||
                       (Est_Pres( V[nb_vect-1].x,V_Suivant[nb_vect1-1].x)&&
                        Est_Pres( V[nb_vect-1].y,V_Suivant[nb_vect1-1].y)
                       ))
                    {

                        first=0;
                        dir=1;
                    }
                    else
                    {
                        first=nb_vect-1;
                        dir=-1;
                    }
                }
                firstx=V[first].x;
                firsty=V[first].y;
                if (Flg_Draw)
                    MyAreaMove(r,firstx,firsty);
            }
            else
            {
                /************************************************/
                /* Un bord a deja ete trace                             */
                /* On regarde lequel des deux extremites            */
                /* pt du segment suivant s'attachent a ce seg.  */
                /************************************************/

                Dist_Deb=Dist(lastx,V[0].x        ,lasty,V[0].y       );
                Dist_Fin=Dist(lastx,V[nb_vect-1].x,lasty,V[nb_vect-1].y);
                /* si min(Dist_Deb,Dist_Fin)>100,       */
                /* les vecteurs sont trop loins,        */
                /* il faut fermer l'ancienne zone       */
                /* et en commencer une nouvelle         */
                if (Dist_Deb<=Dist_Fin)
                {
                    dir=1;
                    first=0;
                }
                else
                {
                    dir=-1;
                    first=nb_vect-1;
                }
            }
            /* On trace tous les vecteurs de ce segment */
            for (j=1;j<nb_vect;j++)
            {
                first+=dir;
                if (Flg_Draw)
                    MyAreaDraw(r,V[first].x,V[first].y,V[first].Code);
            }

            /* Pour rechercher le debut du prochain segment */
            lastx=V[first].x;
            lasty=V[first].y;

            /* Si on boucle, cela veut dire qu'il faut recommencer une nouvelle */
            /* aire (cas des iles par exemple                                   */
            if( (Est_Pres(lastx,firstx)&&Est_Pres(lasty,firsty) )||
                            (i== p_count->nb_borders-1))
            {
                new_area=TRUE;
                if (Flg_Draw)
                    MyAreaEnd(r);
            }
/*          else*/
            /* si le prochain border est trop loin, il recommencer  */
            /* une nouvelle aire                                                                    */
/*          if (i+1<p_count->nb_borders)
                if (ABS(V[nb_vect-1].x-V_Suivant[0].x)
                        +ABS(V[nb_vect-1].y-V_Suivant[0].y) > 500
                        &&
                        ABS(V[nb_vect-1].x-V_Suivant[nb_vect1-1].x)
                        +ABS(V[nb_vect-1].y-V_Suivant[nb_vect1-1].y) > 500)
                {
                        new_area=TRUE;
                        if (Flg_Draw)
                                MyAreaEnd(r);
                }       */                      
        }
        
        if (new_area==FALSE)
            MyAreaEnd(r);

        for (i=0;i<p_count->nb_borders;i++)
        {
            /* nombre de vecteurs du bord */
            num_vect=(p_count->p_borders)[i];

            V=tab_vect[num_vect].p_vect;
/*          nb_vect=tab_vect[num_vect].nb_vect;*/

            Flg_Draw=FALSE;
            if (V[0].Code>1000)
            {
                code=V[0].Code/1000;
                if ((code!=ID_COTE)&&(code!=ID_RIVIERES)&&(code!=ID_LAC))
                {
                    Flg_Draw=TRUE;
                    couleur=COLOR_FRONTIERE;
                }
                if((code==ID_RIVIERES)&&(Pref.Flg_Draw_Riv==TRUE))
                {
                        Flg_Draw=TRUE;
                        couleur=COLOR_LAKES;
                }
                if((code==ID_LAC)&&(Pref.Flg_Draw_Lac==TRUE))
                {
                        Flg_Draw=TRUE;
                        couleur=COLOR_RIVERS;
                }
                if ((code==ID_COTE)||(code==ID_ISLAND))
                {
                        Flg_Draw=TRUE;
                        couleur=COLOR_COAST;
                }                               
            }
            if(Flg_Draw)
            {
        /*      if ( (Pref.Flg_Draw_Riv==TRUE)&& (code==ID_RIVIERES))*/
                map_draw_segment(
                    rpG,
                    num_vect,
                    couleur);
            }
        }
        
        if (Pref.Flg_Aff_Country_Name==TRUE)
                map_aff_country_name(p_count,rpG);
        if (Pref.Flg_Aff_Elem_Rem==TRUE)
                aff_elem_rem(rpG,num_country);
    }
}

/*************************************************************************/

void    DrawSeg(struct  RastPort        *rpG,
                VECT    tab_pt[],
                int     pt1,
                int     pt2,
                int     pt3
                )
{
        
        MyMove(rpG,tab_pt[pt1].x,tab_pt[pt1].y);
        MyDraw(rpG,tab_pt[pt2].x,tab_pt[pt2].y,tab_pt[pt2].Code);
        MyDraw(rpG,tab_pt[pt3].x,tab_pt[pt3].y,tab_pt[pt3].Code);
}



/*************************************************************************/

int     map_find_pt(
        struct  RastPort        *rpG,
        int     x,
        int     y,
        VECT    **pp_v,
        int     *p_num_pt,
        int     *p_num_border
        )
{
        int     flg_found,i,j,dx,dy;
        VECT    *v;
        int     dmaxx,dmaxy;
        int     x0,y0;
        int     x1,y1;

        i=0;
        flg_found=FALSE;
        /*printf("x:%d y:%d\n",x,y);*/

        conv_inv_xy(&x,&y);

        x0=x     ; y0=y;
        x1=y+DMAX; y1=y+DMAX;

        conv_inv_xy(&x0,&y1);
        conv_inv_xy(&x1,&y1);
        dmaxx=x1-x0;
        dmaxy=y1-y0;

        /*printf("x:%d y:%d\n",x,y);*/
        do{
           if(tstclip(  Pref.clip_cur.minx,Pref.clip_cur.miny,
                        Pref.clip_cur.maxx,Pref.clip_cur.maxy,&(tab_vect[i].b_clip))){
                j=0;
                v=tab_vect[i].p_vect;

                do{
                        dx=x-v[j].x;
                        dy=y-v[j].y;
/*
                        printf("vx:%4d vy:%4d %4d %4d\n",v[j].x,v[j].y,dx,dy);
*/
                        if((dx<dmaxx)&&(dx>-dmaxx)&&(dy<dmaxy)&&(dy>-dmaxy)){
                                flg_found=TRUE;
                                /*printf("Found pt!\n");*/
                                *pp_v=&v[j];
                                *p_num_pt=j;
                                *p_num_border=i;
                                SetAPen(rpG,num_first_color+COLOR_TEXTE);
                                x0=v[j].x;
                                y0=v[j].y;
                                conv_xy(&x0,&y0);
                                map_draw_rect(rpG,x0-10,y0-10,20,20);
                                SetAPen(rpG,1L);
                        }
                        j++;

                }while((j<tab_vect[i].nb_vect)&&(flg_found==FALSE));
             }
             i++;
        }while((i<max_border)&&(flg_found==FALSE));
        return(flg_found);
}

/*************************************************************************/

void    map_show_pt(struct      RastPort        *rpG)
{
        int     i,j;
        VECT    *v;
        int     x,y;
        int     nb_vect;
        
        SetDrMd(rpG,JAM2);
        SetAPen(rpG,10L);
        for(i=0;i<max_border;i++){
            if(tstclip( Pref.clip_cur.minx,Pref.clip_cur.miny,
                        Pref.clip_cur.maxx,Pref.clip_cur.maxy,&(tab_vect[i].b_clip))){

                check_border(i,&nb_vect,&v);
                for(j=0;j<tab_vect[i].nb_vect;j++){
                        x=v[j].x;
                        y=v[j].y;
                        conv_xy(&x,&y);
                        map_draw_rect(rpG,x-2,y-2,5,5);
                };
            };
        };
        SetAPen(rpG,1L);
}

/*************************************************************************/

void    map_show_borders(struct RastPort        *rpG)
{
        int     i,max,x,y;
        VECT    *v;
        char    Buffer[10];

        SetDrMd(rpG,JAM2);
        SetAPen(rpG,11L);
        for(i=0;i<max_border;i++){
            if(tstclip( Pref.clip_cur.minx,Pref.clip_cur.miny,
                        Pref.clip_cur.maxx,Pref.clip_cur.maxy,&(tab_vect[i].b_clip))){
                v=tab_vect[i].p_vect;

                x=v[0].x;
                y=v[0].y;
                conv_xy(&x,&y);
                map_draw_rect(rpG,x-2,y-2,5,5);
                max=tab_vect[i].nb_vect-1;
                x=v[max].x;
                y=v[max].y;
                conv_xy(&x,&y);
                map_draw_rect(rpG,x-2,y-2,5,5);

                sprintf(Buffer,"%d",i);
                x=v[0].x;
                y=v[0].y;
                conv_xy(&x,&y);
                MyMove(rpG,x+5,y+5);
                Text(rpG,Buffer,strlen(Buffer));
            };
        };
        SetAPen(rpG,1L);
}

/*************************************************************************/

void    map_show_bords(int country_sel,int start_bord,struct RastPort *rpG)
{
        int     i,num_bord;
        VECT    *v;

        num_bord=(tab_country[country_sel].p_borders)[start_bord];
        /*printf("cmd schow bord de %d\n",num_bord);*/
        SetAPen(rpG,10);
        v=tab_vect[num_bord].p_vect;
        for(i=0;i<tab_vect[num_bord].nb_vect;i++){
                if(i==0){
                        MyMove(rpG,v->x,v->y);
                }else{
                        MyDraw(rpG,v->x,v->y,v->Code);
                }
                v++;
        }
}

/*************************************************************************/

void    map_reset_zoom(struct RastPort *rpG)
{
        Pref.clip_cur=clip_max;
        Pref.prof=Find_Profondeur(Pref.prof_real);
        redraw_map(-1,rpG);
}

/*************************************************************************/

void    exchg(int       *a,int  *b)
{
        int     t;
        t=*a;
        *a=*b;
        *b=t;
}

/*************************************************************************/
void map_set_zoom(struct RastPort       *rpG,
                int     sx,int sy,
                int     lx,int ly
                )
{
        int     minxt,minyt,maxxt,maxyt;

        minxt=sx;
        minyt=sy;
        maxxt=lx;
        maxyt=ly;
        conv_inv_xy(&minxt,&minyt);
        conv_inv_xy(&maxxt,&maxyt);
        /* Il faut faire une copie temporaire de minx...maxy */
        /* car ce sont des variables globales utilisee par   */
        /* conv_in                                                                                   */
        if(minxt>maxxt)exchg(&minxt,&maxxt);
        if(minyt>maxyt)exchg(&minyt,&maxyt);
        Pref.clip_cur.minx=minxt;
        Pref.clip_cur.miny=minyt;
        Pref.clip_cur.maxx=maxxt;
        Pref.clip_cur.maxy=maxyt;
        /*printf("minx:%d miny:%d maxx:%d maxy:%d\n",minxt,minyt,maxxt,maxyt);*/
        Pref.prof=Find_Profondeur(Pref.prof_real);
        redraw_map(-1,rpG);
}

/*************************************************************************/
void map_move(struct RastPort   *rpG,
                int     sx,int sy
                )
{
    int SX; /*=sx*(Pref.clip_cur.maxx-Pref.clip_cur.minx)/100;*/
    int SY; /*=sy*(Pref.clip_cur.maxy-Pref.clip_cur.miny)/100;*/
        int X1=Pref.clip_cur.maxx;
        int X2=Pref.clip_cur.minx;
        int Y1=Pref.clip_cur.maxy;
        int Y2=Pref.clip_cur.miny;

        if (Pref.Type_Proj==PROJ_GLOBE)
        {
                map_center(rpG,
                        ((50-sx)*Pref.clip_cur.minx
                                        +(50+sx)*Pref.clip_cur.maxx)/100,
                        ((50-sy/2)*Pref.clip_cur.miny
                                        +(50+sy/2)*Pref.clip_cur.maxy)/100);
        }
        else
        {
                conv_xy(&X1,&Y1);
                conv_xy(&X2,&Y2);
                SX=sx*(X1-X2)/100;
                SY=sy*(Y1-Y2)/100;
                X1+=SX;
                Y1+=SY;
                Y2+=SY;
                X2=(Pref.win_width)*(Y2-Y1)/(Pref.win_height)+X1;
                conv_inv_xy(&X1,&Y1);
                conv_inv_xy(&X2,&Y2);
        Pref.clip_cur.minx=X2;
        Pref.clip_cur.maxx=X1;
        Pref.clip_cur.miny=Y2;
        Pref.clip_cur.maxy=Y1;
                correct_clip(&Pref.clip_cur);    
                conv_clip();
                redraw_map(-1,rpG);
        }
}

/*************************************************************************/
void map_center(struct RastPort *rpG,
                int     X,
                int Y
                )
{
        int sx=(Pref.clip_cur.maxx-Pref.clip_cur.minx)/2;
        int sy=(Pref.clip_cur.maxy-Pref.clip_cur.miny)/2;
    Pref.clip_cur.minx=X-sx;
    Pref.clip_cur.maxx=X+sx;
    Pref.clip_cur.miny=Y-sy;
    Pref.clip_cur.maxy=Y+sy;
        conv_clip();    
        redraw_map(-1,rpG);
}


/***************************************************** redraw_map() *****/
/*************************************************************/
/* Reaffiche la carte, mais avec des donnees sur cette carte */
/*************************************************************/

void redraw_map_data(
        struct  RastPort        *rpG
        )
{
        struct RastPort *r;
        int     num_country;


        r = rpG;
        SetRast(r,COLOR_BLEU+num_first_color);
        SetBPen(r,COLOR_COTE+num_first_color);
        SetDrMd(r,JAM1);

        reset_bord();
        /********************************************/
        /* Pour tout les pays selectionnes          */
        /********************************************/

        for(num_country=0;num_country<max_country;num_country++){
                
                
        }/* Fin du for sur les country          */

}

void    Map_Set_Spread_Colors(struct ViewPort *vp)
{
        int     i;
        UWORD   r,g,b;

        for(i=0;i<(nb_colors_free-2);i++){
                r=16-i;
                g=r;
                b=r/2;
                SetRGB4(vp,i+num_first_color+2,r,g,b);
        }
        
}

/****************************************************/
/* Fonction a appeler pour initialiser un graphique */
/****************************************************/
/* Cette fonction retourne le nombre de plage dispo */
/* suivant le type de graphique demandee, et le     */
/* nombre de couleur dispo                          */

void Map_Init_Graph(struct RastPort *rpG,char *Titre)
{
        /*printf("Map_Init_Graph\n");*/

        if (Titre_Font_Glob)
            SetFont(rpG,Titre_Font_Glob);
            SetSoftStyle(rpG,(FSF_BOLD | FSF_UNDERLINED | FSF_ITALIC),
                (FSF_BOLD ));

            IPrint(rpG,Pref.win_width/10,30,Titre,-1);
    if (Text_Font_Glob)
        SetFont(rpG,Text_Font_Glob);
        if(Pref.Flg_Proj_3D==FALSE)SetRast(rpG,0L);

/*
        Map_Set_Spread_Colors(vp_glob);
*/      
}

/****************************************************/
/* Nouvelle donnee a afficher                       */
/****************************************************/

void Map_Aff_Data_3D(struct RastPort *rpG,DATA_TRI *Data_Tri,int value)
{
        static  int     Tab_Pt[][3]={
                {-2,-2,  0},{-2,2,  0},{ 2,2,  0},{ 2,-2,  0},
                {-2,-2,200},{-2,2,200},{ 2,2,200},{ 2,-2,200}
                        };
        static  int     Tab_Faces[6][5]=
                {
                {0,1,2,3,4},
                {0,3,5,4,5},
                {3,2,6,7,6},
                {2,1,5,6,7},
                {1,0,4,5,8},
                {7,6,5,4,9}
                };
        static  int     Tab_Pt_2d[100][2];

        int     i,j,x,y,z,xc,yc,xe,ye,n,n1,n2,n3;
        CLIP    *clip;

           if(value>0){
                
                fill_country(Data_Tri->Num_Pays,value,rpG);
                Pref.Flg_Proj_3D=FALSE;
                clip=&tab_country[Data_Tri->Num_Pays].c_clip;
                xc=(clip->maxx+clip->minx)/2;
                yc=(clip->maxy+clip->miny)/2;
                conv_xy(&xc,&yc);
                
                Tab_Pt[4][2]=-value*5;
                Tab_Pt[5][2]=-value*5;
                Tab_Pt[6][2]=-value*5;
                Tab_Pt[7][2]=-value*5;

                for(i=0;i<8;i++){
                        x=Tab_Pt[i][0]+xc;
                        y=Tab_Pt[i][1]+yc;
                        z=Tab_Pt[i][2];
                        
                        T3D_ConvertZ(x,y,z,&xe,&ye);
                        Tab_Pt_2d[i][0]=xe;
                        Tab_Pt_2d[i][1]=ye;
                }
                for(j=0;j<6;j++){
                {
                        n1=Tab_Faces[j][0];
                        n2=Tab_Faces[j][1];
                        n3=Tab_Faces[j][2];

                        if((( Tab_Pt_2d[n2][1] - Tab_Pt_2d[n1][1])*
                            ( Tab_Pt_2d[n2][0] - Tab_Pt_2d[n3][0])
                          - ( Tab_Pt_2d[n2][0] - Tab_Pt_2d[n1][0])*
                            ( Tab_Pt_2d[n2][1] - Tab_Pt_2d[n3][1]))>0){

                                SetAPen(rpG,Tab_Faces[j][4]);
                                for(i=0;i<4;i++){
                                        n=Tab_Faces[j][i];
                                        x=Tab_Pt_2d[n][0];
                                        y=Tab_Pt_2d[n][1];
                                        if(i==0){
                                                AreaMove(rpG,x,y);
                                        }else{
                                                AreaDraw(rpG,x,y);
                                        }
                                }
                                AreaEnd(rpG);
                        }
                                                                                                                }
                }
                Pref.Flg_Proj_3D=TRUE;
              }

}

/****************************************************/

void Map_Aff_Data(struct RastPort *rpG,DATA_TRI *Data_Tri,int value)
{
        /*int   num;*/
        int py,num_pays;
        char    Buffer[100];
        int     Val;
        int     debx=220;
        
        if(Pref.Flg_Proj_3D==FALSE){
           if(Data_Tri->valeur.data_int!=0){
                num_pays=Data_Tri->Num_Pays;
                py=Data_Tri->Rang*10+60;
                IPrint(rpG,10,py,tab_country[num_pays].name,-1);
                SetAPen(rpG,7);
                SafeSetOutlinePen(rpG,6);
                Val=debx+(int)((float)(Data_Tri->valeur.data_int))*((float)(Pref.win_width-(debx+20)))/Pref.Glob_Max;

                if(Glob_Type==GRAPH_BAR){
                        RectFill(rpG,debx,py-7,Val,py);
                }
/*              RectFill(rpG,200,py,200+Data_Tri->valeur.data_int/100,py+7);*/
                sprintf(Buffer,"%d",Data_Tri->valeur.data_int);
                IPrintFond(rpG,150,py,Buffer,-1);
                BNDRYOFF(rpG);
           }
        }else{
                Map_Aff_Data_3D(rpG,Data_Tri,value);
        }
}

/****************************************************/

void    Map_End_Graph(struct RastPort *rpG)
{
        /*printf("Map_End_Graph\n");*/
        /*Aff_Error("Graph ok","End");*/

        /*printf("Map_End_Graph\n");*/
}

/****************************************************/

void    Map_Set_Default_Colors(struct ViewPort *vp)
{
        static  BOOL    Flg_First=TRUE;
        static  ULONG   *Palette_Deb;
        int     i;
        UWORD   r,g,b;

        static UWORD Palette[]={
                0x0ff,          /* 0= Bleu Mer */
                0x0f0,          /* 1=vert*/
                0x06f,          /* 2=Rivers     bleu foncé*/
                0x00f,          /* 3=Lakes      */
                0x085,          /* 4=state      */              
                0xd74,          /* 5=brun frontieres*/
                0xf00,          /* 6=rouge   pour les dégradés*/
                0xf66,          /* 7=rouge 2*/
                0xf98,          /* 8=rouge 3*/
                0xc98,          /* 9=rouge 4*/
                0xd83,          /* 10=rouge 5*/
                0xdb3           /* 11=rouge 6*/
        };
        if(Flg_First){
                Flg_First=FALSE;
                Palette_Deb=(ULONG *)malloc(num_first_color*sizeof(ULONG));
                if(Palette_Deb){
                        for (i=0;i<num_first_color;i++){
                                Palette_Deb[i]=GetRGB4(vp->ColorMap,i);
                        }
                }
        }else{

            if(Palette_Deb){
                for (i=0;i<num_first_color;i++)
                {
                        r=( Palette_Deb[i]>>8 )& 0x0F;
                        g=( Palette_Deb[i]>>4 )& 0x0F;
                        b=( Palette_Deb[i]>>0 )& 0x0F;
                        SetRGB4(vp,i,r,g,b);
                }
           }

        }

        for (i=0;i<nb_colors_free;i++)
    {
                r=( Palette[i]>>8 )& 0x0F;
                g=( Palette[i]>>4 )& 0x0F;
                b=( Palette[i]>>0 )& 0x0F;
                SetRGB4(vp,i+num_first_color,r,g,b);
        }
        vp_glob=vp;     
}

/****************************************************/

int  Map_Get_Plages(    struct  RastPort *rpG   ,
                        int     plages_dem      ,
                        E_Type_Data Type        ,
                        int nb_data,
                        int     min             ,
                        int     max             ,
                        E_Type_Graph In_Type
                )
{
        Type_Data_Glob=Type;
        Pref.Glob_Min=min;
        Pref.Glob_Max=max;
        Glob_Type=In_Type;
        return(10);
}


/****************************************************/

/* Give default font used to draw information */

void    Map_Give_Default_Font( 
        struct TextFont *Titre_Font,
        struct TextFont *Text_Font
        )
{
        Titre_Font_Glob=Titre_Font;
        Text_Font_Glob =Text_Font;
}

/************************************************************************/
/* Trace le quadrillage correspondant aux latitudes et longitudes       */
/************************************************************************/


void    
Map_Aff_LatLon(struct RastPort *rpG)
{
    int     Lat,Lon;
    int code;                                
    if (Pref.Flg_Aff_LatLon)
    {
        SetDrPt(rpG,0xCCCC);
        SetAPen(rpG,COLOR_LATLON+num_first_color);
        for (Lon=-180;Lon<=180;Lon+=1)
        {
            /* traits verticaux */
            if( (Pref.clip_cur.minx < Lon*100)&& (Pref.clip_cur.maxx > Lon *100))
            {
                if (Lon % 20 == 0)
                    code = 5;
                else
                if (Lon % 5 == 0)
                    code = 4;
                else
                    code = 3;
                if (code >= Pref.prof)
                {
                    MyMove(rpG,Lon*100,MAX(Pref.clip_cur.miny,-9000));
                    for (Lat=MAX(Pref.clip_cur.miny/100,-90);
                            Lat<=MIN(Pref.clip_cur.maxy/100,90);Lat+=1)
                    {
                        short int code2;
                        if (Lat%20 == 0)
                            code2=code;
                        else
                        if (Lat%5 == 0)
                            code2=code-1;
                        else
                            code2=code-2;
                        MyDraw(rpG,Lon*100,Lat*100,code2);
                    }
                    MyDraw(rpG,Lon*100,MIN(Pref.clip_cur.maxy,9000),5);
                }
            }
        }
        for (Lat=-90;Lat<=90;Lat+=1)
        {
            if ( (Pref.clip_cur.miny < Lat*100)&& (Pref.clip_cur.maxy > Lat *100))
            {
                if (Lat % 20 == 0)
                    code = 5;
                else
                if (Lat % 5 == 0)
                    code = 4;
                else
                    code = 3;
                if (code>=Pref.prof)
                {
                    MyMove(rpG,MAX(Pref.clip_cur.minx,-18000),Lat*100);
                    for (Lon=MAX(Pref.clip_cur.minx/100,-180);
                            Lon<=MIN(Pref.clip_cur.maxx/100,180);Lon+=1)
                    {
                        short int code2;
                        if (Lon%20 == 0)
                            code2=code;
                        else
                        if (Lon%5 == 0)
                            code2=code-1;
                        else
                            code2=code-2;
                        MyDraw(rpG,Lon*100,Lat*100,code2);
                    }
                    MyDraw(rpG,MIN(Pref.clip_cur.maxx,18000),Lat*100,5);
                }
            }
        }
        SetDrPt(rpG,0xFFFF);
    }
}

void    Map_Aff_Fond(char *pc,struct Screen *sC)
{
        Map_Del_Fond();

        Object_Fond=Iff_New_Charge_Iff(pc,sC);

}
/********** Del Screen Image *****************************/
void Map_Del_Fond(void)
{
        if(Object_Fond){
                Iff_Free_Obj(Object_Fond);
        }
        Object_Fond=NULL;
}


void Map_Aff_Echelle(struct RastPort * rpG)
{
        char zero[4];
        char cent[15];
        int y=(Pref.clip_cur.maxy+Pref.clip_cur.miny)/2;
        int sx=Pref.clip_cur.maxx-Pref.clip_cur.minx;
        float alpha=Map_Convert_Angle(y);
        int perimetre=2*PI*RAYON;
        long int largeur=perimetre*sx*cos(alpha)/36000;
        int puissance=log10(0.9*largeur);
        int max_echelle=pow(10.0,(double)puissance);
        int longueur_echelle=max_echelle*sC->Width/largeur;
        SetAPen(rpG,COLOR_TEXTE);
        Move(rpG, 10,(sC->Height)-20);
        Draw(rpG, 10+longueur_echelle,(sC->Height)-20);
        Move(rpG, 10,(sC->Height)-23);
        Draw(rpG, 10,sC->Height-17);
        Move(rpG, 10+longueur_echelle,sC->Height-23);
        Draw(rpG, 10+longueur_echelle,sC->Height-17);
        sprintf(zero,"0");
        IPrintFond(rpG, 6, sC->Height-25,zero,COLOR_TEXTE);
        sprintf(cent,"%d km",max_echelle);
        IPrintFond(rpG, 10+longueur_echelle-strlen(cent)*4,sC->Height-25,cent,
                        COLOR_TEXTE);
}

void Map_Aff_Trajectoires(struct RastPort * rpG,TRAJECTOIRE ** tableau)
/* affiche les trajectoires d'un tableau*/
/* si tableau==NULL, on prend tab_trajectoire*/
{
    int num_traj=0;
    int j;
    if (tableau==NULL)
            tableau=tab_trajectoire;
    while (tableau[num_traj]!=NULL)
    {
        if (tstclip(        Pref.clip_cur.minx,Pref.clip_cur.miny,
                    Pref.clip_cur.maxx,Pref.clip_cur.maxy,
                    &(tableau[num_traj]->clip)))
        {
                    VECT *V=tableau[num_traj]->vecteurs;
                    SetAPen(rpG,6+num_first_color);
            MyMove(rpG,V[0].x,V[0].y);
            for (j=1;j<tableau[num_traj]->nb_vect;j++)
            {
                    MyDraw(rpG,V[j].x,V[j].y,V[j].Code);
            }
        }
        num_traj++;
    }
}

