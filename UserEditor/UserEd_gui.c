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
#include "usered_gui.h"

struct Screen         *Scr = NULL;
APTR                   VisualInfo = NULL;
struct Window         *Project0Wnd = NULL;
struct Gadget         *Project0GList = NULL;
struct Gadget         *Project0Gadgets[30];
UWORD                  Project0Left = 11;
UWORD                  Project0Top = 12;
UWORD                  Project0Width = 610;
UWORD                  Project0Height = 213;
UBYTE                 *Project0Wdt = (UBYTE *)"System-X UserEditor 1.05";
struct TextAttr       *Font, Attr;
UWORD                  FontX, FontY;
UWORD                  OffX, OffY;

UWORD Project0GTypes[] = {
	STRING_KIND,
	STRING_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	STRING_KIND,
	STRING_KIND,
	STRING_KIND,
	STRING_KIND,
	INTEGER_KIND,
	INTEGER_KIND,
	INTEGER_KIND,
	STRING_KIND,
	INTEGER_KIND,
	INTEGER_KIND,
	INTEGER_KIND,
	INTEGER_KIND,
	TEXT_KIND,
	TEXT_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	BUTTON_KIND,
	INTEGER_KIND,
	INTEGER_KIND,
	INTEGER_KIND,
	INTEGER_KIND,
	BUTTON_KIND,
	BUTTON_KIND
};

struct NewGadget Project0NGad[] = {
	92, 4, 228, 14, (UBYTE *)"_Realname:", NULL, GD_REAL, PLACETEXT_LEFT, NULL, NULL,
	387, 4, 207, 14, (UBYTE *)"_Alias:", NULL, GD_ALIAS, PLACETEXT_LEFT, NULL, NULL,
	12, 191, 193, 15, (UBYTE *)"_Save Current Account", NULL, GD_SAVE, PLACETEXT_IN, NULL, NULL,
	247, 191, 113, 15, (UBYTE *)"S_earch", NULL, GD_SEARCH, PLACETEXT_IN, NULL, NULL,
	403, 191, 193, 15, (UBYTE *)"_Quit UserED", NULL, GD_QUIT, PLACETEXT_IN, NULL, NULL,
	92, 20, 228, 14, (UBYTE *)"_Location:", NULL, GD_LOC, PLACETEXT_LEFT, NULL, NULL,
	387, 20, 207, 14, (UBYTE *)"_Phone:", NULL, GD_PHONE, PLACETEXT_LEFT, NULL, NULL,
	411, 36, 183, 14, (UBYTE *)"_Computer:", NULL, GD_COMP, PLACETEXT_LEFT, NULL, NULL,
	123, 36, 197, 14, (UBYTE *)"S_ent-By-Line:", NULL, GD_SBL, PLACETEXT_LEFT, NULL, NULL,
	140, 52, 180, 14, (UBYTE *)"Se_curity Level:", NULL, GD_SEC, PLACETEXT_LEFT, NULL, NULL,
	428, 68, 166, 14, (UBYTE *)"_Time Limit:", NULL, GD_TIMELIMIT, PLACETEXT_LEFT, NULL, NULL,
	148, 68, 172, 14, (UBYTE *)"T_ime Used Today:", NULL, GD_TIMEUSED, PLACETEXT_LEFT, NULL, NULL,
	413, 52, 181, 14, (UBYTE *)"_Protocol:", NULL, GD_PROTO, PLACETEXT_LEFT, NULL, NULL,
	155, 84, 165, 14, (UBYTE *)"_Bytes Downloaded:", NULL, GD_BYTESDOWN, PLACETEXT_LEFT, NULL, NULL,
	461, 84, 133, 14, (UBYTE *)"B_ytes Uploaded:", NULL, GD_BYTESUP, PLACETEXT_LEFT, NULL, NULL,
	155, 100, 165, 14, (UBYTE *)"_Files Downloaded:", NULL, GD_FILESDOWN, PLACETEXT_LEFT, NULL, NULL,
	461, 100, 133, 14, (UBYTE *)"F_iles Uploaded:", NULL, GD_FILESUP, PLACETEXT_LEFT, NULL, NULL,
	147, 148, 173, 14, (UBYTE *)"First Call Date:", NULL, GD_FIRSTCALL, PLACETEXT_LEFT, NULL, NULL,
	421, 148, 172, 14, (UBYTE *)"Last Call:", NULL, GD_LASTCALL, PLACETEXT_LEFT, NULL, NULL,
	207, 191, 38, 15, (UBYTE *)"< <", NULL, GD_PREV, PLACETEXT_IN, NULL, NULL,
	363, 191, 38, 15, (UBYTE *)"> >", NULL, GD_NEXT, PLACETEXT_IN, NULL, NULL,
	279, 169, 142, 15, (UBYTE *)"C_hange Password", NULL, GD_PASS, PLACETEXT_IN, NULL, NULL,
	135, 169, 142, 15, (UBYTE *)"E_dit Conf Access", NULL, GD_CONFACCESS, PLACETEXT_IN, NULL, NULL,
	12, 169, 121, 15, (UBYTE *)"De_fault Sets", NULL, GD_SETS, PLACETEXT_IN, NULL, NULL,
	107, 132, 213, 14, (UBYTE *)"F_ree Bytes:", NULL, GD_FREEBYTES, PLACETEXT_LEFT, NULL, NULL,
	428, 132, 165, 14, (UBYTE *)"Fr_ee Files:", NULL, GD_FREEFILES, PLACETEXT_LEFT, NULL, NULL,
	429, 116, 164, 14, (UBYTE *)"File _Ratio:", NULL, GD_FILERATIO, PLACETEXT_LEFT, NULL, NULL,
	107, 116, 213, 14, (UBYTE *)"Byte _Ratio:", NULL, GD_BYTERATIO, PLACETEXT_LEFT, NULL, NULL,
	423, 169, 121, 15, (UBYTE *)"_New User Scan", NULL, GD_NEWSCAN, PLACETEXT_IN, NULL, NULL,
	546, 169, 50, 15, (UBYTE *)"Del", NULL, GD_DEL, PLACETEXT_IN, NULL, NULL
};

ULONG Project0GTags[] = {
	(GTST_MaxChars), 30, (GT_Underscore), '_', (TAG_DONE),
	(GTST_MaxChars), 30, (GT_Underscore), '_', (TAG_DONE),
	(GT_Underscore), '_', (TAG_DONE),
	(GT_Underscore), '_', (TAG_DONE),
	(GT_Underscore), '_', (TAG_DONE),
	(GTST_MaxChars), 29, (GT_Underscore), '_', (TAG_DONE),
	(GTST_MaxChars), 12, (GT_Underscore), '_', (TAG_DONE),
	(GTST_MaxChars), 23, (GT_Underscore), '_', (TAG_DONE),
	(GTST_MaxChars), 45, (GT_Underscore), '_', (TAG_DONE),
	(GTIN_Number), 0, (GTIN_MaxChars), 3, (GT_Underscore), '_', (TAG_DONE),
	(GTIN_Number), 0, (GTIN_MaxChars), 9, (GT_Underscore), '_', (TAG_DONE),
	(GTIN_Number), 0, (GTIN_MaxChars), 9, (GT_Underscore), '_', (TAG_DONE),
	(GTST_MaxChars), 1, (GT_Underscore), '_', (TAG_DONE),
	(GTIN_Number), 0, (GTIN_MaxChars), 16, (GT_Underscore), '_', (TAG_DONE),
	(GTIN_Number), 0, (GTIN_MaxChars), 16, (GT_Underscore), '_', (TAG_DONE),
	(GTIN_Number), 0, (GTIN_MaxChars), 16, (GT_Underscore), '_', (TAG_DONE),
	(GTIN_Number), 0, (GTIN_MaxChars), 16, (GT_Underscore), '_', (TAG_DONE),
	(GTTX_Border), TRUE, (TAG_DONE),
	(GTTX_Border), TRUE, (TAG_DONE),
	(TAG_DONE),
	(TAG_DONE),
	(GT_Underscore), '_', (TAG_DONE),
	(GT_Underscore), '_', (TAG_DONE),
	(GT_Underscore), '_', (TAG_DONE),
	(GTIN_Number), 0, (GTIN_MaxChars), 16, (GT_Underscore), '_', (TAG_DONE),
	(GTIN_Number), 0, (GTIN_MaxChars), 16, (GT_Underscore), '_', (TAG_DONE),
	(GTIN_Number), 0, (GTIN_MaxChars), 9, (GT_Underscore), '_', (TAG_DONE),
	(GTIN_Number), 0, (GTIN_MaxChars), 9, (GT_Underscore), '_', (TAG_DONE),
	(GT_Underscore), '_', (TAG_DONE),
	(TAG_DONE)
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

int SetupScreen( void )
{
	if ( ! ( Scr = LockPubScreen( 0L )))
		return( 1L );

	ComputeFont( 0, 0 );

	if ( ! ( VisualInfo = GetVisualInfo( Scr, TAG_DONE )))
		return( 2L );

	return( 0L );
}

void CloseDownScreen( void )
{
	if ( VisualInfo ) {
		FreeVisualInfo( VisualInfo );
		VisualInfo = NULL;
	}

	if ( Scr        ) {
		UnlockPubScreen( NULL, Scr );
		Scr = NULL;
	}
}

void Project0Render( void )
{
	ComputeFont( Project0Width, Project0Height );

	DrawBevelBox( Project0Wnd->RPort, OffX + ComputeX( 9 ),
					OffY + ComputeY( 167 ),
					ComputeX( 591 ),
					ComputeY( 19 ),
					GT_VisualInfo, VisualInfo, GTBB_Recessed, TRUE, TAG_DONE );
	DrawBevelBox( Project0Wnd->RPort, OffX + ComputeX( 8 ),
					OffY + ComputeY( 189 ),
					ComputeX( 592 ),
					ComputeY( 19 ),
					GT_VisualInfo, VisualInfo, GTBB_Recessed, TRUE, TAG_DONE );
	DrawBevelBox( Project0Wnd->RPort, OffX + ComputeX( 3 ),
					OffY + ComputeY( 1 ),
					ComputeX( 603 ),
					ComputeY( 210 ),
					GT_VisualInfo, VisualInfo, TAG_DONE );
}

int OpenProject0Window( void )
{
	struct NewGadget	ng;
	struct Gadget	*g;
	UWORD		lc, tc;
	UWORD		wleft = Project0Left, wtop = Project0Top, ww, wh;

	ComputeFont( Project0Width, Project0Height );

	ww = ComputeX( Project0Width );
	wh = ComputeY( Project0Height );

	if (( wleft + ww + OffX + Scr->WBorRight ) > Scr->Width ) wleft = Scr->Width - ww;
	if (( wtop + wh + OffY + Scr->WBorBottom ) > Scr->Height ) wtop = Scr->Height - wh;

	if ( ! ( g = CreateContext( &Project0GList )))
		return( 1L );

	for( lc = 0, tc = 0; lc < Project0_CNT; lc++ ) {

		CopyMem((char * )&Project0NGad[ lc ], (char * )&ng, (long)sizeof( struct NewGadget ));

		ng.ng_VisualInfo = VisualInfo;
		ng.ng_TextAttr   = Font;
		ng.ng_LeftEdge   = OffX + ComputeX( ng.ng_LeftEdge );
		ng.ng_TopEdge    = OffY + ComputeY( ng.ng_TopEdge );
		ng.ng_Width      = ComputeX( ng.ng_Width );
		ng.ng_Height     = ComputeY( ng.ng_Height);

		Project0Gadgets[ lc ] = g = CreateGadgetA((ULONG)Project0GTypes[ lc ], g, &ng, ( struct TagItem * )&Project0GTags[ tc ] );

		while( Project0GTags[ tc ] ) tc += 2;
		tc++;

		if ( NOT g )
			return( 2L );
	}

	if ( ! ( Project0Wnd = OpenWindowTags( NULL,
				WA_Left,	wleft,
				WA_Top,		wtop,
				WA_Width,	ww + OffX + Scr->WBorRight,
				WA_Height,	wh + OffY + Scr->WBorBottom,
				WA_IDCMP,	STRINGIDCMP|BUTTONIDCMP|INTEGERIDCMP|TEXTIDCMP|IDCMP_CLOSEWINDOW|IDCMP_REFRESHWINDOW|IDCMP_RAWKEY,
				WA_Flags,	WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_SMART_REFRESH,
				WA_Gadgets,	Project0GList,
				WA_Title,	Project0Wdt,
				WA_Activate,	TRUE,
				TAG_DONE )))
	return( 4L );

	GT_RefreshWindow( Project0Wnd, NULL );

	Project0Render();

	return( 0L );
}

void CloseProject0Window( void )
{
	if ( Project0Wnd        ) {
		CloseWindow( Project0Wnd );
		Project0Wnd = NULL;
	}

	if ( Project0GList      ) {
		FreeGadgets( Project0GList );
		Project0GList = NULL;
	}
}