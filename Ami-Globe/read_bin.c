/************************************************************************/
/*                                                                      */
/*      fichier         : read_bin.c                                    */
/*      projet          : amiglobe                                      */
/*      date création   : 27/08/94                                      */
/*      commentaire     : lecture des fichiers new_lac.Pt_bin           */
/*                        création du tableau des points                */
/*      révision        : $VER: read_bin.c 1.002 (27 Aug 1994) */
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*      includes                                                        */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exec/types.h>
#include <stat.h>
#include <libraries/dos.h>
#include <clib/dos_protos.h>

#include "amiglobe_types.h"
#include "read_bin.h"
#include "database_types.h"
#include "map_function_protos.h"
#include "map_function_types.h"
#include "database_protos.h"

/************************************************************************/
/*      defines                                                         */
/************************************************************************/

#define NB_MAX_PT_LIGNE 7000    /* Defini le nombre maximum de pt dans un segment */
                                /* , utilise lors de read map seulement           */

/************************************************************************/
/*      variables externes                                              */
/************************************************************************/
extern CLIP clip_max;
extern BORDER  *tab_vect;
extern BPTR    bptr_big;
extern int     max_border;
extern int     max_country;          /* Nombre max de pays       */
extern COUNTRY *tab_country;
short int * tableau=NULL;
int taille_tableau=0;
/*********************************************************** read_map() ***/

BOOL 
read_map(char *p_filename)
{
    char    Buffer_Name[100];
    FILE    *input;
    int     x, y,code;
    char    c1,c2,c;
    int     mx,my,MX,MY;
    int     i;

    int     flg_border,flg_zone;
    int     flg_end;
    int     num_bord;
    int     Last_Code=0;
    /* Vecteur courant */
    struct  vect *tab_v;
    struct  vect *v;
    int     nb_vect;
    BOOL    Flg_Err=TRUE;


    flg_end=FALSE;

    sprintf(Buffer_Name,"%s.pt",p_filename);

    if (input = fopen(Buffer_Name,"r"))
    {
        /* On alloue la mémoire pour le buffer de lecture ligne */
        tab_v=(VECT *)malloc((NB_MAX_PT_LIGNE)*sizeof(struct vect));

        /* Premiere chose, la taille max de notre carte...*/

        fscanf(input,"%ld%ld%ld%ld",
                &clip_max.minx,&clip_max.miny,
                &clip_max.maxx,&clip_max.maxy);
/*              Pref.clip_cur=clip_max;*/



        /********************************************************/
        /* Puis la liste des bords de pays. C'est une liste     */
        /* de points terminee par le couple -1 -1               */
        /********************************************************/


        num_bord=0;
        flg_zone=TRUE;

        do
        {
#ifdef  TOMDEBUG
            printf("Vecteur numero:%d\n",num_bord);
#endif
            v=tab_v;

            /* Lecture de composantes d'un bord */
            mx=100000;
            my=100000;
            MX=-100000;
            MY=-100000;

            flg_border=TRUE;
            nb_vect=0;
            do
            {
                c1=getc(input);
                c2=getc(input);
                if(c2=='#')
                {
                    do
                    {
                            c=getc(input);
                    }  while((c!=0x0a)&&(c!=EOF));
                    flg_border=FALSE;

                }
                else
                {
                    ungetc(c2,input);
                    ungetc(c1,input);
            
                    fscanf(input,"%ld %ld %ld",&code,&x,&y);
                    v->x=x;
                    v->y=-y;
                    v->Code=code;
                    tstpt(v->x,v->y,&mx,&my,&MX,&MY);
/*                  printf("x:%d y:%d  -> :%ld %ld\n",x,y,v->x,v->y);
*/
                    v++;
                    nb_vect++;
                }
                if(nb_vect>=NB_MAX_PT_LIGNE)
                {
                    printf("Error :max number of pt exceeded -> %d \n",num_bord);
                    flg_border=FALSE;
                }
            }  while(flg_border==TRUE);

            /* On s'assure que le dernier point est bien de niveau 5 */
            if (nb_vect>1)
                v[-1].Code=5;

            /* On s'assure que le code ud premier pt est bien */
            /* un indicateur de nveau segment                 */

            if(tab_v[0].Code<1000)
            {
                tab_v[0].Code=Last_Code;
            }
            Last_Code=tab_v[0].Code;

            tab_vect[num_bord].p_vect=(VECT *)malloc((nb_vect+10)*
                                            sizeof(struct vect));
            for(i=0;i<nb_vect;i++)
            {
                tab_vect[num_bord].p_vect[i]=tab_v[i];
            }
            tab_vect[num_bord].b_clip.minx=mx;
            tab_vect[num_bord].b_clip.miny=my;
            tab_vect[num_bord].b_clip.maxx=MX;
            tab_vect[num_bord].b_clip.maxy=MY;

/*                      printf(" Il y a eu:%d pt dans le bord %d\n",nb_vect,num_bord);
            printf("Code:%d Numero:%d\n",tab_v[0].Code,num_bord);
*/

            tab_vect[num_bord].nb_vect=nb_vect;
            if(num_bord>MAX_BORDER)
            {
                    printf("ERREUR, max number of border exceeded!\n");
            }
            else
            {
                    num_bord++;
            }
            if(tst_end(input)==TRUE)
            {
                    flg_zone=FALSE;
            }
        }  while(flg_zone==TRUE);
        Flg_Err=FALSE;  
        max_border=num_bord;
        free(tab_v);
    }
    return(Flg_Err);
}


/*********************************************************** read_map() ***/

BOOL 
read_map_bin(char *p_filename)
{
    char    Buffer_Name[100];
    char    Buffer_Name_Big[100];
    FILE    *input;
    int     mx,my,MX,MY;
    int     i;

    int     flg_zone;
    int     flg_end;
    int     num_bord;
    int     Last_Code=0;
    /* Vecteur courant */
    short   int     *tab_temp;
    char    *p_deb;
    struct  vect *v;
    short   int     nb_vect;
    int     nb_read,taille;
    struct  stat    my_stat;
    short   int     minx_real,miny_real,maxx_real,maxy_real;
    BOOL    Flg_Err=TRUE;


    flg_end=FALSE;

    sprintf(Buffer_Name,"%s.pt_bin_small",p_filename);
    sprintf(Buffer_Name_Big,"%s.pt_bin",p_filename);

    if ((input = fopen(Buffer_Name,"r"))
            &&(bptr_big=Open(Buffer_Name_Big,MODE_OLDFILE)))
    {
        /* Premiere chose, la taille max de notre carte...*/

        fread(&minx_real,sizeof(short int),1,input);
        fread(&miny_real,sizeof(short int),1,input);
        fread(&maxx_real,sizeof(short int),1,input);
        fread(&maxy_real,sizeof(short int),1,input);
        clip_max.minx=minx_real;
        clip_max.miny=miny_real;
        clip_max.maxx=maxx_real;
        clip_max.maxy=maxy_real;
/*              clip_cur=clip_max;*/



        stat(Buffer_Name,&my_stat);
        
        taille=my_stat.st_size;
        taille=taille-4*sizeof(short int);



        tab_temp=(short int *)malloc(taille);
        tableau=tab_temp;
        taille_tableau=taille;
        if (tab_temp==NULL)
        {
            printf("ERROR: Not Enough Memory!\n");
        }
        else
        {
            nb_read=fread(tab_temp,1,taille,input);
            /* lecture du reste du fichier */
            if (nb_read!=taille)
            {
                printf("ERROR: File truncated\n");
            }
            p_deb=(char *)tab_temp;


            num_bord=0;
            flg_zone=TRUE;

            do
            {
#ifdef  TOMDEBUG
                printf("Vecteur numero:%d\n",num_bord);
#endif
                /* Lecture de composantes d'un bord */
                mx=100000;
                my=100000;
                MX=-100000;
                MY=-100000;

                /* Recupere la position ds le grand fichier */
                tab_vect[num_bord].offset=*((LONG *)tab_temp);
                tab_vect[num_bord].p_vect_maxres=NULL;
                tab_vect[num_bord].nb_vect_bis=-1;
                tab_temp+=sizeof(LONG)/sizeof(short int);

                nb_vect=*tab_temp++;/* 5eme mot = nb de vecteurs */
                tab_vect[num_bord].p_vect=(struct vect *)tab_temp;
                tab_temp+=nb_vect*3;
                v=tab_vect[num_bord].p_vect;

                /* On cree la zone de clipping, mais en */
                /* excluant les lacs et rivieres        */
                for (i=0;i<nb_vect;i++)
                {
                    tstpt(v->x,v->y,&mx,&my,&MX,&MY);
                    v++;
                }

                v=tab_vect[num_bord].p_vect;                
                /* On s'assure que le dernier point est bien de niveau 5 */
                if (nb_vect>1)
                    v[nb_vect-1].Code=5;

                /* On s'assure que le code du premier pt est bien */
                /* un indicateur de nouveau segment                       */

                if (tab_vect[num_bord].p_vect[0].Code<1000)
                {
                    tab_vect[num_bord].p_vect[0].Code=Last_Code;
                }
                Last_Code=tab_vect[num_bord].p_vect[0].Code;

                tab_vect[num_bord].nb_vect=nb_vect;

                tab_vect[num_bord].b_clip.minx=mx;
                tab_vect[num_bord].b_clip.miny=my;
                tab_vect[num_bord].b_clip.maxx=MX;
                tab_vect[num_bord].b_clip.maxy=MY;

/*
                printf(" Il y a eu:%d pt dans le bord %d\n",nb_vect,num_bord);
*/

                if (num_bord>MAX_BORDER)
                {
                    printf("ERROR, max number of border exceeded!\n");
                }
                else
                {
                    num_bord++;
                }
                if ((char *)tab_temp>=taille+p_deb)
                {
                    flg_zone=FALSE;
                }
            }  while(flg_zone==TRUE);
            max_border=num_bord;
            Flg_Err=FALSE;
        }
    }
    return Flg_Err;
}


/*********************************************************** read_zones() ***/

BOOL    
read_zones(char *p_filename, struct  RastPort        *rpG)

/* lecture du fichier .zones */
/// "read_zones"
{
    FILE *input;
    int mx,my,MX,MY;
/*      struct RastPort *r;*/
    BOOL    Flg_Err;

    char str[100];
    char    *P_Temp;
    char    Buffer_Name[100];
    int     flg_country;
    int     flg_end;
    int     num_country;
    int     num_bord;
    int     tab_bord_tempo[MAX_VECT];
    int     i,*p_alloc;
    /* Vecteur courant */
    VECT    *V;
    int     num_vect,code;
    BOOL    Flg_Draw;
    
    Flg_Err=TRUE;

    num_country=0;
    flg_end=FALSE;
#ifdef DRAW
    r = rpG;
    SetRast(r,COLOR_BLEU+num_first_color);
    SetAPen(r,COLOR_COTE+num_first_color);
    SetBPen(r,COLOR_COTE+num_first_color);
#endif

    sprintf(Buffer_Name,"%s.zones",p_filename);


    if (input = fopen(Buffer_Name,"r"))
    {

        do
        {
            /***************************************************/
            /* D'abord le nom de la zone                       */
            /* On alloue de la memoire pour sauvegarder ce nom */
            /* et on le recopie                                */
            /***************************************************/
            Dtb_Init_Country(&tab_country[num_country]);
            fscanf(input,"%d",&tab_country[num_country].continent);

            /* Lecture du reste de la ligne */
            P_Temp=fgets(str,100,input);
            if(P_Temp!=NULL)
            {
                /* Enleve les blancs */
                P_Temp=stpblk(P_Temp);
#ifdef  TOMDEBUG
                printf("Country name:%s\n",P_Temp);
#endif
                /* Enleve le newline de fin de ligne */
                P_Temp[strlen(P_Temp)-1]=0;
                tab_country[num_country].name=strdup(P_Temp);
            }

            /* Debut du dessin */

            flg_country=TRUE;

            /************************************************/
            /* On recupere les numeros de border de ce pays */
            /* puis pour chaque bord on affiche ses vecteurs*/
            /************************************************/
            mx=100000;
            my=100000;
            MX=-100000;
            MY=-100000;
            num_bord=0;
            code=0;
            do
            {
                fscanf(input,"%d",&num_vect);
#ifdef TOMDEBUG
                printf("Vecteur num:%d\n",num_vect);
#endif
                tab_bord_tempo[num_bord]=num_vect;


                V=tab_vect[num_vect].p_vect;

                Flg_Draw=TRUE;

                if(V[0].Code>1000)
                {
                    code=V[0].Code/1000;
                    if((code==ID_LAC)||(code==ID_RIVIERES))
                    {
                        Flg_Draw=FALSE;
                    }
                }
                if(Flg_Draw)
                {
                    for(i=0;i<tab_vect[num_vect].nb_vect;i++)
                    {
#ifdef DRAW
                        if(i==0)
                        {
                                MyMove(r,V[0].x,V[0].y);
                        }
                        else
                        {
                                MyDraw(r,V[i].x,V[i].y,V[i].Code);
                        }
#endif
                        tstpt(V[i].x,V[i].y,&mx,&my,&MX,&MY);
                    }
                }
                num_bord++;
                if(num_bord>=MAX_VECT)
                {
                    printf("ERROR:Max number of border exceeded\n");
                    flg_end=TRUE;
                }

            }  while(tst_end(input)==FALSE);
            if (MX<=mx)
            {
                mx=V[0].x-5;
                MX=V[0].x+5;
            }
            if (MY<=my)
            {
                my=V[0].y-5;
                MY=V[0].y+5;
            }                               
            tab_country[num_country].c_clip.minx=mx;
            tab_country[num_country].c_clip.miny=my;
            tab_country[num_country].c_clip.maxx=MX;
            tab_country[num_country].c_clip.maxy=MY;
#ifdef DRAW
            MyMove(r,mx,my);
            MyDraw(r,MX,my,5);
            MyDraw(r,MX,MY,5);
            MyDraw(r,mx,MY,5);
            MyDraw(r,mx,my,5);

#endif
            /****************************************/
            /* On sait le nombre de bord de ce pays */
            /* on leur alloue de la memoire et on le*/
            /* recopie                              */
            /****************************************/

#ifdef DEBUG_TOM
            printf("Il y a eu:%d bord\n",num_bord);
#endif
            tab_country[num_country].nb_borders=num_bord;
            p_alloc=(int *)malloc((10+num_bord)*sizeof(int));
            tab_country[num_country].p_borders=p_alloc;
            for(i=0;i<num_bord;i++)
            {
                *p_alloc=tab_bord_tempo[i];
/*                              printf("%d %d\n",i,tab_bord_tempo[i]);*/
                p_alloc++;
            }
            num_country++;
            if (num_country>=MAX_COUNTRY)
            {
                printf("ERROR:Max number of country excedeed\n");
                flg_end=TRUE;
            }
        }  while((tst_end(input)==FALSE)&&(flg_end==FALSE));

        fclose(input);
        Flg_Err=FALSE;
    }
    max_country=num_country;
    return(Flg_Err);
}
///


/*********************************************************** save_zones() ***/

void    
save_zones( char *p_filename)
{
    FILE *output;
    int     num_country;
    int     i;




    if (output = fopen(p_filename,"w"))
    {
            for(num_country=0;num_country<max_country;num_country++)
            {
                    fprintf(output,"%d ",tab_country[num_country].continent);
                    fprintf(output,"%s\n",tab_country[num_country].name);




                    for(i=0;i<tab_country[num_country].nb_borders;i++)
                    {

                    
                            fprintf(output,"%d ",tab_country[num_country].p_borders[i]);

                    }
                    fprintf(output,"E\n");

            }
            fclose(output);
    }
}

void close_files(void)
{
    if (bptr_big!=NULL)
        Close(bptr_big);
}
