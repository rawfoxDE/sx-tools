
struct NewScreen NewScreenStructure = {
	0,0,
	640,200,
	4,
	0,1,
	HIRES,
	CUSTOMSCREEN,
	NULL,
	"AmiGlobe 1.0",
	NULL,
	NULL
};

#define NEWSCREENSTRUCTURE NewScreenStructure

struct NewWindow NewWindowStructure1 = {
	0,10,
	640,200,
	0,1,
	MOUSEBUTTONS+MOUSEMOVE+GADGETDOWN+GADGETUP+MENUPICK+CLOSEWINDOW+
        VANILLAKEY+RAWKEY,
	BACKDROP+REPORTMOUSE+BORDERLESS+ACTIVATE+NOCAREREFRESH,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	5,5,
	65535,65535,
	CUSTOMSCREEN
};

/* end of PowerWindows source generation */
