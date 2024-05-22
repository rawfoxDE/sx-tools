
SHORT editBorderVectors1[] = {
	0,0,
	91,0,
	91,15,
	0,15,
	0,0
};

struct Border editBorder1 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	editBorderVectors1,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText editIText1 = {
	3,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	12,3,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Edit pays",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget editm6 = {
	NULL,	/* next gadget */
	16,98,	/* origin XY of hit box relative to window TopLeft */
	90,14,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&editBorder1,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&editIText1,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

SHORT editBorderVectors2[] = {
	0,0,
	91,0,
	91,15,
	0,15,
	0,0
};
struct Border editBorder2 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	editBorderVectors2,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText editIText2 = {
	3,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	19,2,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Redraw",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget editm5 = {
	&editm6,	/* next gadget */
	16,81,	/* origin XY of hit box relative to window TopLeft */
	90,14,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&editBorder2,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&editIText2,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

SHORT editBorderVectors3[] = {
	0,0,
	91,0,
	91,15,
	0,15,
	0,0
};
struct Border editBorder3 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	editBorderVectors3,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText editIText3 = {
	3,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	19,2,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Show pt",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget editm4 = {
	&editm5,	/* next gadget */
	16,64,	/* origin XY of hit box relative to window TopLeft */
	90,14,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&editBorder3,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&editIText3,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

SHORT editBorderVectors4[] = {
	0,0,
	91,0,
	91,15,
	0,15,
	0,0
};
struct Border editBorder4 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	editBorderVectors4,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText editIText4 = {
	3,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	19,2,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Zoom out",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget editm3 = {
	&editm4,	/* next gadget */
	15,47,	/* origin XY of hit box relative to window TopLeft */
	90,14,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&editBorder4,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&editIText4,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

SHORT editBorderVectors5[] = {
	0,0,
	91,0,
	91,15,
	0,15,
	0,0
};
struct Border editBorder5 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	editBorderVectors5,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText editIText5 = {
	3,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	19,2,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"Zoom",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget editm2 = {
	&editm3,	/* next gadget */
	15,30,	/* origin XY of hit box relative to window TopLeft */
	90,14,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&editBorder5,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&editIText5,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

SHORT editBorderVectors6[] = {
	0,0,
	91,0,
	91,15,
	0,15,
	0,0
};
struct Border editBorder6 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	editBorderVectors6,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText editIText6 = {
	3,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	19,2,	/* XY origin relative to container TopLeft */
	NULL,	/* font pointer or NULL for default */
	"MovePt",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget editm1 = {
	&editm2,	/* next gadget */
	15,13,	/* origin XY of hit box relative to window TopLeft */
	90,14,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&editBorder6,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&editIText6,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

#define editGadgetList1 editm1

struct NewWindow editNewWindowStructure1 = {
	507,16,	/* window XY origin relative to TopLeft of screen */
	120,148,	/* window width and height */
	0,1,	/* detail and block pens */
	GADGETDOWN+GADGETUP+CLOSEWINDOW,	/* IDCMP flags */
	WINDOWSIZING+WINDOWDRAG+WINDOWDEPTH+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH,	/* other window flags */
	&editm1,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	"Editor",	/* window title */
	NULL,	/* custom screen pointer */
	NULL,	/* custom bitmap */
	5,5,	/* minimum width and height */
	-1,-1,	/* maximum width and height */
	CUSTOMSCREEN	/* destination screen type */
};


void HandleEvent2(object)
APTR object;
{
/*
  if (object == (APTR)&editm1) { cmd_movept(object); return; }
  if (object == (APTR)&editm2) { cmd_zoom(object); return; }
  if (object == (APTR)&editm3) { cmd_zoom_out(object); return; }
  if (object == (APTR)&editm4) { cmd_show_pt(object); return; }
  if (object == (APTR)&editm5) { cmd_redraw(object); return; }
  if (object == (APTR)&editm6) { cmd_edit_pays(object); return; }
*/
}
#define HANDLEEVENT HandleEvent

/* end of PowerWindows source generation */
