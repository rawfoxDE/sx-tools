#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <graphics/gfx.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#include "amiglobe_types.h"
#include "flag_protos.h"

WORD ColorMap[] =
{
	0x0000,0x0fff,0x0f00,0x00f0,0x000f,0x0444,0x0555,0x0655,
	0x0733,0x0E00,0x0A90,0x0888,0x0050,0x0DDD,0x0FE0,0x0FFF
};

struct	TmpRas	tmpras;		/* Pour les areadraw */
struct	AreaInfo myareainfo;

#define WIDTH	640
#define HEIGHT	480

#define	BUFFER_SIZE	80

int	*P_Tab_Drapeau;

WORD	areabuffer[MAX_AREA_PT];

main(int argc,char **argv)
{

	struct	Screen	*s;
	struct	Window	*win;
	struct	TmpRas	tmpras;
	struct	RastPort	*rp;
	int	oldx,oldy;
	int	Flg_Exit;
	CLIP	My_Clip;
	char	*p_name;

	if(argc<=1){
		p_name="test_token";
	}else{
		p_name=argv[1];
	}
	if(Flag_Read(p_name,&P_Tab_Drapeau)){
		My_Clip.minx=10;
		My_Clip.maxx=300;
		My_Clip.miny=10;
		My_Clip.maxy=250;
	
		s = OpenScreenTags (NULL,
		  	SA_Depth, (ULONG) 5,
		  	SA_ShowTitle, FALSE,
		  	SA_Quiet, TRUE,
		  	TAG_DONE);
		if(s){
			LoadRGB4(&s->ViewPort,ColorMap,32);
			win=OpenWindowTags(
				NULL,
				WA_Left,	0,
				WA_Top,		0,
				WA_Width,	s->Width,
				WA_Height,	s->Height,
				WA_CustomScreen,s,
				WA_IDCMP,	IDCMP_CLOSEWINDOW|IDCMP_MOUSEMOVE,
				WA_Flags,	WFLG_SIMPLE_REFRESH|WFLG_CLOSEGADGET|WFLG_ACTIVATE|WFLG_REPORTMOUSE,
				TAG_DONE);

			if(win){
				rp=win->RPort;
				InitArea(&myareainfo,areabuffer,MAX_AREA_PT);
				rp->AreaInfo=&myareainfo;
				rp->TmpRas=(struct TmpRas *)InitTmpRas(&tmpras,AllocRaster(WIDTH,HEIGHT),RASSIZE(WIDTH,HEIGHT));
	
	
				oldx=100;
				oldy=100;
				Flg_Exit=FALSE;
	
				Flag_Draw(rp,&My_Clip,P_Tab_Drapeau,s->ViewPort.ColorMap);
	
				while(Flg_Exit==FALSE){
					struct	IntuiMessage *msg;
					int	class,code;
		
					WaitPort(win->UserPort);
					while((msg=(struct IntuiMessage *)GetMsg(win->UserPort))!=0){
						class=msg->Class;
						code=msg->Code;
						ReplyMsg((struct Message *)msg);
						switch(class){
							case CLOSEWINDOW:
								Flg_Exit=TRUE;
								break;
							case MOUSEMOVE:
								break;
						}
					}
				}
	
	
				FreeRaster(tmpras.RasPtr,WIDTH,HEIGHT);
				CloseWindow(win);
			}
			CloseScreen(s);
		}
		free(P_Tab_Drapeau);
	}
}
