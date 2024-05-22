/************************************************************************/
/*                                                                      */
/*      fichier         : divers.c                                      */
/*      projet          : amiglobe                                      */
/*      date création   : juin 1994                                     */
/*      commentaire     : fonctions diverses:                           */
/*                        recherche de capitale                         */
/*                        fermeture de fenêtres                         */
/*                        chargement/sauvegarde des preferences         */
/*                        calcul de la distance                         */
/*      révision        : $VER: divers.c 1.003 (29 Sep 1994) Sep 1994) */
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*      includes                                                        */
/************************************************************************/
#include <clib/exec_protos.h>
#include <proto/muimaster.h>
#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/ports.h>
#include <graphics/gfxbase.h>
#include <clib/intuition_protos.h>
#include <clib/macros.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libraries/mui.h>

#include "amiglobe_interf.h"
#include "amiglobe_types.h"
#include "divers.h"
#include "chemin.h"
/* Prototype get_pays_name */
char * get_pays_name (int);

/************************************************************************/
/*      defines                                                        */
/************************************************************************/

#define MAX_ELEM_REM 1500

/************************************************************************/
/*      prototypes                                                      */
/************************************************************************/

float   Map_Convert_Angle(int angle);

/************************************************************************/
/*      variables externes                                              */
/************************************************************************/

extern struct  ObjApp  *My_App; 
extern COUNTRY * tab_country;
ELEM_REM ** tab_elem;
extern int max_elem;
extern int max_country;
extern TRAJECTOIRE ** tab_trajectoire;
extern CLIP clip_max;
extern PREFERENCE Pref;
extern float distance_orig_dest;
extern struct Window * wG;
extern struct GfxBase * GfxBase;

/************************************************************************/
/*      variables privees                                               */
/************************************************************************/

PREFERENCE Default_Pref=
///
{
    0,0,0,0,0,0,0,
    PROJ_FLAT,
    TRACE_FIL,
    0,0, /*glob_min*/
    0,0,0,
    HIRES_KEY,640,256,4,OSCAN_STANDARD,
    0,0,0,
    0,0,
    {0,0,FALSE},{0,0,FALSE},
    -1,
    640,256,
    5,0
};      
///

/************************************************************************/
/*      implémentation                                              */
/************************************************************************/

void CloseWindowSafely(struct Window * win)
///
{
    struct IntuiMessage * msg;
    struct Node * succ;
    msg=(struct IntuiMessage *)(win->UserPort)->mp_MsgList.lh_Head;
    while (succ = msg->ExecMessage.mn_Node.ln_Succ)
    {
        if (msg->IDCMPWindow == win)
        {
            Remove((struct Node *)msg);
            ReplyMsg((struct Message *)msg);
        }
        msg=(struct IntuiMessage *)succ;
    }
    win->UserPort=NULL;
    ModifyIDCMP(win,0L);
    CloseWindow(win);
}       
///

float distance(int x1,int y1,int x2,int y2)
///
{
    float X1=Map_Convert_Angle(x1);
    float X2=Map_Convert_Angle(x2);
    float Y1=Map_Convert_Angle(y1);
    float Y2=Map_Convert_Angle(y2); 
    float angle=acos(sin(Y1)*sin(Y2)+cos(Y1)*cos(Y2)*cos(X1-X2));
    
    return (float)(angle*RAYON);
}
///

void create_tab_elem(void)
///
{
    int i;
    int j=0;
    ELEM_REM * element;
    tab_elem=(ELEM_REM **)malloc(sizeof(ELEM_REM *)*MAX_ELEM_REM);
    for (i=0;i<max_country;i++)
    {
        element=tab_country[i].P_Elem_Rem;
        while (element!=NULL)
        {
            tab_elem[j]=element;
            element=element->P_Next;
            j++;
        }
    }
    printf("nbr d'élements:%d\n",j);
    max_elem=j;
    tab_elem[j]=NULL;
}
///

ELEM_REM * capitale(int num_pays)
///
{
    ELEM_REM *element=tab_country[num_pays].P_Elem_Rem;
    while (element!=NULL)
    {
        if (element->Type==CAPITALE)
        {
            return element;
        }
        element=element->P_Next;
    }
    return NULL;
}
///

int pays(ELEM_REM * element)
/*rend le numero du pays contenant l'element*/
///
{
        int i;
        ELEM_REM * auxElement;
        for (i=0;i<max_country;i++)
        {
                auxElement=tab_country[i].P_Elem_Rem;
                while (auxElement!=NULL)
                {
                        if (auxElement==element)
                                return i;
                        auxElement=auxElement->P_Next;
                }
        }
        return -1;
}       
///

void delete_element(ELEM_REM *element)
/* efface recursivement tous les elements suivants et lui-même*/
///
{
        if (element!=NULL)
        {
                delete_element(element->P_Next);
                free(element);
        }
}       
///

/* lit des trajectoires dans un fichier */
void read_trajectoires(void)
///
{
    char * P_Name;
    FILE * Fichier_Trajectoire;
    int num_vect=0;
    int num_traj=0;
    int i;
    get(My_App->STR_Trajectoire_Fichier,MUIA_String_Contents,(APTR)&P_Name);
    Fichier_Trajectoire=fopen(P_Name,"r");
    if (Fichier_Trajectoire!=NULL)
    {
        while (tab_trajectoire[num_traj]!=NULL)
                num_traj++;
        while (1)
        {
            TRAJECTOIRE * trajectoire;
            char ligne[256];
            char * end=ligne;
            short int * tab_vect=(short int *)malloc(sizeof(short int)*400);
            short int * tab_vect2;
            sprintf(ligne,"");
            /* recherche du nom de la trajectoire*/
            while ( (end!=NULL) && (strlen(ligne)<4) )
            {
                printf("cherche nom\n");
                end=fgets(ligne,256,Fichier_Trajectoire);
            }
            if (end==NULL)
                break;
            printf("longueur ligne:%d\n",strlen(ligne));
            printf("trouvé:%s\n",ligne);
            trajectoire=(TRAJECTOIRE *)malloc(sizeof(TRAJECTOIRE));
            trajectoire->nom=strdup(ligne);
            do
            {
                printf("cherche début donnees\n");
                end=fgets(ligne,256,Fichier_Trajectoire);
            }
              while ( (end!=NULL) && (strlen(ligne)<4) );
            if (end==NULL)
                break;
            trajectoire->clip.maxx=-32000;
            trajectoire->clip.minx=32000;
            trajectoire->clip.maxy=-32000;
            trajectoire->clip.miny=32000;
            while ( (end!=NULL) && (strlen(ligne)>4) )
            {
                char * lon,*lat,*code;
                lat=strtok(ligne,"$");
                lon=strtok(NULL,"$");
                code=strtok(NULL,"$");
                if ((lat!=NULL)&&(lon!=NULL))
                {
                    if ( (code==NULL) || (atoi(code)<1) || (atoi(code)>5) )
                    {
                        tab_vect[num_vect*3]=(short int)5;
                    }
                    else
                        tab_vect[num_vect*3]=(short int)atoi(code);
                    tab_vect[num_vect*3+1]=(short int)atoi(lon);
                    if (atoi(lon)>trajectoire->clip.maxx)
                            trajectoire->clip.maxx=atoi(lon);
                    if (atoi(lon)<trajectoire->clip.minx)
                            trajectoire->clip.minx=atoi(lon);
                    if (-atoi(lat)>trajectoire->clip.maxy)
                            trajectoire->clip.maxy=-atoi(lat);
                    if (-atoi(lat)<trajectoire->clip.miny)
                            trajectoire->clip.miny=-atoi(lat);
                                    
                    tab_vect[num_vect*3+2]=(short int)(-atoi(lat));
                    num_vect++;
                }
                printf("lecture d'un vecteur lat=%d  lon=%d\n",
                                atoi(lat),atoi(lon));
                end=fgets(ligne,256,Fichier_Trajectoire);
            }
            /* le tableau des vecteurs est prêt*/
            tab_vect2=(short int *)malloc(3*sizeof(short int)*num_vect);
            for (i=0;i<num_vect*3;i++)
                    tab_vect2[i]=tab_vect[i];
            trajectoire->vecteurs=(VECT*)tab_vect2;
            trajectoire->nb_vect=num_vect;
            /* rangement dans le tableau */
            tab_trajectoire[num_traj]=trajectoire;
            num_traj++;
            tab_trajectoire[num_traj]=NULL;
            /* initialisation des variables*/
            num_vect=0;
            free(tab_vect);         
        }
    }
    printf("fichier non trouvé\n");
}
///

void init_preference(void)
///
{
        Pref=Default_Pref;
        Pref.clip_cur=clip_max;
}
///

BOOL load_preference(void)
///
{
        FILE * fichier;
        
        fichier=fopen("amiglobe.prefs","r");
        if (fichier==NULL)
        {
                printf("impossible de trouver le fichier preference\n");
                Pref.clip_cur.minx=-18000;
                Pref.clip_cur.miny=-9000;
                Pref.clip_cur.maxx=18000;
                Pref.clip_cur.maxy=9000;
                return FALSE;
        }
        else
        {
                int sel=0;
                fread(&Pref,sizeof(PREFERENCE),1,fichier);
                fclose(fichier);
                set(My_App->CH_Lacs,MUIA_Selected,Pref.Flg_Draw_Lac);
                set(My_App->CH_Riv,MUIA_Selected,Pref.Flg_Draw_Riv);
                set(My_App->CH_Coord,MUIA_Selected,Pref.Flg_Aff_Coord);
                set(My_App->CH_Noms,MUIA_Selected,Pref.Flg_Aff_Country_Name);
/*              set(My_App->CH_Fill_Pays,MUIA_Selected,Pref.Flg_Fill_Pays);*/
                set(My_App->CH_Elements,MUIA_Selected,Pref.Flg_Aff_Elem_Rem);
                set(My_App->CH_LonLat,MUIA_Selected,Pref.Flg_Aff_LatLon);
                set(My_App->CH_Distance,MUIA_Selected,Pref.Flag_Distance);
                set(My_App->CH_Area,MUIA_Selected,Pref.Flag_Surface);
                set(My_App->CH_Scale,MUIA_Selected,Pref.Flag_Echelle);
                set(My_App->CH_Trajectoires,MUIA_Selected,Pref.Flg_Aff_Trajectoires);
                set(My_App->SL_x,MUIA_Slider_Level,Pref.SL_x);
                set(My_App->SL_y,MUIA_Slider_Level,Pref.SL_y);
                set(My_App->SL_z,MUIA_Slider_Level,Pref.SL_z);
               switch(Pref.Type_Proj)
                {
                        case PROJ_FLAT:
                                if (Pref.Flg_Proj_3D==TRUE)
                                        sel=3;
                                else
                                        sel=0;
                                break;
                        case PROJ_MERCATOR:
                                sel=1;
                                break;
                        case PROJ_GLOBE:
                                sel=2;
                                break;
                        default:
                                sel=0;
                }
                set (My_App->CY_Proj,MUIA_Cycle_Active,sel);

                Update_Points();
                return TRUE;
        }
}
///

BOOL save_preference(void)
///
{
/*        FILE * fichier;
        
        fichier=fopen("amiglobe.prefs","w");
        if (fichier==NULL)
        {
                printf("impossible d'ouvrir le fichier preference");
                return FALSE;
        }
        else
        {
                fwrite(&Pref,sizeof(PREFERENCE),1,fichier);
                fclose(fichier);
                return TRUE;
        }*/
        return TRUE;
}               
///

void Application_Sleep(BOOL booleen)
{
    static int increment=0;
    set(My_App->App,MUIA_Application_Sleep,booleen);
    if (booleen==TRUE)
    {
        increment ++;
        /* busy pointer*/
        if (increment==1 && GfxBase->LibNode.lib_Version>=39)
            SetWindowPointer(wG,
                WA_BusyPointer,TRUE,WA_PointerDelay,TRUE,TAG_DONE);
    }
    else
    {
        if (increment==1 && GfxBase->LibNode.lib_Version>=39)
            SetWindowPointer(wG,TAG_DONE);
        increment--;
    }
}
