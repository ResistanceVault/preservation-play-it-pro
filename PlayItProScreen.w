struct TextAttr TOPAZ80 = {
	(STRPTR)"topaz.font",
	TOPAZ_EIGHTY,0,0
};

WORD dummyarray = { -1 };

ULONG screentags[] = { SA_Pens, &dummyarray, TAG_END };

struct ExtNewScreen NewScreenStructure = {
	0,0,	/* screen XY origin relative to View */
	640,256,	/* screen width and height */
	2,	/* screen depth (number of bitplanes) */
	0,1,	/* detail and block pens */
	HIRES,	/* display modes for this screen */
	CUSTOMSCREEN+NS_EXTENDED, /* screen type */
	&TOPAZ80,	/* pointer to default screen font */
	Title,	/* screen title */
	NULL,	/* first in list of custom screen gadgets */
	NULL,	/* pointer to custom BitMap structure */
	screentags
};

#define NEWSCREENSTRUCTURE NewScreenStructure

USHORT Palette[] = {
	0x0AAA,	/* color #0 Grey  */
	0x0000,	/* color #1 Black */
	0x0FFF,	/* color #2 White */
	0x068B,	/* color #3 Blue  */
#define PaletteColorCount 4
};

#define PALETTE Palette

#define GREY	0
#define BLACK	1
#define WHITE	2
#define BLUE	3


struct NewWindow InfoWindow = {
	188,64,	/* window XY origin relative to TopLeft of screen */
	264,128,	/* window width and height */
	0,1,	/* detail and block pens */
	RAWKEY+MOUSEBUTTONS,	/* IDCMP flags */
	ACTIVATE+RMBTRAP,	/* other window flags */
	NULL,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	NULL,	/* window title */
	NULL,	/* custom screen pointer */
	NULL,	/* custom bitmap */
	5,5,	/* minimum width and height */
	0,0,	/* maximum width and height */
	CUSTOMSCREEN	/* destination screen type */
};

/* end of PowerWindows source generation */
