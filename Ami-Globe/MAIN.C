/************************************************************************/
/*                                                                      */
/*      fichier         : main.c                                        */
/*      projet          : amiglobe                                      */
/*      date création   : 1993                                          */
/*      commentaire     : création des menus, lancement de l'application*/
/*      révision        : $VER: main.c 1.216 (16 Oct 1994)
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*     $HISTORY:                                                */
/*                                                              */
/*     16 Oct 1994 : 001.216 :  ajout des choix de continents dans liste classement
/*     16 Oct 1994 : 001.215 :  ajout de destruct hook à LV_classement
/*     15 Oct 1994 : 001.214 :  ajoute LV_classement_Compare_hook
/*     12 Oct 1994 : 001.213 :  LV_classement_func
/*                     1993 : 0.01 : version initiale           */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*      includes                                                        */
/************************************************************************/

/* MUI */
#include <libraries/mui.h>

#include <exec/types.h>
#include <exec/io.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <libraries/dos.h>
#include <intuition/intuition.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>

/* Prototypes */
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/muimaster_protos.h>
#include <clib/asl_protos.h>
#include <clib/diskfont_protos.h>
#include <clib/macros.h>
#include <clib/datatypes_protos.h>
#include <clib/locale_protos.h>
/* ANSI C */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define REG(x) register __ ## x
#define ASM    __asm
#define SAVEDS __saveds

#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/muimaster_pragmas.h>
#include <pragmas/asl_pragmas.h>


#ifdef  LATTICE
#include        <proto/all.h>
#endif

#include "amiglobe_types.h"
#include "database_types.h"
#include "main_protos.h"
#include "map_function_protos.h"
#include "read_bin.h"
#include "read_org.h"
#include "editeur_protos.h"
#include "database_protos.h"
#include "3d_types.h"
#include "3d_protos.h"
#include "divers.h"
#include "map_divers.h"
#include "AmiGlobe_Interf.h"
#include "Flag_Protos.h"  
#include "Iff_Fct_Protos.h"
#include "chemin.h"

/* get the PowerWindows 2.0 code */
/*#include "windowdef.h"*/

/************************************************************************/
/*      variables externes                                              */
/************************************************************************/

extern struct Library *UtilityBase=NULL;
extern struct Library *IconBase=NULL;

/* External var used */
extern  COUNTRY *tab_country;
extern  ELEM_REM **tab_elem;
/* pour l'edition seulement; a effacer version finale:*/
short int * tab_new_border=NULL;
int longueur_new_border=0;
/*                                      */
extern int num_first_color;
extern E_Type_Data Type_Donnee;
/************************************************************************/
/*      variables créées                                                */
/************************************************************************/

/*preferences*/
PREFERENCE Pref;

BOOL Flg_Fond;          

struct Library *MUIMasterBase=NULL;
struct Library *FastsincosBase=NULL;
struct Library *DiskfontBase=NULL;
struct Library *GadToolsBase=NULL;
struct Library *DataTypeBase=NULL;
struct Library *LocaleBase=NULL;
struct Library *AslBase=NULL;
struct IntuitionBase *IntuitionBase=NULL;
struct GfxBase  *GfxBase=NULL;
#define DRAW

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

typedef enum    {CMD_SELECT,CMD_ZOOM,CMD_POINT_ORIG,CMD_POINT_DEST,
                                 CMD_DONN_EXT,CMD_NEW_POINT} /*CMD_NEW_POINT A EFFACER*/
                        E_ACTION_CMD;
E_ACTION_CMD    action=CMD_SELECT;

/*** ids ***/

enum ids {
    ID_DUMMY,
    ID_ABOUT,
    ID_DEFAULT_PREF,
    ID_LOAD_TRAJ,
    ID_LOAD_TRAJ_OK,
    ID_SELECT_FILE_TRAJ,
    ID_REDRAW,
    ID_INFO,
    ID_FILL,
    ID_MOVE_PT,
    ID_SHOW_PT,
    ID_SHOW_SEG,
    ID_EDIT,
    ID_MODE_EDIT,
    ID_MODE_NOT_EDIT,
    ID_CREATE_PAYS,
    ID_SEL_CTY,
    ID_NEW_CTY,
    ID_CLASSEMENT_CTY,
    ID_INFO_ELEM,
    ID_SEL_CONT,
    ID_SEL_ELEM,
    ID_ELEM_SET_POINT_ORIG,
    ID_ELEM_SET_POINT_DEST,
    ID_NOT_YET,
    ID_AFF_DATA,
    ID_AFF_DATA_EXT,
    ID_DETAIL_ALL,
    ID_DETAIL_1,
    ID_DETAIL_2,
    ID_DETAIL_3,
    ID_DETAIL_4,
    ID_DETAIL_5,
    ID_LOCALISATION,
    ID_POINT_ORIG_PUT,/* ajouter point origine*/
    ID_POINT_ORIG_REM,/* enlever point origine*/
    ID_POINT_DEST_PUT,
    ID_POINT_DEST_REM,
    ID_DONN_EXT,
    ID_ORGANISATION,
    ID_DISTANCE,
    ID_SURFACE,
    ID_ECHELLE,
    ID_PREFS_AFF,
    ID_DONNEES,
    ID_PROJ,
    ID_GROUPE_CHG,
    ID_PR_X,
    ID_PR_Y,
    ID_PR_Z,
/*    ID_SELECT_FILE,*/
    ID_SELECT_FILE_DONNEE,
    ID_REDRAW_EXT,
    ID_SCREEN,
    ID_NO_FOND,
    ID_AFF_DRAP,
    ID_CLOSE_DRAP,
    ID_SELECT,
    ID_DESELECT,
    ID_OK_DATAS,
    ID_ADD_DATA
    };

struct  ObjApp  *My_App=NULL;  

#define Offset(x)       ( &(((struct ObjApp *)(0))->x) )

int     Tab_Win[16]={
        Offset(WI_Cmd),
        Offset(WI_Param),
        Offset(WI_Sel_Cty),
        Offset(WI_Cmd_Panel),
        Offset(WI_Sel_Cont),
        Offset(WI_Chargement),
        Offset(WI_Datas),
        Offset(WI_Param_Datas),
        Offset(WI_Sel_Elem),
        Offset(WI_Trajectoire),
        Offset(WI_Organisation),
        Offset(WI_Info_Element),
        Offset(WI_Classement),
        Offset(WI_Info),
        -1
};

struct NewMenu Menu[] =
{
{ NM_TITLE, "Project"  , 0 ,0,0,(APTR)0            },
        { NM_ITEM , "About..." ,"?",0,0,(APTR)ID_ABOUT     },
        { NM_ITEM , NM_BARLABEL, 0 ,0,0,(APTR)0            },
        { NM_ITEM , "Load a path" ,0,0,0,(APTR)ID_LOAD_TRAJ     },
        { NM_ITEM , "Default Pref",0,0,0,(APTR)ID_DEFAULT_PREF },
        { NM_ITEM , "Quit"     ,"Q",0,0,(APTR)MUIV_Application_ReturnID_Quit },
{ NM_TITLE, "Selection" , 0 ,0,0,(APTR)0            },
        { NM_ITEM , "Country" ,"p",0,0,(APTR)ID_SEL_CTY   },
        { NM_ITEM , "Continent" ,0,0,0,(APTR)ID_SEL_CONT   },
        { NM_ITEM , "Element" ,0,0,0,(APTR)ID_SEL_ELEM   },
{ NM_TITLE, "Preferences" , 0 ,0,0,(APTR)0            },
        { NM_ITEM , "Display parameters" ,"c",0,0,(APTR)ID_PREFS_AFF   },
        { NM_ITEM , "Screen" ,"e",0,0,(APTR)ID_SCREEN   },
        { NM_ITEM , "Detail level" ,0,0,0,(APTR)NULL },
                { NM_SUB , "Auto" ,"A",0,0,(APTR)ID_DETAIL_ALL },
                { NM_SUB , "1" ,"1",0,0,(APTR)ID_DETAIL_1 },
                { NM_SUB , "2" ,"2",0,0,(APTR)ID_DETAIL_2 },
                { NM_SUB , "3" ,"3",0,0,(APTR)ID_DETAIL_3 },
                { NM_SUB , "4" ,"4",0,0,(APTR)ID_DETAIL_4 },
                { NM_SUB , "5" ,"5",0,0,(APTR)ID_DETAIL_5 },
{ NM_TITLE, "Functions" , 0 ,0,0,(APTR)0            },
        { NM_ITEM , "Localization Window" ,"l",0,0,(APTR)ID_LOCALISATION   },
        { NM_ITEM , "Info about element" ,0,0,0,(APTR)ID_DONN_EXT   },
        { NM_ITEM , "Source point" ,0,0,0,(APTR)NULL },
                { NM_SUB , "Set" ,"O",0,0,(APTR)ID_POINT_ORIG_PUT },
                { NM_SUB , "Remove" ,0,0,0,(APTR)ID_POINT_ORIG_REM },
        { NM_ITEM , "Target point" ,0,0,0,(APTR)NULL   },
                { NM_SUB , "Set" ,"D",0,0,(APTR)ID_POINT_DEST_PUT },
                { NM_SUB , "Remove" ,0,0,0,(APTR)ID_POINT_DEST_REM },
        { NM_ITEM , "Data comparison" ,"d",0,0,(APTR)ID_DONNEES  },
        { NM_ITEM , "Redisplay comparison" ,"r",0,0,(APTR)ID_AFF_DATA },
        { NM_ITEM , "Display with background" ,"d",0,0,(APTR)ID_AFF_DATA_EXT  },
{ NM_TITLE, "Utilities" , 0 ,0,0,(APTR)0            },
        { NM_ITEM , "Select by flag" ,0,0,0,(APTR)ID_AFF_DRAP   },
/*        { NM_ITEM , "Cities list" ,0,0,0,(APTR)ID_NOT_YET   },     */
/*        { NM_ITEM , "Mountains list" ,0,0,0,(APTR)ID_NOT_YET   },*/
        { NM_ITEM , "Organisations list" ,0,0,0,(APTR)ID_ORGANISATION   },
/*        { NM_ITEM , "External data list" ,0,0,0,(APTR)ID_NOT_YET   },  */
{ NM_END  , NULL       , 0 ,0,0,(APTR)0            },
};
/*fonts*/
struct  TextAttr Titre_Font_Attr=
    {"helvetica.font",11,FSF_BOLD,FPF_DISKFONT};
struct  TextFont *Titre_Font;
struct  TextAttr Text_Font_Attr
        ={"topaz.font",8,0,FPF_DISKFONT};
struct  TextFont *Text_Font;
char Titre_Ecran[100];


UWORD   Tab_Pens[]={(UWORD)(~0) };

#define HProp           PropObject, PropFrame, MUIA_Prop_Horiz, TRUE, MUIA_FixHeight, 8, MUIA_Prop_Entries, 111, MUIA_Prop_Visible, 10, End
#define VProp           PropObject, PropFrame, MUIA_Prop_Horiz, FALSE, MUIA_FixWidth , 8, MUIA_Prop_Entries, 111, MUIA_Prop_Visible, 10, End
#define List(ftxt)      ListviewObject, MUIA_Weight, 50, MUIA_Listview_Input, FALSE, MUIA_Listview_List, FloattextObject, MUIA_Frame, MUIV_Frame_ReadList, MUIA_Floattext_Text, ftxt, MUIA_Floattext_TabSize, 4, MUIA_Floattext_Justify, TRUE, End, End


USHORT quit_flag = FALSE;

BOOL    Prise_En_Compte=TRUE;

struct Window * OpenWindow ();
struct Screen * OpenScreen ();
void   Aff_Image(struct RastPort *rpG,char *P_Name); 
struct Screen   *sC=NULL;
struct Screen   *sC_Image=NULL; 
struct ViewPort *vP;

struct Window *wG;              /* we fetch the RastPort pointer from here */
struct Window *win_info=NULL;
struct Window *win_localisation=NULL;
struct Menu   *MyMenu=NULL;

struct RastPort *rpG,*cmd_rpG;

struct  TmpRas  tmpras;         /* Pour les areadraw */
struct  AreaInfo myareainfo;
WORD    *areabuffer;
char    *buffer_rast=NULL;

int     signal_wG;
int     signal_win_info=0;
int     signal_win_image=0;
int     signal_win_localisation=0;
BOOL     continent_sel[6]= {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE}; 
          /* Continents courants sélectionnés*/
float   distance_orig_dest;
int     flg_edit=FALSE;
int     flg_press=FALSE;
APTR    WI_Drapeaux=NULL;
/* tableau des bitmaps alloues pour les drapeaux des pays */
struct BitMap * Tab_Drapeaux[MAX_COUNTRY];
struct Locale * MyLocale=NULL;

extern CLIP clip_max;

BOOL    Init_Screen(BOOL );
void    Main_Init_Font(void);
void    Do_Action(int);
/*
__oslibversion=42;

ULONG __asm _LibInit   ( register __a0 APTR seglist,
                         register __d0 struct MyLibrary *libbase )
{}

void __regargs __autoopenfail(char *lib)
{
printf("zut\n");
} */

void Aff_Error(char *Texte,char *Bouton)
{
        static struct EasyStruct ES={
                sizeof(struct EasyStruct),
                0,
                "Ami-Globe : Warning",
                "",
                ""
        };
        ES.es_TextFormat   = Texte;
        ES.es_GadgetFormat = Bouton;

        if(wG!=NULL)EasyRequest(wG, &ES, NULL);
}

/* Bascule les elements selectionnes d'une liste vers une autre */

void bascule(APTR Obj_From,APTR Obj_To)
{
                        
        LONG id = -1;
        void    *entree;
        int     *tab_pos,i,Max;

        tab_pos=malloc(sizeof(int)*get_max_country());
        if(tab_pos==NULL)return;

        for (i=0;;i++)
        {
                DoMethod(Obj_From,MUIM_List_NextSelected,&id);
                if (id==-1) break;
                tab_pos[i]=id;
        }
        Max=i;
        for(i=Max-1;i>=0;i--){          
                DoMethod(Obj_From,MUIM_List_GetEntry,tab_pos[i],&entree);
                DoMethod(Obj_To,MUIM_List_Insert,&entree,1,MUIV_List_Insert_Sorted);
                DoMethod(Obj_From,MUIM_List_Remove,tab_pos[i]);
        }
        free(tab_pos);
}

/********** Del Ext Image *****************************/
void Del_Ext_Image(void)
{
        if(sC_Image){
                if(sC_Image->FirstWindow)CloseWindow(sC_Image->FirstWindow);
                CloseScreen(sC_Image);
        }
        sC_Image=NULL;
        signal_win_image=NULL;
}


/****************************************************************************/

/* This is for the event handler */
void quit(void)
{
        quit_flag = TRUE;
}

void SAVEDS ASM activate_window_func(void)
/*active la fenetre wG chaque fois que l'on active une autre fenêtre*/
{
        ActivateWindow(wG);
}

struct Hook activate_window_hook=
{
        {NULL,NULL},
        (void *)activate_window_func,
        NULL,NULL
};

void SAVEDS ASM new_border_func(void)
{
        /*printf("cmd new border\n");*/
        longueur_new_border=0;
        tab_new_border=(short int*)malloc(1200);
}

struct Hook new_border_hook =
{
        {NULL,NULL},
        (void *)new_border_func,
        NULL,NULL
};

void SAVEDS ASM new_point_func(void)
{
        /*printf("cmd new point\n");*/
        action=CMD_NEW_POINT;
}

struct Hook new_point_hook=
{
        {NULL,NULL},
        (void *)new_point_func,
        NULL,NULL
};

void SAVEDS ASM end_border_func(void)
{
        FILE * fichier_border=NULL;
        char * nom_fichier_border;
        char buffer[30];
        get(My_App->STR_Nom_Border,MUIA_String_Contents,
                                (APTR)&nom_fichier_border);
        /*printf("cmd end border nom fichier:%s nb de points:%d\n",
                        nom_fichier_border,longueur_new_border);*/
        sprintf(buffer,"ram:%s",nom_fichier_border);
        fichier_border=fopen(buffer,"w");
        fwrite(&longueur_new_border,sizeof(short int),1,fichier_border);
        fwrite(tab_new_border,sizeof(short int),longueur_new_border*3,
                fichier_border);
        fclose(fichier_border);
        free(tab_new_border);
        tab_new_border=NULL;
        action=CMD_SELECT;
}

struct Hook end_border_hook=
{
        {NULL,NULL},
        (void *)end_border_func,
        NULL,NULL
};


void SAVEDS ASM zoom_func(void)
{
        /*printf("cmd zoom!\n");*/
        flg_press=FALSE;
        action=CMD_ZOOM; 
}

struct Hook zoom_hook=
{
        {NULL,NULL},
        (void *)zoom_func,
        NULL,NULL
};

void SAVEDS ASM zoom_out_func(void)
{
/*      int sx=Pref.clip_cur.maxx-Pref.clip_cur.minx;
        int sy=Pref.clip_cur.maxy-Pref.clip_cur.miny;
        int minx=Pref.clip_cur.minx-sx/2;
        int maxx=Pref.clip_cur.maxx+sx/2;
        int miny=Pref.clip_cur.miny-sy/2;
        int maxy=Pref.clip_cur.maxy+sy/2;
        printf("cmd zoom out !\n");
        conv_xy(&minx,&miny);
        conv_xy(&maxx,&maxy);
        map_set_zoom(   rpG,minx,miny,maxx,maxy);*/
        if (sC)
        {
                int x1=-wG->Width/2-1;
                int y1=-wG->Height/2-1;
                int y2=wG->Height*3/2+1;
                int x2=(wG->Width*(y2-y1))/wG->Height+x1;
                map_set_zoom(   rpG,
                                x1,y1,
                                x2,y2
                                );
        }
}

struct Hook zoom_out_hook=
{
    {NULL,NULL},
    (void *)zoom_out_func,
    NULL,NULL
};

void SAVEDS ASM zoom_reset_func(void)
{
    map_reset_zoom(rpG);
    Pref.Flg_Aff_Coord=TRUE;
}

struct Hook zoom_reset_hook=
{
    {NULL,NULL},
    (void *)zoom_reset_func,
    NULL,NULL
};

void SAVEDS ASM aff_donnee_externe_func(void)
{
    char * nom;
    get(My_App->TX_nom_element,MUIA_Text_Contents,&nom);

    if(sC_Image)Del_Ext_Image();

    sC_Image=Iff_Load_And_Open(nom);
    if(sC_Image)
    {
        signal_win_image=(1<<sC_Image->FirstWindow->UserPort->mp_SigBit);
    }
}

struct Hook aff_donnee_externe_hook=
{
    {NULL,NULL},
    (void *)aff_donnee_externe_func,
    NULL,NULL
};

void SAVEDS ASM continent_select_func(void)
{
    int position=MUIV_List_NextSelected_Start;
    int i;
    for (i=0;i<6;i++)
        continent_sel[i]=FALSE;
    for (;;)
    {
        DoMethod(My_App->LV_Continent,MUIM_List_NextSelected,&position);
        if (position==MUIV_List_NextSelected_End) 
            break;
        continent_sel[position]=TRUE;
    }
}

struct Hook continent_select_hook=
{
    {NULL,NULL},
    (void *)continent_select_func,
    NULL,NULL
};

void SAVEDS ASM Tout_Selectionner_func(void)
{
    int i,max;
    COUNTRY * pays;
    set (My_App->LV_Select,MUIA_List_Quiet,TRUE);
    set (My_App->LV_Deselect,MUIA_List_Quiet,TRUE);
    get (My_App->LV_Deselect,MUIA_List_Entries,&max);
    for (i=max-1;i>=0;i--)
    {
        DoMethod (My_App->LV_Deselect,MUIM_List_GetEntry,i,&pays);
        if (pays!=NULL)
        {
            DoMethod(My_App->LV_Select,MUIM_List_InsertSingle,pays,
                MUIV_List_Insert_Sorted);
            DoMethod(My_App->LV_Deselect,MUIM_List_Remove,i);
        }
    }
    set (My_App->LV_Select,MUIA_List_Quiet,FALSE);
    set (My_App->LV_Deselect,MUIA_List_Quiet,FALSE);
}

struct Hook Tout_Selectionner_hook=
{
    {NULL,NULL},
    (void *)Tout_Selectionner_func,
    NULL,NULL
};

void SAVEDS ASM Tout_Deselectionner_func(void)
{
    int i,max;
    COUNTRY * pays;
    set (My_App->LV_Select,MUIA_List_Quiet,TRUE);
    set (My_App->LV_Deselect,MUIA_List_Quiet,TRUE);
    get (My_App->LV_Select,MUIA_List_Entries,&max);
    for (i=max-1;i>=0;i--)
    {
        DoMethod (My_App->LV_Select,MUIM_List_GetEntry,i,&pays);
        if (pays!=NULL)
        {
            DoMethod(My_App->LV_Deselect,MUIM_List_InsertSingle,pays,
                MUIV_List_Insert_Sorted);
            DoMethod(My_App->LV_Select,MUIM_List_Remove,i);
        }
    }
    set (My_App->LV_Select,MUIA_List_Quiet,FALSE);
    set (My_App->LV_Deselect,MUIA_List_Quiet,FALSE);
}

struct Hook Tout_Deselectionner_hook=
{
    {NULL,NULL},
    (void *)Tout_Deselectionner_func,
    NULL,NULL
};

void  SAVEDS ASM Classement_Continent_func(void)
{
    int i,max;
    COUNTRY * pays;
    Tout_Selectionner_func();
    set (My_App->LV_Select,MUIA_List_Quiet,TRUE);
    set (My_App->LV_Deselect,MUIA_List_Quiet,TRUE);
    get (My_App->LV_Select,MUIA_List_Entries,&max);
    for (i=max-1;i>=0;i--)
    {
        DoMethod (My_App->LV_Select,MUIM_List_GetEntry,i,&pays);
        if (pays!=NULL)
            if (continent_sel[pays->continent]==FALSE)
            {
                DoMethod(My_App->LV_Deselect,MUIM_List_InsertSingle,pays,
                    MUIV_List_Insert_Sorted);
                DoMethod(My_App->LV_Select,MUIM_List_Remove,i);
            }
    }
    set (My_App->LV_Select,MUIA_List_Quiet,FALSE);
    set (My_App->LV_Deselect,MUIA_List_Quiet,FALSE);
}

struct Hook Classement_Continent_hook=
{
    {NULL,NULL},
    (void *)Classement_Continent_func,
    NULL,NULL
};

void SAVEDS ASM Petits_Pays_func(void)
{
    /*elimine tous les pays de pop<1000000 de la liste des  */
    /* pays selectionnés                                    */
    int i,max;
    COUNTRY * pays;
    set (My_App->LV_Select,MUIA_List_Quiet,TRUE);
    set (My_App->LV_Deselect,MUIA_List_Quiet,TRUE);
    get (My_App->LV_Select,MUIA_List_Entries,&max);
    for (i=max-1;i>=0;i--)
    {
        DoMethod (My_App->LV_Select,MUIM_List_GetEntry,i,&pays);
        if (pays!=NULL)
            if (pays->population<1000000)
            {
                DoMethod(My_App->LV_Deselect,MUIM_List_InsertSingle,pays,
                    MUIV_List_Insert_Sorted);
                DoMethod(My_App->LV_Select,MUIM_List_Remove,i);
            }
    }
    set (My_App->LV_Select,MUIA_List_Quiet,FALSE);
    set (My_App->LV_Deselect,MUIA_List_Quiet,FALSE);
}

struct Hook Petits_Pays_hook=
{
    {NULL,NULL},
    (void *)Petits_Pays_func,
    NULL,NULL
};

/* prototype de cmd_new_cty*/
void cmd_new_cty(int);

void SAVEDS ASM drapeau_select_func(REG(a1) int *numero,REG(a2) void * on_sen_fout)

{
    cmd_new_cty(*numero);
    /*
    int old_country;
    old_country=Pref.country_sel;
    if (old_country!=-1)
    {
        fill_country(Pref.country_sel,0,rpG);
    }
    Pref.country_sel=*numero;
    if (Pref.country_sel==old_country)
    {
        set(My_App->TX_Selectionne,MUIA_Text_Contents,0);
        Pref.country_sel=-1;
    }
    else
    {
        if (Pref.country_sel!=-1)
        {
            set(My_App->TX_Selectionne,MUIA_Text_Contents,
                get_pays_name(Pref.country_sel));
            fill_country(Pref.country_sel,1,rpG);
            if (win_info!=NULL)
                cmd_info();
        }
    } */
}

struct Hook drapeau_select_hook=
{
    {NULL,NULL},
    (void *)drapeau_select_func,
    NULL,NULL
};

void cmd_redraw(void)
{
    /*printf("Cmd redraw!\n");*/
    if (Pref.Flg_Fill_Country)
    {
        Map_Give_Type_Trace(TRACE_PLEIN);
    }
    else
    {
        Map_Give_Type_Trace(TRACE_FIL);
    }
    redraw_map(-1,rpG);
    Pref.Flg_Aff_Coord=TRUE;
}


void  cmd_add_data(void)
{
    char    *P_Name;

    get(My_App->STR_fichier_Donnee,MUIA_String_Contents,(APTR)&P_Name);
    if(strlen(P_Name)!=0){
            if(Dtb_Read_Data_File(P_Name)){
                    Aff_Error("Erreur lecture du fichier de donnee...\n","Ok");
            }else{
                    Dtb_Create_Liste_Data(My_App->LV_Datas);
                    Dtb_Create_Liste_Data(My_App->LV_Data1);
                    Dtb_Create_Liste_Data(My_App->LV_Data2);
            }
    }
}


void cmd_localisation(void)
{
    ULONG aux_Flg_Draw_Lac=Pref.Flg_Draw_Lac;
    ULONG aux_Flg_Draw_Riv=Pref.Flg_Draw_Riv;
    ULONG aux_Flg_Aff_Country_Name=Pref.Flg_Aff_Country_Name;
    ULONG aux_Flg_Aff_Elem_Rem=Pref.Flg_Aff_Elem_Rem;  
    ULONG aux_Flg_Aff_Trajectoires=Pref.Flg_Aff_Trajectoires;  
    CLIP aux_clip_cur=Pref.clip_cur;
    E_TYPE_PROJ     aux_Type_Proj=Pref.Type_Proj;
    static char titre_localisation[13]="Localisation";      
    struct TmpRas * P_tmpras=
            (struct TmpRas *)malloc(sizeof(struct TmpRas));
    void * rasbuffer;
    if (win_localisation==NULL) /* une seule fenetre localisation à la fois */
    {
        win_localisation=OpenWindowTags(
                NULL,
                WA_Left,        (sC->Width)*2/3,
                WA_Top,         0,
                WA_Width,       (sC->Width)/3,
                WA_Height,      (sC->Height)/3,
                WA_CustomScreen,sC,
                WA_MinWidth,    (ULONG)50,
                WA_MinHeight,   (ULONG)50,
                WA_MaxWidth,    (ULONG)320,
                WA_MaxHeight,   (ULONG)256,                     
                WA_Flags,       WFLG_SMART_REFRESH|WFLG_CLOSEGADGET|
                        WFLG_GIMMEZEROZERO|WFLG_DRAGBAR|
                        WFLG_SIZEGADGET|WFLG_DEPTHGADGET,
                WA_IDCMP,       IDCMP_CLOSEWINDOW|IDCMP_NEWSIZE|IDCMP_MOUSEBUTTONS,
                TAG_DONE);
                signal_win_localisation=(1<<win_localisation->UserPort->mp_SigBit);     
    }
    /* allocation TmpRas*/
    if (win_localisation)
    {
        rasbuffer=(char *)AllocRaster(320,256);
        if (rasbuffer==NULL)
                printf("Error: not enough memory\n");
        InitTmpRas(P_tmpras,rasbuffer,RASSIZE(320,256));                
        win_localisation->RPort->AreaInfo=&myareainfo;
        win_localisation->RPort->TmpRas=P_tmpras;
        
        give_size_window(win_localisation->Width-win_localisation->BorderRight,
                                win_localisation->Height);
        Pref.Flg_Draw_Lac=0;
        Pref.Flg_Draw_Riv=0;
        Pref.Flg_Aff_Country_Name=0;
        Pref.Flg_Aff_Elem_Rem=0;
        Pref.Flg_Aff_Trajectoires=0;
        Pref.clip_cur=clip_max;
        Map_Give_Type_Proj(PROJ_FLAT);
        map_set_prof(5);
        
        redraw_map(-1,win_localisation->RPort);
        Pref.Flg_Draw_Lac=aux_Flg_Draw_Lac;
        Pref.Flg_Draw_Riv=aux_Flg_Draw_Riv;
        Pref.Flg_Aff_Country_Name=aux_Flg_Aff_Country_Name;
        Pref.Flg_Aff_Elem_Rem=aux_Flg_Aff_Elem_Rem;
        Pref.Flg_Aff_Trajectoires=aux_Flg_Aff_Trajectoires;
        Pref.clip_cur=aux_clip_cur;
        Map_Give_Type_Proj(aux_Type_Proj);
        conv_clip();    
        give_size_window(wG->Width,wG->Height);
        map_set_prof(0);
        SetWindowTitles(win_localisation,titre_localisation,(UBYTE *)~0);

        /* libération TmpRas */
        FreeRaster(rasbuffer,320,256);
        free(P_tmpras);
    }
}

/************ Ouvre un requester de fichier, et met dans l'objet */
/* String Mui passé en parametre le nom du fichier selectionné   */
/* return FALSE si rien n'a ete selectionne                      */

int cmd_select_file(APTR Mui_String)
{
        struct  FileRequester   *my_req=0;
        int     code_ret;
        int     res;
        int     len_dir;
        char    Buffer[200];
        char    *P_Name;
        char    Dir[200];
        char    Name[200];
        int     n;
        code_ret=NULL;

        if(AslBase!=0){
           get(Mui_String,MUIA_String_Contents,(APTR)&P_Name);
           n=strlen(P_Name);
           while((P_Name[n]!='/')&&(P_Name[n]!=':')&&(n>0))n--;
           if(n!=0){
                strncpy(Dir,P_Name,n+1);
                Dir[n+1]=0;
                strcpy(Name,&P_Name[n+1]);
           }else{
                strcpy(Name,P_Name);
                Dir[0]=0;
           }
           my_req=MUI_AllocAslRequestTags(ASL_FileRequest,
                                        ASLFR_Window,wG,
                                        ASL_Hail,"Select a file",
                                        ASLFR_InitialDrawer ,Dir,
                                        ASLFR_InitialFile,Name,
                                        TAG_END);
            /* A t'on pu obtenir le requester? si non, on sort...               */
           if(my_req!=0){

                /* Ouverture du requester de fichier pour l'utilisateur */
                res=(int)MUI_AslRequest(my_req,0L);

                if(res != 0){
                        /*printf("Selection:%s + %s\n",my_req->rf_Dir,
                                                my_req->rf_File);
                        */
                        len_dir=strlen(my_req->rf_Dir);
                        if(     (my_req->rf_Dir[len_dir-1]!='/')&&
                                (my_req->rf_Dir[len_dir-1]!=':')){
                                sprintf(Buffer,"%s/%s",my_req->rf_Dir,my_req->rf_File);
                        }else{
                                sprintf(Buffer,"%s%s",my_req->rf_Dir,my_req->rf_File);
                        }
                        set(Mui_String,MUIA_String_Contents,(APTR)Buffer);
        
                        code_ret=TRUE;

                }
                MUI_FreeAslRequest(my_req);
          }
        }
        return(code_ret);
}

void cmd_change_screen(void)
{
    struct  ScreenModeRequester     *my_req=0;
    int     code_ret;
    int     res;

    code_ret=NULL;

    if(AslBase!=0){
       my_req=MUI_AllocAslRequestTags(ASL_ScreenModeRequest,
                                    ASL_Hail,"Select a screen mode",
                                    ASLFR_Window,wG,
                                    /* On met les parametres deja present */
                                    ASLSM_InitialDisplayID    ,(ULONG)Pref.DisplayId,
                                    ASLSM_InitialDisplayWidth ,(ULONG)Pref.Width,
                                    ASLSM_InitialDisplayHeight,(ULONG)Pref.Height,
                                    ASLSM_InitialDisplayDepth ,(ULONG)Pref.Depth,
                                    ASLSM_InitialOverscanType ,(ULONG)Pref.OScan,
                                    TAG_END);
        /* A t'on pu obtenir le requester? si non, on sort...               */
       if(my_req!=0){

            /* Ouverture du requester de fichier pour l'utilisateur */
            res=(int)MUI_AslRequestTags(my_req,
                                    ASLSM_DoDepth,TRUE,
                                    ASLSM_DoOverscanType,TRUE,
                                    TAG_END);

            if(res != 0){
                    if(my_req->sm_DisplayDepth>=4){

                            Pref.DisplayId=my_req->sm_DisplayID;
                            Pref.Width    =my_req->sm_DisplayWidth;
                            Pref.Height   =my_req->sm_DisplayHeight;
                            Pref.Depth    =my_req->sm_DisplayDepth;
                            Pref.OScan    =my_req->sm_OverscanType;

                            Init_Screen(FALSE);                     
                            cmd_redraw();
                            SetWindowTitles(wG,(UBYTE *)~0,Titre_Ecran );
                            code_ret=TRUE;
                    }else{
                            Aff_Error("ERREUR: il faut quatre plans au minimum!\n","OK");
                    }
            }
            MUI_FreeAslRequest(my_req);
      }
    }
    Main_Init_Font();
/*      return(code_ret);*/
}

void cmd_redraw_ext(void)
{
    char    *P_Name;
    Object * chaine;
    get(My_App->PA_chargement_fond,MUIA_Popstring_String,&chaine);
    get(chaine,MUIA_String_Contents,(APTR)&P_Name);
    if (strlen(P_Name)!=0)
    {
        Map_Aff_Fond(P_Name,sC);
        cmd_redraw();
        Flg_Fond=TRUE;
    }
}


void cmd_point_orig(void)
{
        flg_press=FALSE;
        action=CMD_POINT_ORIG;
}

void cmd_point_dest(void)
{                      
        flg_press=FALSE;
        action=CMD_POINT_DEST;
}


void cmd_info(void)
{
    E_TYPE_PROJ  aux_Type_Proj=Pref.Type_Proj;
/*    static struct AreaInfo win_info_Area;
    static void * areabuffer=NULL;
    static BOOL firsttime=TRUE;
    if (firsttime==TRUE)
        {
            areabuffer=AllocMem(MAX_AREA_PT*5,0L);
            InitArea(&win_info_Area,areabuffer,MAX_AREA_PT);
            firsttime=FALSE;
        }*/
/*    struct TmpRas * P_tmpras=
            (struct TmpRas *)malloc(sizeof(struct TmpRas));
    void * rasbuffer;
    int i;
*/    if (Pref.country_sel!=-1)
    {  
        /* préchargement des borders    */
        /*for (i=0;i<tab_country[Pref.country_sel].nb_borders;i++)
        {
            VECT *V;
            int nb_vect;
            check_border(tab_country[Pref.country_sel].p_borders[i],
                &nb_vect,&V);
        }*/
        if (win_info==NULL)            /* une seule fenetre info à la fois */
        {
            win_info=OpenWindowTags(
                    NULL,
                    WA_Left,        5,
                    WA_Top,         5,
                    WA_Width,       MIN(sC->Width-10,256*sC->Width/sC->Height),
                    WA_Height,      MIN(sC->Height-10,256),
                    WA_CustomScreen,sC,
                    WA_MinWidth,    MIN(sC->Width-10,128*sC->Width/sC->Height),
                    WA_MinHeight,   128L,
                    WA_MaxWidth,    -1,
                    WA_MaxHeight,   -1,
                    WA_Flags,       WFLG_SMART_REFRESH|WFLG_CLOSEGADGET|
                        WFLG_GIMMEZEROZERO|WFLG_DRAGBAR|
                        WFLG_SIZEGADGET|WFLG_DEPTHGADGET,
                    WA_IDCMP,       IDCMP_CLOSEWINDOW|IDCMP_NEWSIZE|IDCMP_MOUSEBUTTONS,
                    TAG_DONE);

        }
        if (win_info)
        {
/*            rasbuffer=(char *)AllocRaster(sC->Width,sC->Height);
            if (rasbuffer==NULL)
                    printf("Erreur allocation buffer info\n");
            InitTmpRas(P_tmpras,rasbuffer,RASSIZE(sC->Width,sC->Height));
            win_info->RPort->AreaInfo=&myareainfo;
            win_info->RPort->TmpRas=P_tmpras;                            */
            signal_win_info=(1<<win_info->UserPort->mp_SigBit);     /* allocation TmpRas*/
            win_info->RPort->AreaInfo=&myareainfo;
            /*win_info->RPort->AreaInfo=&win_info_Area;*/
            win_info->RPort->TmpRas=rpG->TmpRas;
            Map_Give_Type_Proj(PROJ_FLAT);
            aff_info(Pref.country_sel,win_info->RPort,win_info);
            Map_Give_Type_Proj(aux_Type_Proj);
            give_size_window(wG->Width,wG->Height);
            WaitBlit();
            /* libération TmpRas */
            /*FreeRaster(rasbuffer,sC->Width,sC->Height);
            free(P_tmpras);*/
        }
        else
        {
            Aff_Error("Error opening info window!\n(not enough memory)\n",
            "OK");
            printf("Error opening info window!\n(not enough memory)\n");
        }
        /*ActivateWindow(wG);*/ /* ne pas mettre d'activate window ici*/
    }
    else
    if (win_info==NULL)
        Aff_Error("You have not selected any country!","Understood");
}

void SAVEDS ASM info_func(void)
{
    cmd_info();
}

struct Hook info_hook=
{
    {NULL,NULL},
    (void *)info_func,
    NULL,NULL
};

void cmd_donn_ext(void)
{
        action=CMD_DONN_EXT;
}

void cmd_organisation(void)
{
    ULONG drapeau;
    static BOOL FirstTime=TRUE;
    get (My_App->WI_Organisation,MUIA_Window_Open,&drapeau);
    if (!drapeau)
    {
        if (FirstTime==TRUE)
            Create_CY_organisation();
        FirstTime=FALSE;
        set (My_App->WI_Organisation,MUIA_Window_Open,TRUE);
    }
}


void cmd_not_yet(void)
{

        Aff_Error(
                "I'm sorry,but this function is not yet\nimplemented. But soon...",
                "Ok,I'll wait for it!");        
}

void cmd_about(void)
{
        static struct EasyStruct ES={
                sizeof(struct EasyStruct),
                0,
                "About Magellan ",
"Magellan, by Thomas Landspurg & Olivier Collard\n\n"
"This program allow you to nagivate throught the\n"
"world, zooming evrywhere, etc...\n\n"
" This program will be shareware, but\n"
"without any limitation.\n",
"Thanks Man",
        };

        EasyRequest(wG, &ES, NULL);
}


/****************************************************************************/


/*******************************************************************************/

void    cmd_sel_cty(void)
{
        set(My_App->WI_Sel_Cty,MUIA_Window_Open,(APTR)TRUE);
}

//void SAVEDS ASM list_rawkey_func(REG(a2) Object * list,REG(a1) char *caractere)
/* le char * est bien un pointeur sur char et pas une chaîne */
/*{

}

struct Hook list_rawkey_hook =
{
        {NULL,NULL},
        (void *)list_rawkey_func,
        NULL,NULL
};*/


/***********************************************************************/
/*  display hooks pour les listes MUI                                  */
/***********************************************************************/

void
SAVEDS ASM LV_Elements_Display_func(REG(a2) char **array,REG(a1) ELEM_REM * element)
{
    array[0]=element->Nom;
}

struct Hook LV_Elements_Display_hook=
{
    {NULL,NULL},
    (void *)LV_Elements_Display_func,
    NULL,NULL
};

SAVEDS ASM LONG LV_Elements_Compare_func(REG(a1) ELEM_REM *elem1,REG(a2) ELEM_REM * elem2)

{
    return stricmp(elem1->Nom,elem2->Nom);
}

struct Hook LV_Elements_Compare_hook=
{
    {NULL,NULL},
    LV_Elements_Compare_func,
    NULL,NULL
};

void
SAVEDS ASM LV_classement_Display_func(REG(a2) char **array,REG(a1) DATA_TRI * donnee)
{
    static char buffer1[8],buffer2[32];
    sprintf(buffer1,"\0333%d",donnee->Rang);/* numero de ligne*/
    switch(Type_Donnee)
    {
        case DATA_INTEGER:
            Map_Put_Million(buffer2,"\0333\033r%s",donnee->valeur.data_int);
            /*sprintf(buffer2,"\0333%d",donnee->valeur.data_int);*/
            break;
        case DATA_FLOAT:
            sprintf(buffer2,"\0333\033r%.2f",donnee->valeur.data_float);
            break;
        case DATA_PERCENT:
            sprintf(buffer2,"\0333\033r%.2f%%",donnee->valeur.data_float);
            break;
        default:
            break;
    }    
    array[0]=buffer1;
    array[1]=tab_country[donnee->Num_Pays].name;
    array[2]=buffer2;
}

struct Hook LV_classement_Display_hook=
{
    {NULL,NULL},
    (void *)LV_classement_Display_func,
    NULL,NULL
};         

void
SAVEDS ASM LV_classement_Destruct_func(REG(a2) APTR pool,REG(a1) DATA_TRI * donnee)
{
    free(donnee);
}

struct Hook LV_classement_Destruct_hook=
{
    {NULL,NULL},
    (void *)LV_classement_Destruct_func,
    NULL,NULL
};

void cmd_sel_elem(void)
{               
    static BOOL liste_cree=FALSE;
    if (liste_cree==FALSE)
    {
        create_tab_elem();

        DoMethod(My_App->LV_Elements,MUIM_List_Insert,
            tab_elem,-1,MUIV_List_Insert_Bottom);

        liste_cree=TRUE;
        set(My_App->LV_Elements,MUIA_List_CompareHook,
            &LV_Elements_Compare_hook);
        set(My_App->LV_Elements,MUIA_List_DisplayHook,
            &LV_Elements_Display_hook);
        DoMethod(My_App->LV_Elements,MUIM_List_Sort);                           
    }
    set(My_App->WI_Sel_Elem,MUIA_Window_Open,(APTR)TRUE);
}       

/*******************************************************************************/
void
cmd_new_cty(int num_cty)
{
    //int     num_cty;
    int old_country;
    //num_cty=Dtb_Find_Country_Pos(p_name);
    if(num_cty!=-1)
    {
        old_country=Pref.country_sel;
        if (old_country!=-1)
            fill_country(old_country,0,rpG);
        Pref.country_sel=num_cty;
        if (Pref.country_sel==old_country) /* déselection d'un pays*/
        {
            set(My_App->TX_Selectionne,MUIA_Text_Contents,
                    0);
            Pref.country_sel=-1;
        }
        else
        {
            /*BOOL ouverture;*/
            int i;
            set(My_App->TX_Selectionne,MUIA_Text_Contents,
                    tab_country[num_cty].name);
            fill_country(Pref.country_sel,1,rpG);

            /* SI la fenetre d'info sur pays est ouverte */
            if(win_info != NULL)
                cmd_info();
            /*get (My_App->WI_Classement,MUIA_Window_Open,&ouverture);*/
            /* si la fenêtre de comparaison de données est ouverte */
            DoMethod(My_App->LV_classement,MUIM_List_Select,MUIV_List_Select_All,
                MUIV_List_Select_Off,NULL);
            for (i=0;;i++)
            {
                DATA_TRI * donnee;
                DoMethod(My_App->LV_classement,MUIM_List_GetEntry,i,&donnee);
                if (donnee==NULL)
                    break;
                if (donnee->Num_Pays==num_cty)
                {
                    DoMethod(My_App->LV_classement,MUIM_List_Jump,i);
                    DoMethod(My_App->LV_classement,MUIM_List_Select,i,
                        MUIV_List_Select_On,NULL);
                }
            }
        }
    }
}

/*********************************************************/
void cmd_redraw_datas(void)
{
    int     type_graph;
    int     dtb_sel=-1;

    get(My_App->LV_Datas,MUIA_List_Active,&dtb_sel);
    get(My_App->RA_Type_Graph,MUIA_Radio_Active,&type_graph);
    /*printf("Active:%d,type:%d\n",dtb_sel,type_graph);*/
    if(dtb_sel!=-1)
    {
        /*set(My_App->WI_Datas     ,MUIA_Window_Open,(APTR)FALSE);*/
        Dtb_Aff_Result(rpG,dtb_sel,My_App->LV_Select,type_graph);
    }
    else
    {
        Aff_Error("Select first a data type...\n","Ok");
    }
}               


/*********************************************************/
void cmd_new_3d(void)
{
    long    int     posx,posy,sizex,sizey;
    int     x,y,i;
    struct  Window  *win;
    struct  RastPort *rp;
    PREFS_3D        Prefs,Old_Prefs;
    static  int     tab_car[][2]={
                            {-100,-50},
                            { 100,-50},
                            { 100, 50},
                            {-100, 50},
                            {-100,-50}
                            };

    get(My_App->SL_x,MUIA_Slider_Level,&Pref.SL_x);
    get(My_App->SL_y,MUIA_Slider_Level,&Pref.SL_y);
    get(My_App->SL_z,MUIA_Slider_Level,&Pref.SL_z);
    get(My_App->IM_Rectangle,MUIA_Width   ,&sizex);
    get(My_App->IM_Rectangle,MUIA_Height  ,&sizey);
    get(My_App->IM_Rectangle,MUIA_LeftEdge,&posx);
    get(My_App->IM_Rectangle,MUIA_TopEdge ,&posy);

    Prefs.ax=2*PI*( ((float)Pref.SL_x)/360 );
    Prefs.ay=2*PI*( ((float)Pref.SL_y)/360 );
    Prefs.az=2*PI*( ((float)Pref.SL_z)/360 );

    sizex-=4;
    sizey-=4;
    posx+=2;
    posy+=2;
    Prefs.sizex=sizex;
    Prefs.sizey=sizey;
    Prefs.centre_x=posx+sizex/2;
    Prefs.centre_y=posy+sizey/2;
    Prefs.de=300;
    Old_Prefs=T3D_Set_Prefs(Prefs);
    Old_Prefs.ax=Prefs.ax;
    Old_Prefs.ay=Prefs.ay;
    Old_Prefs.az=Prefs.az;
    Old_Prefs.centre_x=wG->Width/2 ;
    Old_Prefs.centre_y=wG->Height/2;
    Old_Prefs.sizex   =wG->Width   ;
    Old_Prefs.sizey   =wG->Height  ;

    get(My_App->WI_Param,MUIA_Window_Window,&win);

    if(win!=NULL)
    {
        rp=win->RPort;
        SetAPen(rp,0);
        RectFill( rp,   posx,posy,
                        posx+sizex,posy+sizey
                );
        SetAPen(rp,1);

        for(i=0;i<5;i++)
        {
            x=tab_car[i][0]+Prefs.centre_x;
            y=tab_car[i][1]+Prefs.centre_y;
            T3D_Convert(x,y,&x,&y);
            if(i==0)
            {
                Move(rp,x,y);
            }
            else
            {
                Draw(rp,x,y);
            }
        }
    }
    T3D_Set_Prefs(Old_Prefs);
}

/*********************************************************/
/* Initialisation des fontes pour une fenetre donnees    */

void Main_Init_Font(void)
{
    struct Window * win;
    get(My_App->WI_Cmd,MUIA_Window_Window,&win);
    if (win!=NULL)
    /* on essaie de recuperer la police de MUI*/
            AskFont(win->RPort,&Text_Font_Attr);
    /*printf("nom de la fonte:%s\n",Text_Font_Attr.ta_Name);*/
    if (DiskfontBase)
    {
        Titre_Font = OpenDiskFont(&Titre_Font_Attr);
        Text_Font  = OpenDiskFont(&Text_Font_Attr );
        Map_Give_Default_Font( Titre_Font, Text_Font );
        SetFont(rpG,Text_Font);
    }
}

/********** Del Screen Image *****************************/
void Del_Image(void)
{
        Map_Del_Fond();
        if(sC){
                Map_Set_Default_Colors(vP);
                cmd_redraw();
                Flg_Fond=FALSE;
        }
}

/**********************************************************/

static VOID fail(APTR app,char *str)
{
    /*printf("Appel a fail\n");*/
        if (app)
                MUI_DisposeObject(app);

        if (MUIMasterBase)
                CloseLibrary(MUIMasterBase);

        if (str)
        {
                puts(str);
                exit(20);
        }
        exit(0);
}

static VOID init(VOID)
{
#ifndef _DCC
        if (!(MUIMasterBase = OpenLibrary (MUIMASTER_NAME,7)))
                fail(NULL,"Failed to open "MUIMASTER_NAME".");
#endif
}


/***********************************************************************/

/* Initialisation des differentes notifications pour MUI, */
/* ainsi que de l'etat de depart des fenetres MUI         */

void Init_Notification(void)
{
    static BOOL Flg_MUI_Initiated=FALSE;
    int     i;
    /*char buffer[16];*/
    /*int NumCol =57;*/
            
    for(i=0;Tab_Win[i]!=-1;i++)
    {
            set ( *(APTR *)((char *)My_App+Tab_Win[i]),
                            MUIA_Window_Screen,(APTR)sC);
            DoMethod( * (APTR *)((char *)My_App+Tab_Win[i]),
                     MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
                     *(APTR *)((char *)My_App+Tab_Win[i]),3,
                     MUIM_Set,MUIA_Window_Open,FALSE);
    }
    /* On indique qu'une fermeture de la fenetre Cmd principale= fermeture  */
    /* de l'appli                                                           */
    DoMethod(My_App->WI_Cmd,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
             My_App->App,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

    /* Fermeture de cmd panel                               */

    DoMethod(My_App->WI_Cmd_Panel,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
            My_App->App,2,MUIM_Application_ReturnID,ID_MODE_NOT_EDIT);
    /* notification des boutons*/
    if (Flg_MUI_Initiated==FALSE)
    {
/*        DoMethod(My_App->BT_Cmd_Panel,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->WI_Cmd_Panel,3,MUIM_Set,MUIA_Window_Open,TRUE);*/
/*      DoMethod(My_App->BT_Cmd_Panel,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_MODE_EDIT);*/
/*        DoMethod(My_App->BT_Fill,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_MODE_EDIT);*/
        DoMethod(My_App->BT_Info,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_CallHook,&info_hook);

/*        DoMethod(My_App->BT_Info,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_INFO);*/
/*        DoMethod(My_App->BT_Fill,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_FILL);*/
        DoMethod(My_App->BT_Move,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_MOVE_PT);
        DoMethod(My_App->BT_New_Border,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_CallHook,&new_border_hook);
        DoMethod(My_App->BT_End_Border,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_CallHook,&end_border_hook);
        DoMethod(My_App->BT_New_Point,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_CallHook,&new_point_hook);
        DoMethod(My_App->CY_Proj,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,
                My_App->App,2,MUIM_Application_ReturnID,ID_PROJ);

        DoMethod(My_App->BT_Zoom,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_CallHook,&zoom_hook);

        DoMethod(My_App->BT_Zoom_Out,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_CallHook,&zoom_out_hook);
        DoMethod(My_App->BT_Zoom_Reset,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_CallHook,&zoom_reset_hook);
        DoMethod(My_App->BT_Show_Pt,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_SHOW_PT);

//      sprintf(buffer,"\33\57",Num_Col);
        set(My_App->TX_Selectionne,MUIA_Text_PreParse,"\0333");

        DoMethod(My_App->BT_Element_Origine,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_ELEM_SET_POINT_ORIG);
        DoMethod(My_App->BT_Element_Destination,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_ELEM_SET_POINT_DEST);

        DoMethod(My_App->BT_Show_Seg,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_SHOW_SEG);
        DoMethod(My_App->BT_Edit,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_EDIT);
        DoMethod(My_App->BT_Create_Pays,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_CREATE_PAYS);
        DoMethod(My_App->BT_Trajectoire_OK,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_LOAD_TRAJ_OK);
        DoMethod(My_App->BT_Trajectoire_Cancel,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->WI_Trajectoire,3,MUIM_Set,MUIA_Window_Open,FALSE);
        DoMethod(My_App->IM_Trajectoire_Choix,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_SELECT_FILE_TRAJ);

        DoMethod(My_App->CH_LonLat,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
                My_App->CH_LonLat,3,MUIM_WriteLong,MUIV_TriggerValue,&Pref.Flg_Aff_LatLon);
        DoMethod(My_App->CH_Coord ,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
                My_App->CH_Coord,3,MUIM_WriteLong,MUIV_TriggerValue,&Pref.Flg_Aff_Coord);
        DoMethod(My_App->CH_Lacs  ,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
                My_App->CH_Lacs,3,MUIM_WriteLong,MUIV_TriggerValue,&Pref.Flg_Draw_Lac);
        DoMethod(My_App->CH_Riv   ,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
                My_App->CH_Riv,3,MUIM_WriteLong,MUIV_TriggerValue,&Pref.Flg_Draw_Riv);
        DoMethod(My_App->CH_Noms  ,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
                My_App->CH_Noms,3,MUIM_WriteLong,MUIV_TriggerValue,&Pref.Flg_Aff_Country_Name);
        DoMethod(My_App->CH_Fill_Pays  ,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
                My_App->CH_Fill_Pays   ,3,MUIM_WriteLong,MUIV_TriggerValue,&Pref.Flg_Fill_Country);
        DoMethod(My_App->CH_Elements  ,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
                My_App->CH_Elements   ,3,MUIM_WriteLong,MUIV_TriggerValue,&Pref.Flg_Aff_Elem_Rem);
        DoMethod(My_App->CH_Trajectoires  ,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
                My_App->CH_Trajectoires   ,3,MUIM_WriteLong,MUIV_TriggerValue,&Pref.Flg_Aff_Trajectoires);
        DoMethod(My_App->CH_Distance  ,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
                My_App->CH_Distance   ,3,MUIM_WriteLong,MUIV_TriggerValue,&Pref.Flag_Distance);
        DoMethod(My_App->CH_Scale  ,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
                My_App->CH_Scale   ,3,MUIM_WriteLong,MUIV_TriggerValue,&Pref.Flag_Echelle);
        DoMethod(My_App->CH_Area  ,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,
                My_App->CH_Area   ,3,MUIM_WriteLong,MUIV_TriggerValue,&Pref.Flag_Surface);
                
/*        DoMethod(My_App->BT_Cmd_Panel,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->WI_Cmd_Panel,3,MUIM_Set,MUIA_Window_Open,TRUE);*/

        DoMethod(My_App->LV_Pays,MUIM_Notify,MUIA_Listview_DoubleClick,TRUE,
                My_App->App,2,MUIM_Application_ReturnID,ID_NEW_CTY);
        DoMethod(My_App->LV_Continent  ,MUIM_List_Insert,cont_name,-1,
                MUIV_List_Insert_Bottom);
        DoMethod(My_App->LV_Continent, MUIM_Notify,MUIA_Listview_SelectChange,TRUE,
                My_App->App,2,MUIM_CallHook,&continent_select_hook);
        DoMethod(My_App->LV_Continent, MUIM_List_Select,MUIV_List_Select_All,
                MUIV_List_Select_On,NULL);
        
        DoMethod(My_App->LV_Elements,MUIM_Notify,MUIA_Listview_DoubleClick,TRUE,
                My_App->App,2,MUIM_Application_ReturnID,ID_INFO_ELEM);
/*      DoMethod(My_App->RA_Proj,MUIM_Notify,MUIA_Radio_Active,MUIV_EveryTime,
                My_App->App,2,MUIM_Application_ReturnID,ID_PROJ);*/

/*
        DoMethod(My_App->PC_Pref_Pages,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,
            My_App->App,2,MUIM_Application_ReturnID,ID_GROUPE_CHG);
*/

        DoMethod(My_App->SL_x,MUIM_Notify,MUIA_Slider_Level,MUIV_EveryTime,
                My_App->App,2,MUIM_Application_ReturnID,ID_PR_X);
        DoMethod(My_App->SL_y,MUIM_Notify,MUIA_Slider_Level,MUIV_EveryTime,
                My_App->App,2,MUIM_Application_ReturnID,ID_PR_Y);
        DoMethod(My_App->SL_z,MUIM_Notify,MUIA_Slider_Level,MUIV_EveryTime,
                My_App->App,2,MUIM_Application_ReturnID,ID_PR_Z);

/*        DoMethod(My_App->IM_Load_Data_Ext,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_SELECT_FILE);
*/
        DoMethod(My_App->BT_Redraw_Ext_Data,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_REDRAW_EXT);

        DoMethod(My_App->BT_Redraw,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_REDRAW);

        DoMethod(My_App->BT_Effacer_Fond,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_NO_FOND);

        DoMethod(My_App->BT_voir_donnee_externe,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_CallHook,&aff_donnee_externe_hook);

        set(My_App->TX_nom_element,MUIA_Text_PreParse,"\0333");
        set(My_App->TX_pays_element,MUIA_Text_PreParse,"\0333");

/*
        DoMethod(My_App->BT_Param_3D,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->WI_Param_3D,3,MUIM_Set,MUIA_Window_Open,TRUE);
*/
        DoMethod(My_App->BT_Vers_Deselect,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_DESELECT);

        DoMethod(My_App->BT_Vers_Select,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_SELECT);

        DoMethod(My_App->BT_Continent,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_CallHook,&Classement_Continent_hook);

        DoMethod(My_App->BT_Tout_Sel,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_CallHook,&Tout_Selectionner_hook);

        DoMethod(My_App->BT_Tout_Des,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_CallHook,&Tout_Deselectionner_hook);

        DoMethod(My_App->BT_Petits_Pays,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_CallHook,&Petits_Pays_hook);

        DoMethod(My_App->BT_Ok_Datas,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_OK_DATAS);

        DoMethod(My_App->BT_Editer,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->WI_Param_Datas,3,MUIM_Set,MUIA_Window_Open,TRUE);

        DoMethod(My_App->IM_Fichier_Donnee,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_SELECT_FILE_DONNEE);

        DoMethod(My_App->BT_Ajouter_Donnee,MUIM_Notify,MUIA_Pressed,FALSE,
                My_App->App,2,MUIM_Application_ReturnID,ID_ADD_DATA);

/*          DoMethod(My_App->WI_Param_3D,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
                My_App->WI_Param_3D,3,MUIM_Set,MUIA_Window_Open,FALSE);

DoMethod(My_App->WI_Donnees,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
        My_App->App,2,MUIM_Application_ReturnID,ID_AFF_DONNEES);
*/
        set(My_App->LV_classement,MUIA_List_Format,",,");
        set(My_App->LV_classement,MUIA_List_DisplayHook,
                        &LV_classement_Display_hook);
        set(My_App->LV_classement,MUIA_List_DestructHook,
                        &LV_classement_Destruct_hook);
        DoMethod(My_App->LV_classement,MUIM_Notify,MUIA_Listview_DoubleClick,TRUE,
                My_App->App,2,MUIM_Application_ReturnID,ID_CLASSEMENT_CTY);
        Flg_MUI_Initiated=TRUE;
    }
    set(My_App->WI_Cmd,MUIA_Window_Open,(APTR)TRUE);
}

void Close_Windows(void)
{
    int     i;

    /*printf("%lx %lx\n",My_App->WI_Cmd,*(APTR *)((char *)My_App+Tab_Win[0]));*/
    for (i=0;Tab_Win[i]!=-1;i++)
    {
        set(*(APTR *)((char *)My_App+Tab_Win[i]),MUIA_Window_Open,(APTR)FALSE);
    }

    if (MyMenu)
    {
        ClearMenuStrip(wG);
        FreeMenus(MyMenu);
        MyMenu=NULL;
    }
    if (win_info!=0)
        CloseWindow(win_info);
    win_info=NULL;
    signal_win_info=0;
    if (win_localisation!=0)
        CloseWindow(win_localisation);
    win_localisation=NULL;
    signal_win_localisation=0;
    if (WI_Drapeaux!=NULL)
        Do_Action((int)ID_CLOSE_DRAP);
    if(wG!=0)
    {
        if(buffer_rast!=NULL)
            FreeRaster(buffer_rast,wG->Width,wG->Height+wG->TopEdge);
        buffer_rast=NULL;
        CloseWindow(wG);
    }
}

        
/****************** init_screen **********************/
BOOL
Init_Screen(BOOL FirstTime)
{
    struct  Screen  *New_sC,*Old_sC;
    BOOL    Flg_Err;
    int     Etape;

    Flg_Err=FALSE;
    Etape=0;

    while((Etape<7)&&(Flg_Err==FALSE))
    {
        switch(Etape)
        {
        case 0:
            /* Ouverture d'un nouvel ecran  */
            /*                              */
            if (FirstTime==TRUE)
            {
                struct Screen * workbench=LockPubScreen("Workbench");
                New_sC = OpenScreenTags(        NULL,
                        SA_Depth    , (ULONG) Pref.Depth     ,
                /*        SA_Overscan , Pref.OScan,  */
                        SA_DisplayID, GetVPModeID(&(workbench->ViewPort)) ,
                        SA_Pens     ,(ULONG)Tab_Pens,
                        TAG_DONE);
                UnlockPubScreen("Workbench",workbench);
            }
            else
            {       
                New_sC = OpenScreenTags(        NULL,
                        SA_Depth    , (ULONG) Pref.Depth     ,
          /*              SA_Overscan , Pref.OScan,  */
                        SA_DisplayID, Pref.DisplayId  ,
                        SA_Pens     ,(ULONG)Tab_Pens,
                        TAG_DONE
                );
            }
            /* Si l'ecran a pu etre ouvert */
            /* sinon on garde l'ancien    */

            if(New_sC!=NULL)
            {
                /* on le prend comme nouvel ecran */
                Old_sC=sC;
                sC=New_sC;
                /* il faut vider la queue de messages sans prendre 
                                en compte */
                Prise_En_Compte=FALSE;
            };
            if (sC==NULL)
                Flg_Err=TRUE;
            break;
        case 1:

            /* Ferme les fenetres eventuellement ouvertes     */
            /* et on libere les ressources (rastport, etc...) */

            Close_Windows();

            /*NewWindowStructure1.Screen = sC;*/
            Pref.Width=sC->Width;
            Pref.Height=sC->Height;
            Pref.Depth=sC->BitMap.Depth;
            wG=OpenWindowTags(
                    NULL,
                    WA_Left,        0,
                    WA_Top,         0,
                    WA_Width,       sC->Width,
                    WA_Height,      sC->Height,
                    WA_CustomScreen,sC,
                    WA_MinWidth,    20L,
                    WA_MinHeight,   20L,
                    WA_MaxWidth,    -1,
                    WA_MaxHeight,   -1,
                    WA_Flags,       WFLG_BACKDROP|WFLG_REPORTMOUSE|
                        WFLG_BORDERLESS|WFLG_ACTIVATE|WFLG_NOCAREREFRESH,
                    WA_IDCMP,       IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|
                        /*IDCMP_GADGETUP|IDCMP_GADGETDOWN|*/IDCMP_MENUPICK|
                        IDCMP_CLOSEWINDOW|IDCMP_VANILLAKEY|IDCMP_RAWKEY,
                    TAG_DONE);
            vP = &sC->ViewPort;
            /*NewWindowStructure1.Height=sC->Height-NewWindowStructure1.TopEdge;
            NewWindowStructure1.Width =sC->Width;
            wG=OpenWindow(&NewWindowStructure1);
            */

            if(wG==NULL)Flg_Err=TRUE;
            break;
        case 2:
            Init_Notification();
            if(Old_sC!=NULL)CloseScreen(Old_sC);
            break;
        case 3:
            {
               APTR *MyVisualInfo;

               MyVisualInfo=GetVisualInfo(sC,TAG_END);
               if (MyVisualInfo)
                {
                    MyMenu=CreateMenus(Menu,TAG_END);
                    if (MyMenu)
                    {
                        LayoutMenus(MyMenu,MyVisualInfo,TAG_END);
                        SetMenuStrip(wG,MyMenu);
                    }
                    else
                        Flg_Err=TRUE;
                    FreeVisualInfo(MyVisualInfo);       
                }
                else
                    Flg_Err=TRUE;
            }
            break;
        case 4:
            Map_Set_Default_Colors(vP);

            rpG = wG->RPort;
            signal_wG=(1<<wG->UserPort->mp_SigBit);

            give_size_window(wG->Width,wG->Height);
            /* Window 2 */
    
            buffer_rast=(char *)AllocRaster(wG->Width,wG->Height+wG->TopEdge);
            /*buffer_rast=(char *)AllocRaster(Pref.Width,Pref.Height);*/
            if  (buffer_rast==NULL)
            {
                printf("Error allocating buffer!\n");
                Flg_Err=TRUE;
            }
            break;
        case 5:
            InitArea(&myareainfo,areabuffer,MAX_AREA_PT);
            rpG->AreaInfo=&myareainfo;
            InitTmpRas(&tmpras,buffer_rast,RASSIZE(wG->Width,wG->Height+wG->TopEdge));
            rpG->TmpRas=&tmpras;
            if(rpG->TmpRas==NULL)Flg_Err=TRUE;
            break;
        default:
            break;
        }
        Etape++;
    }
    return(Flg_Err);
}

/***********************************************************************/
/*  display et compare hooks pour les listes MUI                       */
/***********************************************************************/

void
SAVEDS ASM LV_Pays_Display_func(REG(a2) char **array,REG(a1) COUNTRY * pays)
{
    array[0]=strdup(pays->name);
    array[1]=strdup(pays->name);
}

struct Hook LV_Pays_Display_hook=
{
    {NULL,NULL},
    (void *)LV_Pays_Display_func,
    NULL,NULL
};

SAVEDS ASM LONG LV_Pays_Compare_func(REG(a1) COUNTRY *pays1,REG(a2) COUNTRY * pays2)

{
    return stricmp(pays1->name,pays2->name);
}

struct Hook LV_Pays_Compare_hook=
{
    {NULL,NULL},
    LV_Pays_Compare_func,
    NULL,NULL
};


/*********************************************************** main() ****/

main( int argc,  char **argv   )
{
        char    *name;
        int     Etape;
        BOOL    Flg_Err;

        Flg_Err=FALSE;
        Etape=0;

        /* Initialisation .                             */
        /* Il y a un certian nombre d'etape. Si chaque  */
        /* etape s'est initialisee correctement, on     */
        /* passe a la suivante. Sinon, on sort et on    */
        /* libere les ressources                        */

        while((Etape<17)&&(Flg_Err==FALSE)){

           switch(Etape){
            case 0:
                    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
                    if (IntuitionBase == NULL)
                    {
                            printf("intuition is not here.  where are we?\n");
                            Flg_Err=TRUE;
                    }
                    GfxBase      = (struct GfxBase *)OpenLibrary("graphics.library", 0);
                    DiskfontBase = OpenLibrary("diskfont.library", 37L);
                    GadToolsBase = OpenLibrary("gadtools.library", 37L);
                    AslBase=(struct Library *)OpenLibrary(AslName,0);
                    /* ce n'est pas grave si la librairie fastsincos n'est pas la*/
                    FastsincosBase=(struct Library *)OpenLibrary("fastsincos.library",0);
                    if (FastsincosBase==NULL)
                        printf("Warning: Fastsincos library is not here\n");
                    if (GfxBase->LibNode.lib_Version>=39)
                    {
                        /* nous en sommes en Workbench 3.0 ou + */
                        DataTypeBase = OpenLibrary ("datatypes.library", 39L);
                        LocaleBase=OpenLibrary("locale.library",38L);
                        if (LocaleBase!=NULL)
                            MyLocale=OpenLocale(NULL);
                    }
                    break;
            case 1:

                /* allocation mémoire tableaux tab_vect et tab_country */
                    init_map_datas();
                    /*initialisation des valeurs de Pref*/
                    init_preference();
                    break;
            case 2:

                    init();                 /* Mui function */
                    break;

            case 5:
                    My_App=CreateApp();

                    if (!My_App->App) Flg_Err=TRUE;
                    break;
            
            case 6:
                    areabuffer=AllocMem(MAX_AREA_PT*5,0L);
                    //areabuffer=(WORD *)malloc(MAX_AREA_PT*5*sizeof(WORD));
                    if(areabuffer==NULL)Flg_Err=TRUE;
                    break;
            case 7:
                    {
                    srand(time(NULL));
                    if (load_preference()==FALSE)
                    {
                            /*printf("impossible de charger les préférences!\n");*/
                            /* Ouverture des ecrans et fenetres */
                            Flg_Err=Init_Screen(TRUE);
                    }
                    else
                    {
                        /* init screen                          */
                        if (Init_Screen(FALSE)==TRUE)
                            Flg_Err=Init_Screen(TRUE);
                        else
                            Flg_Err=FALSE;
                        /* initialisation des paramètres vue 3D */
                        /* il faut que wG existe                */
                        cmd_new_3d();
                    }
                    }
                    break;
            case 8:
                {
                    /* affichage ecran d'intro*/
                    ULONG modeID = LORES_KEY;
                    DisplayInfoHandle displayhandle;
                    struct DimensionInfo dimensioninfo;
                    if ((displayhandle=FindDisplayInfo(modeID))!=NULL)
                        if (GetDisplayInfoData(displayhandle,(UBYTE *) &dimensioninfo,
                            sizeof(struct DimensionInfo),DTAG_DIMS,NULL)!=0)
                            if (dimensioninfo.MaxDepth>=8)
                                /* mode AGA ? (on peut afficher l'image)*/
                                sC_Image=Iff_Load_And_Open("intro");
                            else
                                /* mode ECS */
                                sC_Image=Iff_Load_And_Open("intro8");
                    if (rand()>RAND_MAX/2)
                            sprintf(Titre_Ecran,"Amiglobe   (C)1994 by %s & %s",
                                            "Olivier Collard","Thomas Landspurg");
                    else
                            sprintf(Titre_Ecran,"Amiglobe   (C)1994 by %s & %s",
                                            "Thomas Landspurg","Olivier Collard");
                    SetWindowTitles(wG,(UBYTE *)~0,Titre_Ecran );

                    /*
                            SetMenuStrip(wG,&MenuList1);
                    */
                    
                    /*cmd_warning();*//* affichage blabla */
                }
                    break;
            case 9:

                    IPrint(rpG,10,wG->Height/2,"Loading Data, Please Wait....",-1);

                    if(argc<2)
                    {
                            name="new_lac";
                    }
                    else
                    {
                            name=argv[1];
                    }
                    /*IPrint(rpG,10,wG->Height/2+20,"Reading Points..",-1);*/
                    Flg_Err=read_map_bin(name);
                    break;
            case 10:
                    /*IPrint(rpG,10,wG->Height/2+40,"Reading Zones...",-1);*/
                    Flg_Err=read_zones(name,rpG);
                    break;
            case 11:
                    /*IPrint(rpG,10,wG->Height/2+60,"Reading Infos..",-1);*/
                    Flg_Err=read_database(name);
                    break;
            case 12:
                    /*IPrint(rpG,10,wG->Height/2+80,"Reading Statistics..",-1);
                    Flg_Err=Dtb_Read_Stats();
                    Dtb_Create_Liste_Data(My_App->LV_Datas);
                    Dtb_Create_Liste_Data(My_App->LV_Data1);
                    Dtb_Create_Liste_Data(My_App->LV_Data2);*/
                    break;
            case 13:
                    Main_Init_Font();
                    break;
            case 14:
                /* Cree la liste des pays a inserer dans les listes utilisant */
                /* les pays                                             */
                {
                   int  max_country=get_max_country();
                   COUNTRY * pays;
                   int i;
                    set(My_App->LV_Pays,MUIA_List_CompareHook,
                        &LV_Pays_Compare_hook);
                    set(My_App->LV_Pays,MUIA_List_DisplayHook,
                        &LV_Pays_Display_hook);
                    set(My_App->LV_Select,MUIA_List_CompareHook,
                        &LV_Pays_Compare_hook);
                    set(My_App->LV_Select,MUIA_List_DisplayHook,
                        &LV_Pays_Display_hook);
                    set(My_App->LV_Deselect,MUIA_List_CompareHook,
                        &LV_Pays_Compare_hook);
                    set(My_App->LV_Deselect,MUIA_List_DisplayHook,
                        &LV_Pays_Display_hook);
                    for(i=0;i<max_country;i++)
                    {
                        pays=&(tab_country[i]);
                        DoMethod(My_App->LV_Pays,MUIM_List_Insert,
                            &pays,1,MUIV_List_Insert_Sorted);
                        DoMethod(My_App->LV_Select,MUIM_List_Insert,
                            &pays,1,MUIV_List_Insert_Sorted);
                    }
                    if (Pref.country_sel!=-1)
                        set(My_App->TX_Selectionne,MUIA_Text_Contents,
                            tab_country[Pref.country_sel].name);

                }
                break;
            case 15:
                if (Pref.point_orig.existe==TRUE
                    && Pref.point_dest.existe==TRUE)
                    distance_orig_dest=distance(
                        Pref.point_orig.longitude,
                        Pref.point_orig.latitude,
                        Pref.point_dest.longitude,
                        Pref.point_dest.latitude);
                break;

            case 16:
            /*ScreenToFront(sC);*/
                    /* effacement de l'ecran d'intro*/
                    if (sC_Image) Del_Ext_Image();
                    break;  
            default:
                    break;
            }
           Etape++;
           /*printf("Etape:%d\n",Etape);*/
        
        }
        if(Flg_Err==FALSE){
                cmd_redraw();
                main_loop();
        }
        
        
        /* Liberation des ressources */
        Close_Windows();
        /*printf("disposeapp\n");*/
        if(My_App)DisposeApp(My_App);
        Dispose_Organisation();
        /*printf("disposescreen\n");*/
        if(sC!=0)CloseScreen(sC);
        sC=NULL;
        /*printf("del image \n");*/
        Del_Image();
        /*printf("del ext image \n");*/
        Del_Ext_Image();
        /*printf("free map data\n");*/
        free_map_datas();
        /*printf("close libraries\n");*/
        if (AslBase              ) CloseLibrary(AslBase);
        if (DiskfontBase         ) CloseLibrary(DiskfontBase);
        if (GadToolsBase         ) CloseLibrary(GadToolsBase);
        if (GfxBase != NULL      ) CloseLibrary((struct Library *)GfxBase);
        if (IntuitionBase != NULL) CloseLibrary((struct Library *)IntuitionBase);
        if (MUIMasterBase        ) CloseLibrary(MUIMasterBase);
        if (DataTypeBase         ) CloseLibrary(DataTypeBase);
        if (FastsincosBase       ) CloseLibrary(FastsincosBase);
        /* sauvegarde des préférences */
        if (save_preference()==FALSE)
                {}
                /*printf("impossible de sauver les préférences!\n");*/
        return(0);
        
}

 
void Main_Print_Deg(struct RastPort *rpG,int x,int y,int val1,int val2)
{
        char    buffer[100];
        char east,north;
        int     deg1=ABS(val1/100);
        int min1=(ABS(val1)%100)*60/100;
        int     deg2=ABS(val2/100);
        int min2=(ABS(val2)%100)*60/100;
        if (val1<0)
                east='W';
        else
                east='E';
        if (val2<0)
                north='S';
        else
                north='N';

        
        sprintf(buffer,"%d°%02d'%c   ",deg1,min1,east);
        set(My_App->TX_lon,MUIA_Text_Contents,buffer);
/*      IPrint(rpG,x,y,buffer,-1);*/
        sprintf(buffer,"%d°%02d'%c   ",deg2,min2,north);
        set(My_App->TX_lat,MUIA_Text_Contents,buffer);  
/*      IPrint(rpG,x+200,y,buffer,-1);*/
        
}


void mouse_action(int x,int y,ULONG class,UWORD code)
{
   static       int     new_pays,old_pays=-1;
   static       int     old_x,old_y;
   static       int     start_zx,start_zy;
   /*int old_country;*/

   char buffer[100]     ;

   switch (class)
   {
    case MOUSEBUTTONS:
        if(code == SELECTDOWN)
        {
            switch (action)
            {
            case CMD_SELECT:
                cmd_new_cty(find_country(x,y));
                /*
                old_country=Pref.country_sel;
                if (old_country!=-1)
                {
                    fill_country(Pref.country_sel,0,rpG);
                }
                Pref.country_sel=find_country(x,y);
                if (Pref.country_sel==old_country) /* déselection d'un pays*/
                /*{
                    set(My_App->TX_Selectionne,MUIA_Text_Contents,
                        0);
                    Pref.country_sel=-1;
                }
                else
                {
                    if (Pref.country_sel!=-1)
                    {
                        set(My_App->TX_Selectionne,MUIA_Text_Contents,
                            get_pays_name(Pref.country_sel));
                        fill_country(Pref.country_sel,1,rpG);
                        if (win_info!=NULL)
                            cmd_info();
                    }
                }  */
                    break;
            case CMD_NEW_POINT:
            {
                int code;
                conv_inv_xy(&x,&y);
                tab_new_border[longueur_new_border*3+1]=x;
                tab_new_border[longueur_new_border*3+2]=y;
                if (longueur_new_border==0)
                {
                        char * buffer;
                get(My_App->STR_Code,MUIA_String_Contents,(APTR)&buffer);
                        code=atoi(buffer);
                }
                else
                {
                get(My_App->SL_Code_Point,MUIA_Slider_Level,(APTR)&code);
                        MyMove(rpG,
                                tab_new_border[longueur_new_border*3-2],
                                tab_new_border[longueur_new_border*3-1]);
                        MyDraw(rpG,x,y,code);
                }                                       
                tab_new_border[longueur_new_border*3]=code;
                longueur_new_border++;
                /*printf("nouveau point lon=%d lat=%d code=%d nbre de points=%d \n",
                        x,y,code,longueur_new_border);*/
            }
                break;
            case CMD_ZOOM:
                flg_press=TRUE;
                old_x=x;
                old_y=y;
                start_zx=x;
                start_zy=y;
                SetDrMd(rpG,COMPLEMENT);
                break;
            case CMD_POINT_ORIG:
                conv_inv_xy(&x,&y);
                if (point_est_dans(x,y,&clip_max)==TRUE)
                {
                    Pref.point_orig.longitude=x;
                    Pref.point_orig.latitude=y;
                    Pref.point_orig.existe=TRUE;
                    map_aff_point(rpG,&Pref.point_orig,0);
                    if (Pref.point_dest.existe==TRUE)
                        distance_orig_dest=distance(
                        Pref.point_orig.longitude,
                        Pref.point_orig.latitude,
                        Pref.point_dest.longitude,
                        Pref.point_dest.latitude);
                    Update_Points();
                    Draw_Chemin(rpG);
                    if (Pref.Flag_Distance)
                        aff_distance();
                    /*reselect(rpG);*/
                    action=CMD_SELECT;
                }
                break;
            case CMD_POINT_DEST:
                conv_inv_xy(&x,&y);
                if (point_est_dans(x,y,&clip_max)==TRUE)
                {
                    Pref.point_dest.longitude=x;
                    Pref.point_dest.latitude=y;
                    Pref.point_dest.existe=TRUE;
                    map_aff_point(rpG,&Pref.point_dest,1);
                    if (Pref.point_orig.existe==TRUE)
                            distance_orig_dest=distance(
                            Pref.point_orig.longitude,
                            Pref.point_orig.latitude,
                            Pref.point_dest.longitude,
                            Pref.point_dest.latitude);
                    Update_Points();
                    Draw_Chemin(rpG);
                    if (Pref.Flag_Distance)
                        aff_distance();

                    /*reselect(rpG);*/
                    action=CMD_SELECT;
                }
                break;
            case CMD_DONN_EXT:
            {
                char buffer[256];
                char buffer2[256];
                int i;
                int distance=100000;
                ELEM_REM * element,*sel_element=tab_country[0].P_Elem_Rem;
                conv_inv_xy(&x,&y);
                for (i=0;i<get_max_country();i++)
                {
                    element=tab_country[i].P_Elem_Rem;
                    while (element!=NULL)
                    {
                        if (iabs(element->Lon-x)+iabs(element->Lat-y)<distance)
                        {
                            distance=iabs(element->Lon-x)+iabs(element->Lat-y);
                            sel_element=element;
                        }
                        element=element->P_Next;
                    }
                }
                set(My_App->WI_Info_Element,MUIA_Window_Open,TRUE);
                set(My_App->TX_nom_element,MUIA_Text_Contents,sel_element->Nom);
                switch(sel_element->Type)
                {
                case CAPITALE:
                    sprintf(buffer,"is the capital city of ");
                    Map_Put_Million(buffer2,"Population:%s hab",sel_element->caracteristique);
                    break;
                case VILLE:
                    sprintf(buffer,"is a city of ");
                    Map_Put_Million(buffer2,"Population:%s hab",sel_element->caracteristique);
                    break;
                case MONTAGNE:
                    sprintf(buffer,"is a mountain of ");
                    Map_Put_Million(buffer2,"Altitude:%s m",sel_element->caracteristique);
                    break;
                case MONUMENT:
                    sprintf(buffer,"is a monument of ");
                    if (sel_element->caracteristique>50)
                            sprintf(buffer2,"Very interesting");
                    else
                            sprintf(buffer2,"Less interesting");
                    break;
                }
                set(My_App->TX_type_element,MUIA_Text_Contents,buffer);
                i=pays(sel_element);
                if (i!=-1)
                        set(My_App->TX_pays_element,MUIA_Text_Contents,
                                        tab_country[i].name);                           
                sprintf(buffer,"%d°%d'",-(sel_element->Lat)/100,
                        iabs((sel_element->Lat%100)*60/100));
                set(My_App->TX_latitude_element,MUIA_Text_Contents,buffer);
                sprintf(buffer,"%d°%d'",(sel_element->Lon)/100,
                        iabs((sel_element->Lon%100)*60/100));
                set(My_App->TX_longitude_element,MUIA_Text_Contents,buffer);
                set(My_App->TX_caracteristique_element,MUIA_Text_Contents,buffer2);
                if (fopen(sel_element->Nom,"r")==NULL)
                    set(My_App->BT_voir_donnee_externe,MUIA_Disabled,TRUE);
                else
                    set(My_App->BT_voir_donnee_externe,MUIA_Disabled,FALSE);
                ActivateWindow(wG);
                action=CMD_SELECT;
            }
            break;
            }/* End of switch/case sur cmd */

        }
        else
        if ( code == SELECTUP )
        {
            switch (action)
            {
            case CMD_ZOOM:
                y=(x-start_zx)*wG->Height/wG->Width+start_zy;
                if((x!=start_zx)&&(y!=start_zy))
                {
                    map_set_zoom(rpG,start_zx,start_zy,x,y);
                }
                flg_press=FALSE;
                action = CMD_SELECT;
                Pref.Flg_Aff_Coord=TRUE;
            break;
            }
        }
        break;

        case MOUSEMOVE :

            switch (action)
            {
            case CMD_NEW_POINT:
            case CMD_POINT_ORIG:
            case CMD_POINT_DEST:
            case CMD_DONN_EXT:
            case CMD_SELECT:
                if (Pref.Flg_Aff_Coord)
                 {
                    new_pays=find_country(x,y);
                    if (new_pays!=old_pays)
                    {
                        if (new_pays!=-1)
                        {
                            set (My_App->TX_courant,MUIA_Text_Contents,
                                    get_pays_name(new_pays));
                        }
                        else
                        set (My_App->TX_courant,MUIA_Text_Contents,
                                    0);

                        }

                        conv_inv_xy(&x,&y);
                        Main_Print_Deg(rpG,300,Text_Font->tf_YSize+2,x,-y);
                        if ((Pref.Flag_Distance)&&(Pref.point_dest.existe==FALSE)
                                &&(Pref.point_orig.existe==TRUE))
                        {
                            sprintf(buffer,"Distance: %.6f km  ",distance(x,y,
                                Pref.point_orig.longitude,Pref.point_orig.latitude));
                            IPrint(rpG,8,Text_Font->tf_YSize*2+2,buffer,-1);
                        }
                    }
                    break;
                      case CMD_ZOOM:
                                if (flg_press)
                {
                                        y=(x-start_zx)*wG->Height/wG->Width+start_zy;
                                        map_draw_rect(rpG,start_zx,start_zy,old_x-start_zx,old_y-start_zy);
                                        map_draw_rect(rpG,start_zx,start_zy,x-start_zx,y-start_zy);
                                        old_x=x;
                                        old_y=y;
                                }
                                break;
                }
                break;
        }/* End of case */
}


void Do_Action(int In_Action)
{
    int     sel;
    switch(In_Action)
    {
        case MUIV_Application_ReturnID_Quit:
                quit();
                break;
        case ID_ABOUT:
                cmd_about();
                break;
        case ID_DEFAULT_PREF:
                init_preference();
                Init_Screen(FALSE);
                cmd_redraw();
                SetWindowTitles(wG,(UBYTE *)~0,Titre_Ecran );
                break;
        case ID_LOAD_TRAJ:
                /*printf("affichage fenetre trajectoire\n");*/
                set(My_App->WI_Trajectoire,MUIA_Window_Open,TRUE);
                break;
        case ID_LOAD_TRAJ_OK:
                read_trajectoires();
                break;
        case ID_SELECT_FILE_TRAJ:
                cmd_select_file(My_App->STR_Trajectoire_Fichier);                       
                break;
        case ID_REDRAW:
                cmd_redraw();
                break;
        case ID_INFO:
                cmd_info();
                break;
        case ID_FILL:
                //cmd_fill();
                break;
        case ID_MOVE_PT:
                cmd_move_pt();
                break;
        case ID_SHOW_PT:
                cmd_show_pt();
                break;
        case ID_SHOW_SEG:
                cmd_show_seg();
                break;
/*              case ID_ZOOM:
                cmd_zoom();
                break;
        case ID_ZOOM_RESET:
                cmd_zoom_reset();
                break;
        case ID_ZOOM_OUT:
                cmd_zoom_out();
                break;*/
        case ID_EDIT:
                cmd_edit_pays();
                break;
        case ID_CREATE_PAYS:
                cmd_create_pays();
                break;
        case ID_MODE_EDIT:
                flg_edit=TRUE;
                break;
        case ID_MODE_NOT_EDIT:
                flg_edit=FALSE;
                break;
/*              case ID_NEW_BORDER:
                tab_new_border=(short int*)malloc(500);
                break;
        case ID_NEW_POINT:
                action=CMD_NEW_POINT;
                break;
        case ID_END_BORDER:
                cmd_end_border();
                break;*/
        case ID_SEL_CTY:
                cmd_sel_cty();
                break;
        case ID_NEW_CTY:
             {
                int     cty;
                COUNTRY * pays;
                get(My_App->LV_Pays,MUIA_List_Active,&cty);
                DoMethod(My_App->LV_Pays,MUIM_List_GetEntry,cty,&pays);
                cmd_new_cty(Dtb_Find_Country_Pos(pays->name));
             }
             break;
        case ID_CLASSEMENT_CTY:
            {
                int cty;
                DATA_TRI * donnee;
                get(My_App->LV_classement,MUIA_List_Active,&cty);
                DoMethod(My_App->LV_classement,MUIM_List_GetEntry,cty,&donnee);
                cmd_new_cty(donnee->Num_Pays);
            }
            break;
        case ID_SEL_CONT:
                set(My_App->WI_Sel_Cont,MUIA_Window_Open,(APTR)TRUE);
                break;
        case ID_SEL_ELEM:
                cmd_sel_elem();
                        break;
        case ID_INFO_ELEM:
                {
                int elem,i;
                ELEM_REM * element;
                /*char * nom_elem;*/
                char buffer[30];
                get(My_App->LV_Elements,MUIA_List_Active,&elem);
                DoMethod(My_App->LV_Elements,MUIM_List_GetEntry,elem,&element);
                if (element==NULL)
                        break;
                set(My_App->TX_Selected_Element,MUIA_Text_Contents,
                                element->Nom);
                i=pays(element);
                if (i!=-1)
                        set(My_App->TX_Pays,MUIA_Text_Contents,
                                        tab_country[i].name);                           
                sprintf(buffer,"Lat:%d°%d'",-(element->Lat)/100,
                        iabs((element->Lat%100)*60/100));
                set(My_App->TX_Latitude,MUIA_Text_Contents,buffer);
                sprintf(buffer,"Lon:%d°%d'",(element->Lon)/100,
                        iabs((element->Lon%100)*60/100));
                set(My_App->TX_Longitude,MUIA_Text_Contents,buffer);
                switch (element->Type)
                {
                case CAPITALE:
                case VILLE:
        Map_Put_Million(buffer,"Pop:%s hab",element->caracteristique);
                        /*sprintf(buffer,"Pop:%d hab",element->caracteristique);*/
                        break;
                case MONTAGNE:
        Map_Put_Million(buffer,"Altitude:%s m",element->caracteristique);
                        /*sprintf(buffer,"Altitude:%d m",element->caracteristique);*/
                        break;
                case MONUMENT:
                        if (element->caracteristique>50)
                                sprintf(buffer,"Very interesting");
                        else
                                sprintf(buffer,"Less interesting");
                        break;
                default:
                                sprintf(buffer,"Element type unknown");
                        break;
                }
                set(My_App->TX_Caracteristique,MUIA_Text_Contents,buffer);      
                free (buffer);
                }
                break;
        case ID_ELEM_SET_POINT_ORIG:
                {
                /*char * nom_elem;*/
                int elem;
                ELEM_REM * element;
                get(My_App->LV_Elements,MUIA_List_Active,&elem);
                DoMethod(My_App->LV_Elements,MUIM_List_GetEntry,elem,&element);
                Pref.point_orig.longitude=element->Lon;
                Pref.point_orig.latitude=element->Lat;
                Pref.point_orig.existe=TRUE;
                map_aff_point(rpG,&Pref.point_orig,0);
                if (Pref.point_dest.existe==TRUE)
                        distance_orig_dest=distance(
                        Pref.point_orig.longitude,
                        Pref.point_orig.latitude,
                        Pref.point_dest.longitude,
                        Pref.point_dest.latitude);
                Update_Points();
                Draw_Chemin(rpG);
                }
                break;
        case ID_ELEM_SET_POINT_DEST:
                {
                /*char * nom_elem;*/
                int elem;
                ELEM_REM * element;
                get(My_App->LV_Elements,MUIA_List_Active,&elem);
                DoMethod(My_App->LV_Elements,MUIM_List_GetEntry,elem,&element);
                Pref.point_dest.longitude=element->Lon;
                Pref.point_dest.latitude=element->Lat;
                Pref.point_dest.existe=TRUE;
                map_aff_point(rpG,&Pref.point_dest,0);
                if (Pref.point_orig.existe==TRUE)
                        distance_orig_dest=distance(
                        Pref.point_orig.longitude,
                        Pref.point_orig.latitude,
                        Pref.point_dest.longitude,
                        Pref.point_dest.latitude);
                }
                Update_Points();
                Draw_Chemin(rpG);
                break;                                  
        case ID_NOT_YET:
                cmd_not_yet();
                break;
        case ID_AFF_DATA:
                cmd_redraw_datas();
                break;
        case ID_AFF_DATA_EXT:
                set(My_App->WI_Chargement,MUIA_Window_Open,(APTR)TRUE);
                break;
/*        case ID_SELECT_FILE:
                        cmd_select_file(My_App->STR_Fichier);   
                        break;*/
        case ID_SELECT_FILE_DONNEE:
                cmd_select_file(My_App->STR_fichier_Donnee);
                break;
        case ID_REDRAW_EXT:
                        cmd_redraw_ext();
                        break;            
        case ID_DETAIL_ALL:
                map_set_prof(0);
                break;
        case ID_DETAIL_1:
                map_set_prof(1);
                break;
        case ID_DETAIL_2:
                map_set_prof(2);
                break;
        case ID_DETAIL_3:
                map_set_prof(3);
                break;
        case ID_DETAIL_4:
                map_set_prof(4);
                break;
        case ID_DETAIL_5:
                map_set_prof(5);
                break;
        case ID_LOCALISATION:
                cmd_localisation();
                        break;
        case ID_POINT_ORIG_PUT:
                cmd_point_orig();
                        break;
        case ID_POINT_ORIG_REM:
                Pref.point_orig.existe=FALSE;
                        break;                  
        case ID_POINT_DEST_PUT:
                cmd_point_dest();
                        break;
        case ID_POINT_DEST_REM:
                Pref.point_dest.existe=FALSE;
                        break;  
        case ID_DONN_EXT:
                cmd_donn_ext();
                        break;
        case ID_ORGANISATION:
                cmd_organisation();
                break;
        case ID_PREFS_AFF:
                set(My_App->WI_Param,MUIA_Window_Open,(APTR)TRUE);
                cmd_new_3d();
                break;
        case ID_DONNEES:
                Dtb_Read_Stats();
                /*Dtb_Create_Liste_Data(My_App->LV_Datas);*/
                set(My_App->WI_Datas,MUIA_Window_Open,(APTR)TRUE);
                break;
        case ID_SCREEN:
                cmd_change_screen();
                break;
        case ID_GROUPE_CHG:
/*
                get(My_App->PC_Pref_Pages,MUIA_Cycle_Active,&sel);
                DoMethod(My_App->PC_Pref_Groupes,MUIM_Set,MUIA_Group_ActivePage,sel);
*/
                break;
        case ID_PR_X:
        case ID_PR_Y:
        case ID_PR_Z:
                cmd_new_3d();
                break;
        case ID_NO_FOND:
                Del_Image();
                break;
        case ID_SELECT:
                bascule(My_App->LV_Deselect,My_App->LV_Select);
                break;
        case ID_DESELECT:
                bascule(My_App->LV_Select,My_App->LV_Deselect);
                break;
        case ID_OK_DATAS:
                cmd_redraw_datas();
                break;          
        case ID_AFF_DRAP:
                if (WI_Drapeaux==NULL && MUIMasterBase->lib_Version>=8)
                {
                    /* Création d'une fenêtre MUI comprenant un groupe    */
                    /* virtuel contenant des groupes horizontaux contenant*/
                    /* chacun au plus 10 bitmaps representant les drapeaux*/
                    int     i,max_country;
                    CLIP    clip;
                    char    Buffer[100];
                    int     *P_Tab_Drapeau;
                    struct RastPort rp;
                    struct BitMap bm;
                    APTR GR_Drapeaux,GR_Drapeaux_Virtuel;
                    WI_Drapeaux = WindowObject,
                            MUIA_Window_Title, "Choose a flag",
                                WindowContents, ScrollgroupObject,
                                    MUIA_Scrollgroup_Contents, GR_Drapeaux_Virtuel=VirtgroupObject,
                                    VirtualFrame,
                                    Child, GR_Drapeaux=GroupObject,
                                        MUIA_Group_Horiz, TRUE,
                                        End,
                                    End,
                                End,
                            End;
                    DoMethod(My_App->App,OM_ADDMEMBER,WI_Drapeaux);
                    set ( WI_Drapeaux, MUIA_Window_Screen,(APTR)sC);
                    InitBitMap(&bm,4,wG->Width,wG->Height/5);
                    bm.Planes[0]=AllocRaster(wG->Width,wG->Height/5);
                    bm.Planes[1]=AllocRaster(wG->Width,wG->Height/5);
                    bm.Planes[2]=AllocRaster(wG->Width,wG->Height/5);
                    bm.Planes[3]=AllocRaster(wG->Width,wG->Height/5);

                    if (bm.Planes[3]!=NULL)
                    {
                        int nombre_drapeaux=0;
                        for (i=0;i<MAX_COUNTRY;i++)
                            Tab_Drapeaux[i]=NULL;
                        InitRastPort(&rp);
                        rp.BitMap=&bm;
                        rp.TmpRas=rpG->TmpRas;
                        rp.AreaInfo=&myareainfo;
                        max_country=get_max_country();
                        clip.minx=0;
                        clip.miny=0;
                        for(i=0;i<max_country;i++)
                        {

                            sprintf(Buffer,"flags/%s.flag",tab_country[i].name);
                            if(Flag_Read(Buffer,&P_Tab_Drapeau))
                            {
                              Object * bitmap;
                              if (nombre_drapeaux>10)
                              {
                                  GR_Drapeaux=GroupObject,
                                        MUIA_Group_Horiz, TRUE,
                                        End;
                                  DoMethod(GR_Drapeaux_Virtuel,OM_ADDMEMBER,GR_Drapeaux);
                                  nombre_drapeaux=0;
                              }
                              nombre_drapeaux++;
                              Tab_Drapeaux[i]=(struct BitMap *)malloc(sizeof(struct BitMap));
                              InitBitMap(Tab_Drapeaux[i],4,wG->Width/10,wG->Height/10);
                              Tab_Drapeaux[i]->Planes[0]=AllocRaster(wG->Width/10,wG->Height/10);
                              Tab_Drapeaux[i]->Planes[1]=AllocRaster(wG->Width/10,wG->Height/10);
                              Tab_Drapeaux[i]->Planes[2]=AllocRaster(wG->Width/10,wG->Height/10);
                              Tab_Drapeaux[i]->Planes[3]=AllocRaster(wG->Width/10,wG->Height/10);
                              if (Tab_Drapeaux[i]!=NULL)
                              {
                                  clip.maxx=clip.minx+wG->Width/10;
                                  clip.maxy=clip.miny+wG->Height/10;
                                  Flag_Draw(&rp,&clip,P_Tab_Drapeau,wG->WScreen->ViewPort.ColorMap);
                                  BltBitMap(&bm,0,0,Tab_Drapeaux[i],0,0,wG->Width/10,wG->Height/10,
                                            (long)0xc0,0xff,TAG_END);
                                  bitmap=BitmapObject,
                                    MUIA_Bitmap_Bitmap, Tab_Drapeaux[i],
                                    MUIA_Bitmap_Height,wG->Height/10,
                                    MUIA_Bitmap_Width,wG->Width/10,
                                    MUIA_FixHeight,wG->Height/10,
                                    MUIA_FixWidth,wG->Width/10,
                                    MUIA_InputMode, MUIV_InputMode_RelVerify,
                                    End;

                                  DoMethod(GR_Drapeaux,OM_ADDMEMBER,bitmap);
                                  DoMethod(bitmap,MUIM_Notify,MUIA_Pressed,FALSE,
                                    bitmap,3,MUIM_CallHook,&drapeau_select_hook,i);
                              }
                              free(P_Tab_Drapeau);
                            }
                         }
                         FreeRaster(bm.Planes[0],wG->Width,wG->Height/5);
                         FreeRaster(bm.Planes[1],wG->Width,wG->Height/5);
                         FreeRaster(bm.Planes[2],wG->Width,wG->Height/5);
                         FreeRaster(bm.Planes[3],wG->Width,wG->Height/5);

                         DoMethod(WI_Drapeaux, MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
                            My_App->App,2,MUIM_Application_ReturnID,ID_CLOSE_DRAP);
                         set(WI_Drapeaux,MUIA_Window_Open,TRUE);
                     }
                }
                break;
        case ID_CLOSE_DRAP:
            {
                int i;
                set(WI_Drapeaux,MUIA_Window_Open,FALSE);
                DoMethod(My_App->App,OM_REMMEMBER,WI_Drapeaux);
                MUI_DisposeObject(WI_Drapeaux);
                WI_Drapeaux=NULL;
                for (i=0;i<MAX_COUNTRY;i++)
                    if (Tab_Drapeaux[i]!=NULL)
                    {
                        FreeRaster(Tab_Drapeaux[i]->Planes[0],wG->Width/10,wG->Height/10);
                        FreeRaster(Tab_Drapeaux[i]->Planes[1],wG->Width/10,wG->Height/10);
                        FreeRaster(Tab_Drapeaux[i]->Planes[2],wG->Width/10,wG->Height/10);
                        FreeRaster(Tab_Drapeaux[i]->Planes[3],wG->Width/10,wG->Height/10);
                        free(Tab_Drapeaux[i]);
                        Tab_Drapeaux[i]=NULL;
                    }
            }
                break;
        case ID_PROJ:
                get(My_App->CY_Proj,MUIA_Cycle_Active,&sel);
                Pref.Flg_Proj_3D=FALSE;
                switch(sel){
                        case 0:
                                Pref.Type_Proj=PROJ_FLAT;
                                break;
                        case 1:
                                Pref.Type_Proj=PROJ_MERCATOR;
                                break;
                        case 2:
                                Pref.Type_Proj=PROJ_GLOBE;
                                break;
                        case 3:
                                Pref.Type_Proj=PROJ_FLAT;
                                Pref.Flg_Proj_3D=TRUE;
                                break;
                        default:
                                Pref.Type_Proj=PROJ_FLAT;
                                break;
                }
                redraw_map(-1,rpG);
                /*printf("Type proj:%d\n",Pref.Type_Proj);*/
                break;
        case ID_ADD_DATA:
                cmd_add_data();
                break;

        default:
                        break;
    }
}

/****************************************************************************/

void main_loop(void)
{
    UWORD   code            ;
    ULONG   class           ;
    APTR    object          ;
    int     x,y             ;
    int localisation_x,localisation_y;
    ULONG   signal_rec      ;
    int     Code_Action     ;
    ULONG   signal_mui=-1   ;
    USHORT  selection       ;

    struct  IntuiMessage *message;  /* the message the IDCMP sends us */
    struct  IntuiMessage copy_msg;

    {
/*      ULONG signal=0;*/

        while (quit_flag==FALSE)
        {
            if(signal_mui!=0)
            {
                do
                {
        Code_Action=DoMethod(My_App->App,MUIM_Application_Input,&signal_mui);
                    Do_Action(Code_Action);
                }
                      while(signal_mui==0);

                if(quit_flag==FALSE)
                {
                    signal_rec=Wait( signal_mui | signal_wG | signal_win_info |
                                    signal_win_image|signal_win_localisation);
                }
            
                if(signal_rec & signal_win_image)
                {
                    Del_Ext_Image();
                }
                if(signal_rec & signal_wG)
                {
                    while( (message = (struct IntuiMessage *)
                        GetMsg(wG->UserPort) ) != NULL)
                    {
                        copy_msg=*message;
                        code = message->Code;  /* MENUNUM */
                        object = message->IAddress;  /* Gadget */
                        class = message->Class;
                        x= message->MouseX;
                        y= message->MouseY;

                        ReplyMsg((struct Message *)message);
                        if (flg_edit==FALSE)
                        {
                            switch(class)
                            {
                            case VANILLAKEY:
                                if ((code>='0')&&(code<='5'))
                                {
                                   map_set_prof(code-'0');
                                   redraw_map(-1,rpG);
                                }
                                break;
                            case RAWKEY:
                                if (code==CURSORUP)
                                        map_move(rpG,0,-10);
                                    if (code==CURSORDOWN)
                                                map_move(rpG,0,10);
                                    if (code==CURSORLEFT)
                                    map_move(rpG,-10,0);
                                    if (code==CURSORRIGHT)
                                    map_move(rpG,10,0);
                             
                                    break;
        
                            case MENUPICK:
                                Prise_En_Compte=TRUE;
                                for(selection=code;
                (selection!=MENUNULL)&&(selection!=0);
                selection=(ItemAddress(MyMenu,(LONG)selection))->NextSelect)
                                {
    Code_Action=(int)GTMENUITEM_USERDATA(ItemAddress(MyMenu,(LONG)selection));
                                    if(Prise_En_Compte)
                                        Do_Action(Code_Action);
                                }
                                break;
                            case MOUSEBUTTONS:
                            case MOUSEMOVE :
                                  mouse_action(x,y,class,code);
                                  break;
                            }/* End of case */
                        }/* end de if */
                        else
                        {
                            edit_loop(&copy_msg);
                        }
                    }/*end de while*/
                            
                }
            }
            if(signal_rec & signal_win_info)
            {
               while( (win_info!=NULL)&&
                      (message = (struct IntuiMessage *)
                            GetMsg(win_info->UserPort) ) != NULL)
               {
                    code = message->Code;
                    class = message->Class;
                    ReplyMsg((struct Message *)message);

                    switch(class)
                    {
                    case IDCMP_CLOSEWINDOW:
                        CloseWindowSafely(win_info);
                        win_info=NULL;
                        signal_win_info=0;
                        ActivateWindow(wG);
                        break;
                    case IDCMP_NEWSIZE:
                        cmd_info();
                        break;
                    case IDCMP_MOUSEBUTTONS:
                        /*printf("bouton appuyé dans l'info-win\n");*/
                        break;
                    }/* End of case */
                                
                }/* end de while*/
            }/* end de if*/
            if(signal_rec & signal_win_localisation)
            {
               static BOOL Flg_MOUSEBUTTONS=FALSE;
               while( (win_localisation!=NULL)&&
                      (message = (struct IntuiMessage *)
                            GetMsg(win_localisation->UserPort) ) != NULL)
               {
                    int X,Y;    /* declaration */
                    code = message->Code;
                    class = message->Class;
                    ReplyMsg((struct Message *)message);

                    switch(class)
                    {
                    case IDCMP_CLOSEWINDOW:
                        CloseWindowSafely(win_localisation);
                        win_localisation=NULL;
                        signal_win_localisation=0;
                        ActivateWindow(wG);
                        break;
                    case IDCMP_NEWSIZE:
                        cmd_localisation();
                        break;
                   case IDCMP_MOUSEBUTTONS:
                        if (Flg_MOUSEBUTTONS==FALSE)
                        {
                            localisation_x=message->MouseX
                                -win_localisation->BorderLeft;
                            localisation_y=message->MouseY
                                -win_localisation->BorderTop;
                            X=localisation_x*(clip_max.maxx-clip_max.minx)
        /(win_localisation->Width-win_localisation->BorderRight)+clip_max.minx;
                            Y=(localisation_y)*(clip_max.maxy-clip_max.miny)
                                /(win_localisation->Height)+clip_max.miny;
                            map_center(rpG,X,Y);   
                            Flg_MOUSEBUTTONS=TRUE;
                        }
                        else
                            Flg_MOUSEBUTTONS=FALSE;
                        break;
                    }/* End of case */
                                
                }/* end de while */
            }/* end de if */            

        }/* Fin de WHILE quit_flag */
    }
    return;
}


void aff_surface(void)
{
    char buffer[60];
    int sx=Pref.clip_cur.maxx-Pref.clip_cur.minx;
    float angle_maxy=Map_Convert_Angle(Pref.clip_cur.maxy);
    float angle_miny=Map_Convert_Angle(Pref.clip_cur.miny);
    double h=RAYON*(sin(angle_maxy)-sin(angle_miny));
    double surface=2*PI*RAYON*h*sx/36000;
    sprintf(buffer,"View area: %.0f km²",
                    surface);
    IPrint(rpG,8,3*Text_Font->tf_YSize+2,buffer,-1);
}

void aff_distance(void)
{
    if ((Pref.point_orig.existe==TRUE)&&(Pref.point_dest.existe==TRUE))
    {
        char buffer[40];
        sprintf(buffer,"distance: %.3f km",distance_orig_dest);
        IPrint(rpG,8,2*Text_Font->tf_YSize+2,buffer,-1);
    }
}


/****************************************************** reselect() ******/

void reselect(struct RastPort * r)
{
    int i;
    if (Pref.country_sel!=-1)
    {
        fill_country(Pref.country_sel,1,r);
    }
    if (Pref.Flag_Echelle)
        Map_Aff_Echelle(r);
    if (Pref.Flag_Surface)
        aff_surface();
    if (Pref.Flg_Aff_Elem_Rem)
        for (i=0;i<get_max_country();i++)
            aff_elem_rem(r,i);
    if (Pref.Flg_Aff_Trajectoires)
        Map_Aff_Trajectoires(r,NULL);
    map_aff_point(r,&Pref.point_orig,0);
    map_aff_point(r,&Pref.point_dest,1);
    Draw_Chemin(r);
    if (Pref.Flag_Distance)
        aff_distance();
    ActivateWindow(wG);
}

void aff_elem_rem(struct RastPort * r,int numero)
{
    if (tab_country[numero].P_Elem_Rem!=NULL)
    {
        ELEM_REM * element=tab_country[numero].P_Elem_Rem;
        while (element!=NULL)
        {
            Map_Aff_Elem_Rem(r,element);
            element=element->P_Next;
        }
    }
}       
