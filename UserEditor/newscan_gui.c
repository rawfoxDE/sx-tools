/*
 *  Source machine generated by GadToolsBox V2.0b
 *  which is (c) Copyright 1991-1993 Jaba Development
 *
 *  GUI Designed by : -- Unnamed --
 */

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <libraries/gadtools.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/utility_protos.h>
#include <string.h>
#include "newscan_gui.h"

struct Window         *Project1Wnd = NULL;
struct Gadget         *Project1GList = NULL;
struct Gadget         *Project1Gadgets[2];
UWORD                  Project1Left = 162;
UWORD                  Project1Top = 24;
UWORD                  Project1Width = 266;
UWORD                  Project1Height = 23;
UBYTE                 *Project1Wdt = (UBYTE *)"New User Scan";
extern struct TextAttr       *Font, Attr;
extern UWORD                  FontX, FontY;
extern UWORD                  OffX, OffY;

UWORD Project1GTypes[] = {
	BUTTON_KIND,
	BUTTON_KIND
};

struct NewGadget Project1NGad[] = {
	11, 5, 121, 13, (UBYTE *)"_Find Next", NULL, GD_FINDNEXT, PLACETEXT_IN, NULL, NULL,
	134, 5, 121, 13, (UBYTE *)"_Quit NewScan", NULL, GD_QUITNEWSCAN, PLACETEXT_IN, NULL, NULL
};

ULONG Project1GTags[] = {
	(GT_Underscore), '_', (TAG_DONE),
	(GT_Underscore), '_', (TAG_DONE)
};

static UWORD ComputeX( UWORD value )
{
	return(( UWORD )((( FontX * value ) + 4 ) / 8 ));
}

static UWORD ComputeY( UWORD value )
{
	return(( UWORD )((( FontY * value ) + 4 ) / 8 ));
}

static void ComputeFont( UWORD width, UWORD height )
{
	Font = &Attr;
	Font->ta_Name = (STRPTR)Scr->RastPort.Font->tf_Message.mn_Node.ln_Name;
	Font->ta_YSize = FontY = Scr->RastPort.Font->tf_YSize;
	FontX = Scr->RastPort.Font->tf_XSize;

	OffX = Scr->WBorLeft;
	OffY = Scr->RastPort.TxHeight + Scr->WBorTop + 1;

	if ( width && height ) {
		if (( ComputeX( width ) + OffX + Scr->WBorRight ) > Scr->Width )
			goto UseTopaz;
		if (( ComputeY( height ) + OffY + Scr->WBorBottom ) > Scr->Height )
			goto UseTopaz;
	}
	return;

UseTopaz:
	Font->ta_Name = (STRPTR)"topaz.font";
	FontX = FontY = Font->ta_YSize = 8;
}

void Project1Render( void )
{
	ComputeFont( Project1Width, Project1Height );

	DrawBevelBox( Project1Wnd->RPort, OffX + ComputeX( 3 ),
					OffY + ComputeY( 1 ),
					ComputeX( 260 ),
					ComputeY( 21 ),
					GT_VisualInfo, VisualInfo, TAG_DONE );
}

int OpenProject1Window( void )
{
	struct NewGadget	ng;
	struct Gadget	*g;
	UWORD		lc, tc;
	UWORD		wleft = Project1Left, wtop = Project1Top, ww, wh;

	ComputeFont( Project1Width, Project1Height );

	ww = ComputeX( Project1Width );
	wh = ComputeY( Project1Height );

	if (( wleft + ww + OffX + Scr->WBorRight ) > Scr->Width ) wleft = Scr->Width - ww;
	if (( wtop + wh + OffY + Scr->WBorBottom ) > Scr->Height ) wtop = Scr->Height - wh;

	if ( ! ( g = CreateContext( &Project1GList )))
		return( 1L );

	for( lc = 0, tc = 0; lc < Project1_CNT; lc++ ) {

		CopyMem((char * )&Project1NGad[ lc ], (char * )&ng, (long)sizeof( struct NewGadget ));

		ng.ng_VisualInfo = VisualInfo;
		ng.ng_TextAttr   = Font;
		ng.ng_LeftEdge   = OffX + ComputeX( ng.ng_LeftEdge );
		ng.ng_TopEdge    = OffY + ComputeY( ng.ng_TopEdge );
		ng.ng_Width      = ComputeX( ng.ng_Width );
		ng.ng_Height     = ComputeY( ng.ng_Height);

		Project1Gadgets[ lc ] = g = CreateGadgetA((ULONG)Project1GTypes[ lc ], g, &ng, ( struct TagItem * )&Project1GTags[ tc ] );

		while( Project1GTags[ tc ] ) tc += 2;
		tc++;

		if ( NOT g )
			return( 2L );
	}

	if ( ! ( Project1Wnd = OpenWindowTags( NULL,
				WA_Left,	wleft,
				WA_Top,		wtop,
				WA_Width,	ww + OffX + Scr->WBorRight,
				WA_Height,	wh + OffY + Scr->WBorBottom,
				WA_IDCMP,	BUTTONIDCMP|IDCMP_CLOSEWINDOW|IDCMP_REFRESHWINDOW,
				WA_Flags,	WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_SMART_REFRESH,
				WA_Gadgets,	Project1GList,
				WA_Title,	Project1Wdt,
				WA_Activate,	TRUE,
				TAG_DONE )))
	return( 4L );

	GT_RefreshWindow( Project1Wnd, NULL );

	Project1Render();

	return( 0L );
}

void CloseProject1Window( void )
{
	if ( Project1Wnd        ) {
		CloseWindow( Project1Wnd );
		Project1Wnd = NULL;
	}

	if ( Project1GList      ) {
		FreeGadgets( Project1GList );
		Project1GList = NULL;
	}
}

