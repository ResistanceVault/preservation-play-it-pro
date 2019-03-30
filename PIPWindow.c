;/*
Delete PIPWindow.o quiet
sc5 -j73 -v -O PIPWindow.c
quit
*/

#include <JEO:JEO.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <graphics/gfxbase.h>

#include "-P:PIPWindow.h"
#include "-P:PlayItPro.printer"
#include "-P:StatusImage.h"
#include <-p:header.h>

struct Screen         *Scr = NULL;
APTR                   VisualInfo = NULL;
struct Window         *PIPWnd = NULL;
struct Menu           *PIPMenus = NULL;
UWORD                  PIPLeft = 1;
UWORD                  PIPTop = 0;
UWORD                  PIPWidth = 639;
UWORD                  PIPHeight = 244;
UBYTE                 *PIPWdt = (UBYTE *)"Work Window";

struct TextAttr topaz8 = {
	( STRPTR )"topaz.font", 8, 0x00, 0x01 };



USHORT chip ImageData[] = {
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x1000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x3000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x3000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x3000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x3000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x3000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x3000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x3000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x3000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x3000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x3000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x3000,0x7FFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xF000,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
	0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xE000,0xC000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0xC000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0xC000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0xC000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0xC000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xC000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0xC000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0xC000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xC000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0xC000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0xC000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x8000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000
};

struct Image Image1 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget T4GD = {
	NULL,
	50,231-14,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image1,
	NULL,
	NULL,
	NULL,
	NULL,
	13,
	NULL
};

struct Image Image2 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget T3GD = {
	&T4GD,
	50,216-13,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image2,
	NULL,
	NULL,
	NULL,
	NULL,
	12,
	NULL
};

struct Image Image3 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget T2GD = {
	&T3GD,
	50,201-12,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image3,
	NULL,
	NULL,
	NULL,
	NULL,
	11,
	NULL
};

struct Image Image4 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget T1GD = {
	&T2GD,
	50,186-11,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image4,
	NULL,
	NULL,
	NULL,
	NULL,
	10,
	NULL
};

struct Image Image5 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget F10GD = {
	&T1GD,
	50,171-10,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image5,
	NULL,
	NULL,
	NULL,
	NULL,
	9,
	NULL
};

struct Image Image6 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget F9GD = {
	&F10GD,
	50,156-9,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image6,
	NULL,
	NULL,
	NULL,
	NULL,
	8,
	NULL
};

struct Image Image7 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget F8GD = {
	&F9GD,
	50,141-8,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image7,
	NULL,
	NULL,
	NULL,
	NULL,
	7,
	NULL
};

struct Image Image8 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget F7GD = {
	&F8GD,
	50,126-7,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image8,
	NULL,
	NULL,
	NULL,
	NULL,
	6,
	NULL
};

struct Image Image9 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget F6GD = {
	&F7GD,
	50,111-6,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image9,
	NULL,
	NULL,
	NULL,
	NULL,
	5,
	NULL
};

struct Image Image10 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget F5GD = {
	&F6GD,
	50,96-5,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image10,
	NULL,
	NULL,
	NULL,
	NULL,
	4,
	NULL
};

struct Image Image11 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget F4GD = {
	&F5GD,
	50,81-4,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image11,
	NULL,
	NULL,
	NULL,
	NULL,
	3,
	NULL
};

struct Image Image12 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget F3GD = {
	&F4GD,
	50,66-3,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image12,
	NULL,
	NULL,
	NULL,
	NULL,
	2,
	NULL
};

struct Image Image13 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget F2GD = {
	&F3GD,
	50,51-2,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image13,
	NULL,
	NULL,
	NULL,
	NULL,
	1,
	NULL
};

struct Image Image14 = {
	0,0,
	196,13,
	2,
	ImageData,
	0x0003,0x0000,
	NULL
};

struct Gadget F1GD = {
	&F2GD,
	50,36-1,
	196,13,
	GADGIMAGE,
	RELVERIFY+TOGGLESELECT,
	BOOLGADGET,
	(APTR)&Image14,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	NULL
};

#define GadgetList1 F1GD

struct NewMenu PIPNewMenu[] = {
	NM_TITLE, (STRPTR)"Project", NULL, 0, NULL, NULL,
	NM_ITEM, (STRPTR)"About...", (STRPTR)"A", 0, 0L, NULL,					// 0
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,								// 1
	NM_ITEM, (STRPTR)"Load sample...", (STRPTR)"L", 0, 0L, NULL,		// 2
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,								// 3
	NM_ITEM, (STRPTR)"Change volume...", (STRPTR)"V", 0, 0L, NULL,	// 4
	NM_ITEM, (STRPTR)"Increse all volumes...", NULL, 0, 0L, NULL,		// 5
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,								// 6
	NM_ITEM, (STRPTR)"Enter loop...", (STRPTR)"O", 0, 0L, NULL,			// 7
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,								// 8
	NM_ITEM, (STRPTR)"Edit sample info...", (STRPTR)"I", 0, 0L, NULL,		// 9
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,								// 10
	NM_ITEM, (STRPTR)"Delete all samples", NULL, 0, 0L, NULL,				// 11
	NM_ITEM, (STRPTR)"Delete one sample", (STRPTR)"D", 0, 0L, NULL,	// 12
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,								// 13
	NM_ITEM, (STRPTR)"Delete file", NULL, 0, 0L, NULL,							// 14
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,								// 15
	NM_ITEM, (STRPTR)"Quit", (STRPTR)"Q", 0, 0L, NULL,							// 16
	NM_TITLE, (STRPTR)"Load", NULL, 0, NULL, NULL,
	NM_ITEM, (STRPTR)"Preset  1", (STRPTR)"1", 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  2", (STRPTR)"2", 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  3", (STRPTR)"3", 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  4", (STRPTR)"4", 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  5", (STRPTR)"5", 0, 0L, NULL,
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  6", (STRPTR)"6", 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  7", (STRPTR)"7", 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  8", (STRPTR)"8", 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  9", (STRPTR)"9", 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset 10", (STRPTR)"0", 0, 0L, NULL,
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset 11", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset 12", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset 13", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset 14", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset 15", NULL, 0, 0L, NULL,
	NM_TITLE, (STRPTR)"Save as", NULL, 0, NULL, NULL,
	NM_ITEM, (STRPTR)"Current preset", (STRPTR)"S", 0, 0L, NULL,
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  1", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  2", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  3", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  4", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  5", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  6", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  7", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  8", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset  9", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset 10", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)NM_BARLABEL, NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset 11", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset 12", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset 13", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset 14", NULL, 0, 0L, NULL,
	NM_ITEM, (STRPTR)"Preset 15", NULL, 0, 0L, NULL,
	NM_END, NULL, NULL, 0, 0L, NULL };

struct ColorSpec ScreenColors[] = {
	 0, 0x0A, 0x0A, 0x0A,
	 1, 0x00, 0x00, 0x00,
	 2, 0x0F, 0x0F, 0x0F,
	 3, 0x06, 0x08, 0x0B,
	~0, 0x00, 0x00, 0x00 };

UWORD DriPens[] = {
	0xFFFF };

int SetupScreen( void )
{
	if ( ! ( Scr = OpenScreenTags( NULL, SA_Left,	0,
					SA_Top,		0,
					SA_Width,	screenWidth,
					SA_Height,	screenHeight,
					SA_Depth,	2,
					SA_Colors,	&ScreenColors[0],
					SA_Font,	&topaz8,
					SA_Type,	CUSTOMSCREEN,
					SA_DisplayID,	DEFAULT_MONITOR_ID|HIRES_KEY,
					SA_AutoScroll,	FALSE,
					SA_Overscan,	OSCAN_TEXT,
					SA_Pens,	&DriPens[0],
					SA_Title,	Ver,
					TAG_DONE )))
		return( 1L );

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
		CloseScreen( Scr );
		Scr = NULL;
	}
}

int OpenPIPWindow( void )
{
	struct NewGadget	ng;
	struct Gadget	*g;
	UWORD		lc, tc;
	UWORD		offx = Scr->WBorLeft, offy = Scr->WBorTop + Scr->RastPort.TxHeight + 1;

	if ( ! ( PIPMenus = CreateMenus( PIPNewMenu, GTMN_FrontPen, 1L, TAG_DONE )))
		return( 3L );
	LayoutMenus( PIPMenus, VisualInfo, GTMN_TextAttr, &topaz8, TAG_DONE );


	if (video_mode == NTSC)
	  T1GD.NextGadget = 0;	// Da stopper vi her ;)

	if ( ! ( PIPWnd = OpenWindowTags( NULL,
				WA_Left,	PIPLeft,
				WA_Top,		PIPTop,
				WA_Width,	screenWidth,
				WA_Height,	screenHeight + offy,
				WA_IDCMP,	IDCMP_MOUSEBUTTONS+IDCMP_GADGETUP+IDCMP_MENUPICK+IDCMP_RAWKEY,
				WA_Flags,	WFLG_NEWLOOKMENUS|WFLG_SMART_REFRESH|WFLG_BACKDROP|WFLG_BORDERLESS|WFLG_ACTIVATE,
				WA_Gadgets,	&F1GD,
				WA_CustomScreen,	Scr,
				TAG_DONE )))
	return( 4L );

	SetMenuStrip( PIPWnd, PIPMenus );
	GT_RefreshWindow( PIPWnd, NULL );

	return( 0L );
}

void ClosePIPWindow( void )
{
	if ( PIPMenus      ) {
		ClearMenuStrip( PIPWnd );
		FreeMenus( PIPMenus );
		PIPMenus = NULL;	}

	if ( PIPWnd        ) {
		CloseWindow( PIPWnd );
		PIPWnd = NULL;
	}
}

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
