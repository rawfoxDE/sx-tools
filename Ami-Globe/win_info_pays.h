
UBYTE infpinfpnaSIBuff[20];
struct StringInfo infpinfpnaSInfo = {
	infpinfpnaSIBuff,	/* buffer where text will be edited */
	NULL,	/* optional undo buffer */
	0,	/* character position in buffer */
	20,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

SHORT infpBorderVectors1[] = {
	0,0,
	132,0,
	132,9,
	0,9,
	0,0
};
struct Border infpBorder1 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	infpBorderVectors1,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText infpIText1 = {
	1,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	-44,0,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Name:",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget infpna = {
	NULL,	/* next gadget */
	81,18,	/* origin XY of hit box relative to window TopLeft */
	131,8,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	(APTR)&infpBorder1,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&infpIText1,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&infpinfpnaSInfo,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

SHORT infpBorderVectors2[] = {
	0,0,
	55,0,
	55,16,
	0,16,
	0,0
};
struct Border infpBorder2 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	infpBorderVectors2,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText infpIText2 = {
	3,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	10,3,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Add",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget infps3 = {
	&infpna,	/* next gadget */
	167,84,	/* origin XY of hit box relative to window TopLeft */
	54,15,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&infpBorder2,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&infpIText2,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

SHORT infpBorderVectors3[] = {
	0,0,
	55,0,
	55,16,
	0,16,
	0,0
};
struct Border infpBorder3 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	infpBorderVectors3,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText infpIText3 = {
	3,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	11,3,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Del",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget infps2 = {
	&infps3,	/* next gadget */
	167,66,	/* origin XY of hit box relative to window TopLeft */
	54,15,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&infpBorder3,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&infpIText3,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

SHORT infpBorderVectors4[] = {
	0,0,
	55,0,
	55,16,
	0,16,
	0,0
};
struct Border infpBorder4 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	infpBorderVectors4,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText infpIText4 = {
	3,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	6,3,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Show",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget infps1 = {
	&infps2,	/* next gadget */
	167,48,	/* origin XY of hit box relative to window TopLeft */
	54,15,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&infpBorder4,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&infpIText4,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

UBYTE infpinfpbord_numSIBuff[4];
struct StringInfo infpinfpbord_numSInfo = {
	infpinfpbord_numSIBuff,	/* buffer where text will be edited */
	NULL,	/* optional undo buffer */
	0,	/* character position in buffer */
	4,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

SHORT infpBorderVectors5[] = {
	0,0,
	68,0,
	68,9,
	0,9,
	0,0
};
struct Border infpBorder5 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	infpBorderVectors5,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct Gadget infpbord_num = {
	&infps1,	/* next gadget */
	158,36,	/* origin XY of hit box relative to window TopLeft */
	67,8,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	(APTR)&infpBorder5,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	NULL,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&infpinfpbord_numSInfo,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

SHORT infpBorderVectors6[] = {
	0,0,
	33,0,
	33,12,
	0,12,
	0,0
};
struct Border infpBorder6 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	infpBorderVectors6,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct Gadget infpd2 = {
	&infpbord_num,	/* next gadget */
	128,77,	/* origin XY of hit box relative to window TopLeft */
	32,11,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&infpBorder6,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	NULL,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

SHORT infpBorderVectors7[] = {
	0,0,
	33,0,
	33,12,
	0,12,
	0,0
};
struct Border infpBorder7 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	infpBorderVectors7,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct Gadget infpu1 = {
	&infpd2,	/* next gadget */
	129,48,	/* origin XY of hit box relative to window TopLeft */
	32,11,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&infpBorder7,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	NULL,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

SHORT infpBorderVectors8[] = {
	0,0,
	77,0,
	77,16,
	0,16,
	0,0
};
struct Border infpBorder8 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	infpBorderVectors8,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText infpIText5 = {
	1,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	20,4,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Quit",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget infpok1 = {
	&infpu1,	/* next gadget */
	102,108,	/* origin XY of hit box relative to window TopLeft */
	76,15,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&infpBorder8,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&infpIText5,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

#define infpGadgetList1 infpok1

struct NewWindow infpNewWindowStructure1 = {
	12,33,	/* window XY origin relative to TopLeft of screen */
	258,128,	/* window width and height */
	0,1,	/* detail and block pens */
	GADGETDOWN+GADGETUP+CLOSEWINDOW,	/* IDCMP flags */
	WINDOWSIZING+WINDOWDRAG+WINDOWDEPTH+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH,	/* other window flags */
	&infpok1,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	"Info pays",	/* window title */
	NULL,	/* custom screen pointer */
	NULL,	/* custom bitmap */
	5,5,	/* minimum width and height */
	65535,65535,	/* maximum width and height */
	CUSTOMSCREEN	/* destination screen type */
};


void HandleEvent3(APTR object)
{
  if (object == (APTR)&infpok1) { cmd_ok(); return; }
  if (object == (APTR)&infpu1) { cmd_up(); return; }
  if (object == (APTR)&infpd2) { cmd_down(); return; }
  if (object == (APTR)&infps1) { cmd_show_bord(); return; }
  if (object == (APTR)&infps2) { cmd_del_bord(); return; }
  if (object == (APTR)&infps3) { cmd_add_bord(); return; }
  if (object == (APTR)&infpna) { cmd_newname(); return; }
}
#define HANDLEEVENT HandleEvent

/* end of PowerWindows source generation */
