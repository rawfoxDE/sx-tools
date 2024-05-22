/* Prototypes for functions defined in
map_function.c
 */

void give_size_window(int , int );
void IPrint(struct RastPort * , int , int , char * ,int color);
void IPrintFond(struct RastPort * , int , int , char * ,int color);
/*void TPrint(struct RastPort * , int , int , char * ,int, int);*/
void aff_country(int , struct RastPort * );
void aff_info(int , struct RastPort * ,struct Window *);
void print_pays(struct RastPort * );
void print_cont(struct RastPort * );
/*void aff_selection(int ,struct RastPort * );*/
int sel_country(struct Screen *,int , void (* )());
int find_country(int , int );
int     Find_Profondeur(int Prof);
void reset_bord(void);
void map_aff_country_name(COUNTRY * ,struct RastPort *);
void redraw_map(int , struct RastPort * );
void fill_country(int , int , struct RastPort * );
void init_map_datas(void);
void free_map_datas(void);
int     get_max_country(void);
char    *get_pays_name(int);

void    DrawSeg(struct  RastPort        *,VECT  *tab_pt,int,int,int);
int     map_find_pt(struct RastPort *,int       x,int   y,VECT  **pp_v,int      *p_num_pt,
        int     *p_num_border);

void    reset_zoom(struct RastPort *rpG);
void    map_show_bords(int country_sel,int start_bord,struct RastPort *rpG);
void    map_show_borders(struct RastPort        *rpG);
void    map_show_pt(struct      RastPort        *rpG);
void    map_reset_zoom(struct   RastPort        *rpG);
void    map_draw_rect(struct    RastPort *rpG,int       x,int   y,int   sx,
                        int     sy);
void map_set_zoom(struct RastPort       *rpG,int        sx,int sy,int   lx,int ly);
void map_move(struct RastPort   *rpG,int        sx,int sy);
void map_center(struct RastPort *rpG,int        X,int Y);
void map_draw_segment(struct    RastPort        *,int   num_seg,int     color);
int map_find_segment(int        x,int   y);
void redraw_map_complete(struct RastPort        *rpG);
int     tstclip(int     mx,int  my,int  MX,int  MY,CLIP *p_clip);
void tstpt(int x,int y,int *pmx,int *pmy,int *pMX,int *pMY);
int tst_end( FILE    *input);
void    map_set_prof(int        profondeur);

void    conv_clip(void);
void    conv_inv_xy(int *,int *);
void    conv_xy(int *,int *);


void Map_Put_Million(char *In_Buffer,char *P_Format,int data);
void Map_Init_Graph(struct RastPort *rpG,char *Titre);
void Map_Aff_Data(  struct RastPort *rpG,DATA_TRI *Data_Tri,int value);
void Map_End_Graph (struct RastPort *rpG);
void Map_Set_Default_Colors(struct ViewPort *vp);
int  Map_Get_Plages(    struct  RastPort *rpG   ,
                        int     plages_dem      ,
                        E_Type_Data Type        ,
                        int nb_data,
                        int     min             ,
                        int     max             ,
                        E_Type_Graph In_Type_Graph
                );
void    Map_Give_Type_Trace(E_TYPE_TRACE Type);
void    Map_Give_Default_Font( 
        struct TextFont *Titre_Font,
        struct TextFont *Text_Font
        );
void    Map_Aff_LatLon(struct RastPort *rpG);
void    Map_Give_Type_Proj(E_TYPE_PROJ Type);
float   Map_Convert_Angle(int angle);
void Map_Aff_Echelle(struct RastPort * rpG);
void    Map_Aff_Fond(char *pc,struct Screen *sC);
void    Map_Del_Fond(void);
void Map_Aff_Trajectoires(struct RastPort *,TRAJECTOIRE ** );

void MyDraw( struct  RastPort *rpG, int x, int y,  int code);
void MyMove( struct  RastPort *rpG, int x, int y);  
void check_border(int num_vect,int *Out_Nb_Vect,VECT **Out_P_Vect);

extern  char    *cont_name[];
