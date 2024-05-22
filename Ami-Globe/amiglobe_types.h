#define MAX_VECT    180
#define MAX_COUNTRY 1000
#define MAX_PT      500
#define MAX_AREA_PT MAX_PT*50
#define MAX_BORDER  5000
#define MAX_TRAJECTOIRE 100

#define MAX_CONTINENT 2

/* rayon de la Terre            */
/* d'après:         rayon equatorial    rayon polaire   */
/* Hayford              6378.388            6356.912    */
/* Krasovskii           6378.245            6356.863    */
/* approx: la Terre est une sphère de rayon moyen 6371.221 km*/
#define RAYON 6371.221

typedef enum{ PROJ_FLAT,PROJ_MERCATOR,PROJ_GLOBE }E_TYPE_PROJ;
typedef enum{ TRACE_FIL,TRACE_PLEIN }E_TYPE_TRACE;

typedef struct  vect
{
    short   int Code;
    short   int x,y;
}   VECT;

typedef struct  clip
{
    int  minx,miny,maxx,maxy;/* Carre dans lequel s'inscrit l'element */
}   CLIP;
/***********************************/
/* Structure definissant un border */
/***********************************/
typedef struct border
{
    BOOL flg_draw;      /* Drapeau indiquant qu'il a deja ete dessinee */
    CLIP    b_clip;
    int nb_vect;        /* Nombre de pt de ce bord      */
    VECT *p_vect;       /* pointeur sur le tableau de points    */

    LONG    offset;     /* Position du tableau de pt a la resloution max */
    int nb_vect_bis;
    VECT *p_vect_maxres;    /* Pointeur sur le tableau de points a la reso   */
                /* maximum                       */
}   BORDER;


/* Definition d'un element remarquable -> Ville, montagne, monument,...*/

typedef enum{ CAPITALE,VILLE,MONUMENT,MONTAGNE}E_TYPE;

typedef struct  Elem_Rem
{
    E_TYPE  Type;       /* Type (par ex, capitale, ville    */
    char    *Nom;       /* Nom de cet element remarquable   */
    int caracteristique; /* caracteristique de l'élem: pop, altitude... */
    int Lon,Lat;    /* Position             */
    struct  Elem_Rem *P_Next;/* Ptr sur le prochain element remarq. */
}   ELEM_REM;

    
#define NB_CAR_ABBREV   4
/************************/
/* Definition d'un pays */
/************************/

typedef struct country
{
    char *name;     /* ptr sur le nom du pays       */
    int  continent;     /* Numero du continent auquel app. pays */
    int  nb_borders;    /* nombre de bords de ce pays       */
    int  *p_borders;    /* pointeur sur le tableau des bords    */
    char * off_name;
    int superficie;
    int population;
    int PIB;
    int pnb1,pnb2,pnb3;
    char * presentation;    
    CLIP c_clip;
    ELEM_REM *P_Elem_Rem; /* Liste chainee des elements remarquables */ 
}   COUNTRY;

typedef struct point
{
    int longitude;
    int latitude;
    BOOL existe;
}   POINT;

typedef struct trajectoire
{
    char * nom;      /* nom de la trajectoire*/
    VECT * vecteurs; /* tableau de vecteurs composant la trajectoire*/ 
    int nb_vect;   /* nombre de vecteurs de la trajectoire*/    
    CLIP clip;
}   TRAJECTOIRE;

/* PREFERENCES: structure des préférences */
typedef struct preference
{
    /* checkmarks de la fenêtre choix de la projection */
    LONG Flg_Aff_Coord;
    ULONG Flg_Draw_Lac;
    ULONG Flg_Draw_Riv;
    ULONG Flg_Aff_Country_Name;
    ULONG Flg_Fill_Country;
    ULONG Flg_Aff_Elem_Rem;
    ULONG Flg_Aff_Trajectoires;

    /* type de projection */
    E_TYPE_PROJ Type_Proj;
    
    /* type de tracé*/
    E_TYPE_TRACE Type_Trace;
    
    int Glob_Min;
    int Glob_Max;

    /* sliders de la vue 3D */
    long int SL_x;
    long int SL_y;
    long int SL_z;
    
    /* ecran */
    long DisplayId;
    long Width;
    long Height;
    long Depth;
    long OScan;

    /* booléens */
    ULONG Flag_Distance;
    ULONG Flag_Surface;
    ULONG Flag_Echelle;
    
    /* autres booléens*/
    ULONG Flg_Aff_LatLon;
    ULONG Flg_Proj_3D;

    /* points */
    POINT point_orig;
    POINT point_dest;

    /* pays courant sélectionné*/
    int country_sel;
    
    /*taille de la fenêtre*/
    int win_width;
    int win_height;
    
    /* profondeur de détail*/
    int prof;
    int prof_real;

    /* clip actuel */
    CLIP clip_cur;

    /* extensions futures*/
    char Dummy[40];
}   PREFERENCE;
    
