/********************************/
/* AmiGlobe			*/
/* T.Landspurg			*/
/********************************/

/* INCLUDES ********************************************************** */

/* MUI */
#include <libraries/mui.h>

#include <exec/types.h>
#include <exec/io.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <libraries/dos.h>
#include <intuition/intuition.h>
#include <graphics/gfxmacros.h>

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
/* ANSI C */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>


#define REG(x) register __ ## x
#define ASM    __asm
#define SAVEDS __saveds

#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/muimaster_pragmas.h>
#include <pragmas/asl_pragmas.h>

/*
extern struct Library *IntuitionBase;
extern struct Library *GfxBase;
extern struct Library *DOSBase;
*/
extern struct Library *UtilityBase;
extern struct Library *IconBase;

struct Library *MUIMasterBase;


/*
#include <libraries/reqbase.h>
*/

#ifdef	LATTICE
#include	<proto/all.h>
#endif

#include "amiglobe_types.h"
#include "main_protos.h"

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

/*** ids ***/

enum ids {
	ID_DUMMY,
	ID_ABOUT,
	ID_REDRAW,
	ID_INFO,
	ID_FILL,
	ID_MOVE_PT,
	ID_ZOOM,
	ID_ZOOM_OUT,
	ID_SHOW_PT,
	ID_EDIT,
	ID_MODE_EDIT,
	ID_MODE_NOT_EDIT
	};


struct NewMenu Menu[] =
{
	{ NM_TITLE, "Project"  , 0 ,0,0,(APTR)0            },
	{ NM_ITEM , "About..." ,"?",0,0,(APTR)ID_ABOUT     },
	{ NM_ITEM , NM_BARLABEL, 0 ,0,0,(APTR)0            },
	{ NM_ITEM , "Quit"     ,"Q",0,0,(APTR)MUIV_Application_ReturnID_Quit },
	{ NM_END  , NULL       , 0 ,0,0,(APTR)0            },
};


static APTR AP_Main;

static WI_Cmd,WI_Cmd_Edit;


static	APTR	BT_Cmd_Panel,BT_Redraw,BT_Info,BT_Fill;
static	APTR	BT_Move,BT_Zoom,BT_Zoom_Out,BT_Show_Pt,BT_Edit;

#define HProp                    PropObject, PropFrame, MUIA_Prop_Horiz, TRUE, MUIA_FixHeight, 8, MUIA_Prop_Entries, 111, MUIA_Prop_Visible, 10, End
#define VProp                    PropObject, PropFrame, MUIA_Prop_Horiz, FALSE, MUIA_FixWidth , 8, MUIA_Prop_Entries, 111, MUIA_Prop_Visible, 10, End

int	max_border=0;

/* Tableau de pointeurs sur les vecteurs */
	
BORDER	*tab_vect;
COUNTRY	*tab_country;

int	max_country=0;		/* Nombre max de pays	    */
int	country_sel=-1;		/* Pays courant selectionne */
BOOL	flg_edit=FALSE;

/**********************************/
/* Tableau des noms de continents */
/**********************************/

char	*cont_name[]={"Europe","New contry"};

long minx, miny, maxx, maxy;
int	cx=320;
int	cy=100;


USHORT quit_flag = FALSE;


struct IntuitionBase *IntuitionBase=0;
struct GfxBase	*GfxBase=0;
struct ReqBase	*ReqBase;

struct Window *OpenWindow();
struct Screen *OpenScreen();

struct Screen *sC;
struct ViewPort vP;

struct Window *wG,*cmd_wG;	/* we fetch the RastPort pointer from here */
struct RastPort *rpG,*cmd_rpG;

struct	TmpRas	tmpras;		/* Pour les areadraw */
struct	AreaInfo myareainfo;
WORD	areabuffer[MAX_AREA_PT*3];

int	signal_wG,signal_cmd_wG,signal_all;

/****************************************************************************/

/* This is for the event handler */
void quit(void)
{
	quit_flag = TRUE;
}
void cmd1(void)
{
	cmd_edit();
}
void cmd_redraw(void)
{
	printf("Cmd redraw!\n");
	redraw_map(-1);
}
void cmd_fill(void)
{
	int	i;
	for(i=0;i<max_country;i++){
		fill_country(i,i+1);
	}
}


void cmd_select_country(void)
{
	country_sel=sel_country(max_country,print_pays);
	aff_selection();

}

void cmd_sel_continent(void)
{
	int num_cont;
	printf("Sel continent!\n");
	num_cont=sel_country(MAX_CONTINENT,print_cont);
	redraw_map(num_cont);
}

void cmd_info(void)
{
	aff_info(country_sel);
}

/****************************************************************************/

/* get the PowerWindows 2.0 code */
#include "windowdef.h"

/****************************************************************************/

void	cmd_edit(void)
{
/*
	if(cmd_wG!=NULL)CloseWindow(cmd_wG);
	edit();
	cmd_wG = OpenWindow(&NewWindowStructure2);
*/
}

/****************************************************************************/

void IPrint(
	struct	RastPort *rpG,
	int	x,
	int	y,
	char	*p_texte
	)
{
	SetAPen(rpG,1);
	SetDrMd(rpG,JAM2);
	Move(rpG,x,y);
	Text(rpG,p_texte,strlen(p_texte));
}

/****************************************************************************/

void WaitButton(void)
{
	UWORD code;
	ULONG class;
	struct IntuiMessage *message;
	int	flg_quit;

	flg_quit=FALSE;
	do{
		WaitPort(wG->UserPort);
		while( (message = (struct IntuiMessage *)
				GetMsg(wG->UserPort) ) != NULL)
		{
			code = message->Code;  /* MENUNUM */
			class = message->Class;
			ReplyMsg((struct Message *)message);
			if(( class == MOUSEBUTTONS)&&( code == SELECTDOWN)){
				flg_quit=TRUE;
			}
		}
	}while(flg_quit==FALSE);
}

/****************************************************************************/

void aff_country(int	num_pays)
{
	int	omx,omy,OMX,OMY;
	char buffer[100];

	sprintf(buffer,"Info sur:%s",tab_country[num_pays].name);

	omx=minx;
	omy=miny;
	OMX=maxx;
	OMY=maxy;

	minx=tab_country[num_pays].c_clip.minx-50;
	miny=tab_country[num_pays].c_clip.miny-50;
	maxx=tab_country[num_pays].c_clip.maxx+50;
	maxy=tab_country[num_pays].c_clip.maxy+50;

	SetRast(rpG,0L);
	fill_country(num_pays,2);
	IPrint(rpG,50,20,buffer);

	minx=omx;
	miny=omy;
	maxx=OMX;
	maxy=OMY;
}

/****************************************************************************/

void aff_pnb(void)
{
	int	i;
	int	color;

	for(i=0;i<max_country;i++){
		color=(16*tab_country[i].pnb1)/300;
		fill_country(i,color);
	}
}

/****************************************************************************/

void	aff_info_page1(int	num_pays)
{
	char buffer[100];
	sprintf(buffer,"Info sur:%s",tab_country[num_pays].name);

	SetRast(rpG,0L);
	SetAPen(rpG,1L);
	SetBPen(rpG,0L);

	IPrint(rpG,50,20,buffer);
	sprintf(buffer,"Taux de croissance:%d",10);
	IPrint(rpG,10,50,buffer);
	sprintf(buffer,"Investissements:%d",10);
	IPrint(rpG,10,60,buffer);
	sprintf(buffer,"Habitants:%d",tab_country[num_pays].habitants);
	IPrint(rpG,10,70,buffer);
	sprintf(buffer,"Continent:%s",cont_name[tab_country[num_pays].continent]);
	IPrint(rpG,10,80,buffer);
	sprintf(buffer,"Pnb89:%d",tab_country[num_pays].pnb1);
	IPrint(rpG,10,90,buffer);
	sprintf(buffer,"Pnb90:%d",tab_country[num_pays].pnb2);
	IPrint(rpG,10,100,buffer);
	sprintf(buffer,"Pnb91:%d",tab_country[num_pays].pnb3);
	IPrint(rpG,10,110,buffer);

}
/******************************************************** aff_info() ******/

void aff_info(int num_pays)
{

	/* Verification qu'un pays est selectionne */
	printf("Aff info,num pays:%d\n",num_pays);
	if(num_pays!=-1){
/*
		ModifyIDCMP(wG,MOUSEBUTTONS);
*/
		/*************************/
		/* Affichage de la carte */
		/*************************/
		aff_country(num_pays);
		WaitButton();

		aff_info_page1(num_pays);
		WaitButton();

/*
		aff_pnb();
		WaitButton();
*/
/*
		ModifyIDCMP(wG,	NewWindowStructure1.IDCMPFlags);
*/
		redraw_map(-1);
	}
}

/******************************************************** print_pays() ****/

void print_pays(struct	RastPort *rpG)
{
	int	i,posx,posy;
	posx=0;
	posy=0;
	for(i=0;i<max_country;i++){
		if(posy>8){
			posy=0;
			posx++;
		}
		Move(rpG,10+posx*100,20+posy*10);
		Text(rpG,tab_country[i].name,strlen(tab_country[i].name));
		posy++;
	}
}

void print_cont(struct	RastPort *rpG)
{
	int	i;
	for(i=0;i<MAX_CONTINENT;i++){
		Move(rpG,10,20+i*10);
		Text(rpG,cont_name[i],strlen(cont_name[i]));
	}
}

/****************************************************************************/
void MyDraw(
	struct	RastPort *rpG,
	int	x,
	int	y
	)
{
	int	x1,y1;
	x1=(((x-minx)*(wG->GZZWidth))/(maxx - minx)),
	y1=(((y-miny)*(wG->GZZHeight))/(maxy-miny));
	Draw(rpG,x1,y1);
}

/****************************************************************************/

void MyMove(
	struct	RastPort *rpG,
	int	x,
	int	y
	)
{
	int	x1,y1;
	x1=(((x-minx)*(wG->GZZWidth))/(maxx - minx)),
	y1=(((y-miny)*(wG->GZZHeight))/(maxy-miny));
	Move(rpG,x1,y1);
}
/****************************************************************************/

int conv_inv_x(
	int	x
	)
{
	x=(x*(maxx-minx))/(wG->GZZWidth)+minx;
	return(x);
}
/****************************************************************************/
int conv_inv_y(
	int	y)
{
	y=(y*(maxy-miny))/(wG->GZZHeight)+miny;
	return(y);
}
/****************************************************************************/

int conv_x(x)
int	x;
{
	x=(((x-minx)*(wG->GZZWidth))/(maxx - minx));	
	return(x);
}
/****************************************************************************/

int conv_y(
	int	y
	)
{
	y=(((y-miny)*(wG->GZZHeight))/(maxy - miny));
	return(y);
}

/****************************************************************************/

void	draw_rect(
	struct	RastPort *rpG,
	int	x,
	int	y,
	int	sx,
	int	sy
	)
{
	Move(rpG,x,y);
	Draw(rpG,x+sx,y);
	Draw(rpG,x+sx,y+sy);
	Draw(rpG,x,y+sy);
	Draw(rpG,x,y);
}

void	aff_selection(void)
{
	char	buffer[100];

	if((country_sel>=max_country)||(country_sel<0)){
		country_sel=-1;
		sprintf(buffer,"Sel:          ");
	}else{
		sprintf(buffer,"Sel:%10s",tab_country[country_sel].name);
	}
	IPrint(rpG,200,20,buffer);
}
/******************************************************** sel_country() ****/

/************************************/
/* Procedure de selection d'un pays */
/************************************/

int	sel_country(
	int	max_sel,
	void	print_func()
	)
{

	UWORD code;
	ULONG class;
	APTR object;
	struct IntuiMessage *message;
	struct	Window *sel_wG;
	struct	RastPort  *sel_rpG;
	int	quit_flag;
	int	x,y;
	int	oldpx,oldpy;
	int	result;

	/***********************************************************/
	/* Ouvre la fenetre de selection et attend que l'on clique */
	/***********************************************************/

	result=-1;
	printf("Select country!\n");
/*	selNewWindowStructure1.Screen = sC;
	sel_wG = OpenWindow(&selNewWindowStructure1);*/
	if(sel_wG==NULL){
		printf("Error opening selection window\n");
	}else{
		sel_rpG = sel_wG->RPort;
		SetDrMd(sel_rpG,COMPLEMENT);

		print_func(sel_rpG);
		oldpx=8;
		oldpy=13;
		draw_rect(sel_rpG,oldpx,oldpy,100,10);
		quit_flag=FALSE;
		do{
			WaitPort(sel_wG->UserPort);

			while( (message = (struct IntuiMessage *)
				GetMsg(sel_wG->UserPort) ) != NULL)
			{
				code = message->Code;  /* MENUNUM */
				object = message->IAddress;  /* Gadget */
				class = message->Class;
				x=message->MouseX;
				y=message->MouseY;
				ReplyMsg((struct Message *)message);
				if ( class == CLOSEWINDOW ) (quit_flag = TRUE);
				if( class == MOUSEBUTTONS){
					if( code == SELECTDOWN){
						result=(((y-13)/10)%8+8*((x-8)/100))%max_sel;
						quit_flag=TRUE;
						printf("result=%d\n",result);
					}
				}
				if ( class == MOUSEMOVE ){
					x=((x-8)/100)*100+8;
					y=((y-13)/10)*10+13;
					if( ((oldpx!=x)||(oldpy!=y))&&(y>=13) ){
						SetAPen(sel_rpG,1L);
						draw_rect(sel_rpG,oldpx,oldpy,100,10);
						oldpx=x;
						oldpy=y;
						draw_rect(sel_rpG,oldpx,oldpy,100,10);
					}
				}
			}
		} while (quit_flag == FALSE);

		CloseWindow(sel_wG);
	}
	return(result);
}

/****************************************************************************/

int find_country(
	int	x,
	int	y)
{
	int i,num_country;
	COUNTRY *p_c;

	num_country=-1;
	x=((x)*(maxx-minx))/(wG->GZZWidth)+minx;
	y=((y)*(maxy-miny))/(wG->GZZHeight)+miny;
	for(i=0;(i<max_country)&&(num_country==-1);i++){
		p_c=&tab_country[i];
		if( (x<p_c->c_clip.maxx) && (x>p_c->c_clip.minx) &&
		    (y<p_c->c_clip.maxy) && (y>p_c->c_clip.miny)){
			num_country=i;
		}
	}

	return(num_country);
}

static VOID fail(APTR app,char *str)
{
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
	if (!(MUIMasterBase = OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN)))
		fail(NULL,"Failed to open "MUIMASTER_NAME".");
#endif
}
/*********************************************************** main() ****/

main(
	int	argc,
	char	**argv
	)
{
	char	*buffer;
 

	tab_vect=(BORDER *)malloc(sizeof(BORDER)*MAX_BORDER);
	tab_country=(COUNTRY *)malloc(sizeof(COUNTRY)*MAX_COUNTRY);

	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
	if (IntuitionBase == NULL)
	{
		printf("intuition is not here.  where are we?\n");
		goto cleanup1;
	}
	GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);
	ReqBase = (struct ReqBase *)OpenLibrary("req.library", 0L);

	if(ReqBase==NULL){
		printf("I Can't open req.library!\n");
		goto cleanup1;
	}
	init();			/* Mui function */

	NewScreenStructure.Height=256;
	sC = OpenScreen(&NewScreenStructure);	/* open screen if present */
	NewWindowStructure1.Screen = sC;
	if(sC==NULL)goto cleanup1;
	vP = sC->ViewPort;
	wG->Height=sC->Height;
	wG=OpenWindow(&NewWindowStructure1);

	AP_Main = ApplicationObject,
		MUIA_Application_Title         , "AmiGlobe",
		MUIA_Application_Version       , "$VER:AmiGlobe 1.0 (28.10.93)",
		MUIA_Application_Copyright     , "Copyright ©1993, T.Landspurg",
		MUIA_Application_Author        , "Thomas Landspurg",
		MUIA_Application_Description   , "AmiGlobe",
		MUIA_Application_Base          , "AmiGlobe Base",
		MUIA_Application_Menu          , Menu,

		SubWindow,
			WI_Cmd = WindowObject,
			MUIA_Window_Title, "Commandes",
			MUIA_Window_ID   , MAKE_ID('C','M','D','1'),
			MUIA_Window_Screen,	sC,
			WindowContents, VGroup,

				Child, HGroup,  MUIA_Group_SameWidth, TRUE,
					Child, BT_Cmd_Panel = KeyButton("Cmd Panel",'c'),
					Child, BT_Redraw = KeyButton("Redraw",'r'),
					Child, BT_Info = KeyButton("Info",'i'),
					Child, BT_Fill     = KeyButton("Fill"  ,'f'),
					End,
				End,
			End,


		SubWindow,
			WI_Cmd_Edit = WindowObject,
			MUIA_Window_Title, "Commandes2",
			MUIA_Window_ID   , MAKE_ID('C','M','D','2'),
			MUIA_Window_Screen,	sC,
			WindowContents, HGroup,

				Child, VGroup,  MUIA_Group_SameWidth, TRUE,
					Child, BT_Move       = KeyButton("Move Pt",'m'),
					Child, BT_Zoom       = KeyButton("Zomm",'+'),
					Child, BT_Zoom_Out   = KeyButton("Zoom out",'-'),
					Child, BT_Show_Pt    = KeyButton("Show Pt"  ,'s'),
					Child, BT_Edit       = KeyButton("Edit"  ,'e'),
					End,

				End,
			End,


		End;
	if (!AP_Main) fail(AP_Main,"Failed to create application.");

	/* On indique qu'une fermeture de la fenetre Cmd principale= fermeture	*/
	/* de l'appli								*/
	DoMethod(	WI_Cmd		,MUIM_Notify,
			MUIA_Window_CloseRequest,TRUE,
			AP_Main		,2,
			MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);

	/* Fermeture de cmd panel				*/
	DoMethod(	WI_Cmd_Edit	,MUIM_Notify,
			MUIA_Window_CloseRequest,TRUE,
			WI_Cmd_Edit	,3,
			MUIM_Set,MUIA_Window_Open,FALSE);

	DoMethod(	WI_Cmd_Edit	,MUIM_Notify,
			MUIA_Window_CloseRequest,TRUE,
			AP_Main 	,2,
			MUIM_Application_ReturnID,ID_MODE_NOT_EDIT
			);

	DoMethod(	BT_Cmd_Panel	,MUIM_Notify,
			MUIA_Pressed	,FALSE,
			WI_Cmd_Edit 	,3,
			MUIM_Set	,MUIA_Window_Open,TRUE);

	DoMethod(	BT_Cmd_Panel	,MUIM_Notify,
			MUIA_Pressed	,FALSE,
			AP_Main 	,2,
			MUIM_Application_ReturnID,ID_MODE_EDIT
			);

	DoMethod(	BT_Info   	,MUIM_Notify,
			MUIA_Pressed	,FALSE,
			AP_Main		,2,
			MUIM_Application_ReturnID,ID_INFO);

	DoMethod(	BT_Redraw	,MUIM_Notify,
			MUIA_Pressed	,FALSE,
			AP_Main		,2,
			MUIM_Application_ReturnID,ID_REDRAW);

	DoMethod(	BT_Fill   	,MUIM_Notify,
			MUIA_Pressed	,FALSE,
			AP_Main		,2,
			MUIM_Application_ReturnID,ID_FILL);

	DoMethod(	BT_Move   	,MUIM_Notify,
			MUIA_Pressed	,FALSE,
			AP_Main		,2,
			MUIM_Application_ReturnID,ID_MOVE_PT);

	DoMethod(	BT_Zoom   	,MUIM_Notify,
			MUIA_Pressed	,FALSE,
			AP_Main		,2,
			MUIM_Application_ReturnID,ID_ZOOM);

	DoMethod(	BT_Zoom_Out	,MUIM_Notify,
			MUIA_Pressed	,FALSE,
			AP_Main		,2,
			MUIM_Application_ReturnID,ID_ZOOM_OUT);

	DoMethod(	BT_Show_Pt	,MUIM_Notify,
			MUIA_Pressed	,FALSE,
			AP_Main		,2,
			MUIM_Application_ReturnID,ID_SHOW_PT);

	DoMethod(	BT_Edit   	,MUIM_Notify,
			MUIA_Pressed	,FALSE,
			AP_Main		,2,
			MUIM_Application_ReturnID,ID_EDIT);

	set(WI_Cmd,MUIA_Window_Open,TRUE);

/*
	LoadRGB4(&vP,&Palette,PaletteColorCount);
*/
	rpG = wG->RPort;
	/* Window 2 */

	buffer=(char *)AllocRaster(MAXX,MAXY);
	if(buffer==NULL){
		printf("Error allocating buffer!\n");
		goto cleanup1;
	}

	InitArea(&myareainfo,areabuffer,MAX_AREA_PT);
	rpG->AreaInfo=&myareainfo;
	rpG->TmpRas=(struct TmpRas *)InitTmpRas(&tmpras,buffer,RASSIZE(MAXX,MAXY));

/*
	SetMenuStrip(wG,&MenuList1);
*/
	if(argc<2){
		read_map("tst_map");
	}else{
		read_map(argv[1]);
	}
	printf("draw map ok!\n");

	signal_wG=(1<<wG->UserPort->mp_SigBit);
/*	signal_cmd_wG=(1<<cmd_wG->UserPort->mp_SigBit);
	signal_all=signal_wG | signal_cmd_wG;
*/
	main_loop();
	
cleanup3:
/*
	ClearMenuStrip(wG);
*/
cleanup2:
/*
	if(cmd_wG!=0)CloseWindow(cmd_wG);
*/
cleanup1:
	if(wG!=0)CloseWindow(wG);
	if(AP_Main)MUI_DisposeObject(AP_Main);
	if(sC!=0)CloseScreen(sC);
	if(buffer!=NULL)FreeRaster(buffer,MAXX,MAXY);
	if (GfxBase != NULL) CloseLibrary((struct Library *)GfxBase);
	if (IntuitionBase != NULL) CloseLibrary((struct Library *)IntuitionBase);
	if (ReqBase != NULL) CloseLibrary((struct Library *)ReqBase);
	if (MUIMasterBase)
		CloseLibrary(MUIMasterBase);

	return(0);

}

/****************************************************************************/

void main_loop(void)
{
	UWORD code;
	ULONG class;
	APTR object;
	int	x,y;
	ULONG signal_rec;
	int	new_pays,old_pays;
	char	buffer[100];

	struct IntuiMessage *message;	/* the message the IDCMP sends us */
	struct	IntuiMessage copy_msg;

	old_pays=-1;
	{
		ULONG signal=0;
		BOOL running = TRUE;
		char *buf;

		while (running)
		{
		    do{
			switch (DoMethod(AP_Main,MUIM_Application_Input,&signal))
			{
				case MUIV_Application_ReturnID_Quit:
					running = FALSE;
					break;

				case ID_ABOUT:
					MUI_Request(AP_Main, WI_Cmd, 0, NULL, "OK", "AmiGlobe");
					break;
				case ID_REDRAW:
					cmd_redraw();
					break;
				case ID_INFO:
					cmd_info();
					break;
				case ID_FILL:
					cmd_fill();
					break;
				case ID_MOVE_PT:
					cmd_move_pt();
					break;
				case ID_SHOW_PT:
					cmd_show_pt();
					break;
				case ID_ZOOM:
					cmd_zoom();
					break;
				case ID_ZOOM_OUT:
					cmd_zoom_out();
					break;
				case ID_EDIT:
					cmd_edit();
					break;
				case ID_MODE_EDIT:
					flg_edit=TRUE;
					printf("Flg edit\n");
					break;
				case ID_MODE_NOT_EDIT:
					printf("Flg not edit\n");
					flg_edit=FALSE;
					break;
			}
   		       }while(signal==0);
			if(running){
				printf("signal:%lx signal wg:%lx\n",signal,signal_wG);
				signal_rec=Wait( signal | signal_wG );
				printf("Signal recu:%lx\n",signal_rec);
			}

			if(signal_rec & signal_wG){
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
				if(flg_edit==FALSE){
				switch(class){
				   case CLOSEWINDOW:
					running=FALSE;
					break;
				   case MOUSEBUTTONS:
					if(code == SELECTDOWN) {
					country_sel=find_country(x,y);
					aff_selection();
					fill_country(country_sel,10);
					}
				    case MOUSEMOVE :
				       if(flg_edit==FALSE){
					new_pays=find_country(x,y);
					if(new_pays!=old_pays){
					   if(new_pays!=-1){
						sprintf(buffer,"%10s",tab_country[new_pays].name);
					   }else{
						sprintf(buffer,"          ");
					   }
					   old_pays=new_pays;
					   IPrint(rpG,100,20,buffer);
					}
				      }
				    }
				}else{
					edit_loop(&copy_msg);
				}	
					
			   }
		    }

		}/* Fin de WHILE running */
	}
	return;

}

/****************************************************************************/

tst_end(
	FILE	*input
	)
{
	char c;
	int	code_ret;

	/* lecture du prochain caractere */
	do{
		c=getc(input);
		if(c=='#'){
			do{
				c=getc(input);
			}while((c!=0x0a)&&(c!=-1));
			c=getc(input);
		}

	}while((!isalnum(c))&&(c!='-'));
	if((!isalpha(c))||(c=='-')){
		ungetc(c,input);
		code_ret=FALSE;
	}else{
		code_ret=TRUE;
	}
	return(code_ret);
}

/****************************************************************************/

void tstpt(
	int	x,
	int	y,
	int	*pmx,
	int	*pmy,
	int	*pMX,
	int	*pMY
	)
{
	if(x<*pmx){
		*pmx=x;
	}
	if(x>*pMX){
		*pMX=x;
	}
	if(y<*pmy){
		*pmy=y;
	}
	if(y>*pMY){
		*pMY=y;
	}
}
/****************************************************************************/

int	tstclip(
	int	mx,
	int	my,
	int	MX,
	int	MY,
	CLIP *p_clip
	)
{
	int	code_ret;

	if((mx>p_clip->maxx)||(MX<p_clip->minx)||
	   (my>p_clip->maxy)||(MY<p_clip->miny)){
		code_ret=FALSE;
	}else{
		code_ret=TRUE;
	}
	return(code_ret);
}

/*********************************************************** read_map() ***/

void read_map(
	char *p_filename
	)
{
	FILE *input;
	int	x, y;
	int mx,my,MX,MY;
	struct RastPort *r;
	struct Window *w;
	
	char str[100];
	int	flg_zone,flg_country,flg_border;
	int	flg_end;
	int	num_country;
	int	num_bord;
	int	tab_bord_tempo[MAX_VECT];
	int	i,*p_alloc;
	/* Vecteur courant */
	struct vect *v;
	VECT	*V;
	int	num_vect,nb_vect;

	printf("Lecture de %s\n",p_filename);
	flg_end=FALSE;
	r = rpG;
	w = wG;
	SetRast(r,0L);
	SetAPen(r,1L);
	SetBPen(r,0L);
	if (input = fopen(p_filename,"r"))
	{

		/* Premiere chose, la taille max de notre carte...*/

		fscanf(input,"%ld%ld%ld%ld",&minx,&miny,&maxx,&maxy);
		printf("minx:%ld miny:%ld maxx:%ld maxy:%ld\n",minx,miny,maxx,maxy);

		/********************************************************/
		/* Puis la liste des bords de pays. C'est une liste	*/
		/* de points terminee par le couple -1 -1		*/
		/********************************************************/

		printf("Lecture des borders\n");
		num_bord=0;
		flg_zone=TRUE;

		do{
#ifdef	TOMDEBUG
			printf("Vecteur numero:%d\n",num_bord);
#endif
			tab_vect[num_bord].p_vect=(VECT *)malloc(MAX_VECT*sizeof(struct vect));
			v=(struct vect *)tab_vect[num_bord].p_vect;


			/* Lecture de composantes d'un bord */
			mx=100000;
			my=100000;
			MX=-100000;
			MY=-100000;

			flg_border=TRUE;
			nb_vect=0;
			do{
				fscanf(input,"%ld%ld",&x,&y);
				v->x=x;
				v->y=-y;
				tstpt(v->x,v->y,&mx,&my,&MX,&MY);
#ifdef	TOMDEBUG
				printf("x:%d y:%d  -> :%ld %ld\n",x,y,v->x,v->y);
#endif
				v++;
				nb_vect++;

				if((x==-1)&&(y==-1)){
					v->x=-1;
					v->y=-1;
#ifdef	TOMDEBUG
					printf("End of bord\n");
#endif
					flg_border=FALSE;
				}
				if(nb_vect>=MAX_PT){
					printf("Error :max number of pt exceeded...\n");
					flg_border=FALSE;
				}
			}while(flg_border==TRUE);
			tab_vect[num_bord].b_clip.minx=mx;
			tab_vect[num_bord].b_clip.miny=my;
			tab_vect[num_bord].b_clip.maxx=MX;
			tab_vect[num_bord].b_clip.maxy=MY;
/*
			printf(" Il y a eu:%d pt dans ce bord\n",nb_vect);
*/
			tab_vect[num_bord].nb_vect=nb_vect-1;

			num_bord++;

			if(tst_end(input)==TRUE){
/*
				printf("End of border def\n");
*/
				flg_zone=FALSE;
			}

		}while(flg_zone==TRUE);
		max_border=num_bord;

		num_country=0;

		do{
			/***************************************************/
			/* D'abord le nom de la zone                       */
			/* On alloue de la memoire pour sauvegarder ce nom */
			/* et on le recopie				   */
			/***************************************************/

			fscanf(input,"%d",&tab_country[num_country].continent);

			fscanf(input,"%s",str);
#ifdef	TOMDEBUG
			printf("Country name:%s\n",str);
#endif
			tab_country[num_country].name=(char *)malloc(strlen(str)+1);
			strcpy(tab_country[num_country].name,str);

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
			do{
				fscanf(input,"%d",&num_vect);
/*
				printf("Vecteur num:%d\n",num_vect);
*/
	
				tab_bord_tempo[num_bord]=num_vect;

	
				V=tab_vect[num_vect].p_vect;
				MyMove(r,V[0].x,V[0].y);
				tstpt(V[0].x,V[0].y,&mx,&my,&MX,&MY);
				for(i=1;i<tab_vect[num_vect].nb_vect;i++){
					MyDraw(r,V[i].x,V[i].y);
					tstpt(V[i].x,V[i].y,&mx,&my,&MX,&MY);
				}
				num_bord++;
				if(num_bord>=MAX_VECT){
					printf("ERROR:Max number of border excedded\n");
					flg_end=TRUE;
				}

			}while(tst_end(input)==FALSE);
			tab_country[num_country].c_clip.minx=mx;
			tab_country[num_country].c_clip.miny=my;
			tab_country[num_country].c_clip.maxx=MX;
			tab_country[num_country].c_clip.maxy=MY;
			MyMove(r,mx,my);
			MyDraw(r,MX,my);
			MyDraw(r,MX,MY);
			MyDraw(r,mx,MY);
			MyDraw(r,mx,my);

			/****************************************/
			/* On sait le nombre de bord de ce pays	*/
			/* on leur alloue de la memoire et on le*/
			/* recopie				*/
			/****************************************/

/*
			printf("Il y a eu:%d bord\n",num_bord);
*/
			tab_country[num_country].nb_borders=num_bord;
			p_alloc=(int *)malloc((10+num_bord)*sizeof(int));
			tab_country[num_country].p_borders=p_alloc;
			for(i=0;i<num_bord;i++){
				*p_alloc=tab_bord_tempo[i];
/*
				printf("%d %d\n",i,tab_bord_tempo[i]);
*/
				p_alloc++;
			}
			fscanf(input,"%d",&(tab_country[num_country].habitants));

			fscanf(input,"%d",&(tab_country[num_country].pnb1));
			fscanf(input,"%d",&(tab_country[num_country].pnb2));
			fscanf(input,"%d",&(tab_country[num_country].pnb3));

			num_country++;
			if(num_country>=MAX_COUNTRY){
				printf("ERROR:Max number of country excedded\n");
				flg_end=TRUE;
			}
		}while((tst_end(input)==FALSE)&&(flg_end==FALSE));
		printf("Il y a eu:%d pays et %d bords\n",num_country,max_border);
		fclose(input);
	}
	max_country=num_country;

}
/***************************************************** reset_bord() *****/

/************************************************/
/* Reinitialise les drapequ indiquant qu'un	*/
/* bord a ete trace				*/
/************************************************/

void reset_bord(void)
{
	int	i;
	for(i=0;i<max_border;i++){
		tab_vect[i].flg_draw=FALSE;
	}
}

/***************************************************** redraw_map() *****/

void redraw_map(
	int	num_continent
	)
{
	struct RastPort *r;
	struct Window *w;
	int	num_country;
	int	i,j;
	/* Vecteur courant */
	int	num_vect;
	COUNTRY	*p_count;
	VECT	*V;

	r = rpG;
	w = wG;
	SetRast(r,0L);
	SetAPen(r,1L);
	SetBPen(r,0L);
	SetDrMd(r,JAM1);

	reset_bord();
	/********************************************/
	/* Pour tout les pays de la base de donnees */
	/********************************************/

	for(num_country=0;num_country<max_country;num_country++){

	  if((tab_country[num_country].continent==num_continent)||
	      (num_continent==-1)){

	    /****************************************************/
	    /* Pour un pays, on connait le nom, et le nombre de */
	    /* bords					    */
	    /****************************************************/

	    p_count=&tab_country[num_country];
	    if(tstclip(minx,miny,maxx,maxy,&(p_count->c_clip))){

		for(i=0;i<p_count->nb_borders;i++){
		
			/*********************************************/
			/* Pour chaque bord, on affiche les vecteurs */
			/* correspondants a ce bord		     */
			/*********************************************/
			num_vect=(p_count->p_borders)[i];
			if(tstclip(minx,miny,maxx,maxy,&(tab_vect[num_vect].b_clip))){
			   if(tab_vect[num_vect].flg_draw==FALSE){
				tab_vect[num_vect].flg_draw=TRUE;
				V=tab_vect[num_vect].p_vect;
				MyMove(r,V[0].x,V[0].y);
				for(j=1;j<tab_vect[num_vect].nb_vect;j++){
					MyDraw(r,V[j].x,V[j].y);
				}
			    }
			}

		}/* fin du for sur les borders	*/
	     }
	   }
	}/* Fin du for sur les country 		*/

}

/***************************************************** fill_country() *****/

void fill_country(
	int	num_country,
	int	color
	)
{
	struct RastPort *r;
	struct Window *w;
	long	lastx,lasty;	
	int	i,j;
	/* Vecteur courant */
	VECT	*V;
	int	num_vect;
	COUNTRY	*p_count;

	r = rpG;
	w = wG;
	SetAPen(r,(long)color);
	SetBPen(r,3L);
	SetOPen(r,1L);
	SetDrMd(r,JAM1);


	/****************************************************/
	/* Pour un pays, on connait le nom, et le nombre de */
	/* bords					    */
	/****************************************************/

	p_count=&tab_country[num_country];

	for(i=0;i<p_count->nb_borders;i++){
		
		/*********************************************/
		/* Pour chaque bord, on affiche les vecteurs */
		/* correspondants a ce bord		     */
		/*********************************************/
		num_vect=(p_count->p_borders)[i];
		V=tab_vect[num_vect].p_vect;
		if(i==0){
			AreaMove(r,conv_x(V[0].x),conv_y(V[0].y));
			lastx=V[0].x;
			lasty=V[0].y;
		}
		if((lastx==V[0].x)&&(lasty==V[0].y)){
			for(j=1;j<tab_vect[num_vect].nb_vect;j++){
				AreaDraw(r,conv_x(V[j].x),conv_y(V[j].y));
			}
			lastx=V[j-1].x;
			lasty=V[j-1].y;
		}else{
			j=(tab_vect[num_vect].nb_vect)-1;
			do{
				j--;
				AreaDraw(r,conv_x(V[j].x),conv_y(V[j].y));
			}while(j!=0);
			lastx=V[0].x;
			lasty=V[0].y;
		}

	}/* fin du for sur les borders	*/
	AreaEnd(r);

}
