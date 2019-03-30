#include <JEO:JEO.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/dos.h>

#include <libraries/reqtools.h>
#include <proto/reqtools.h>
#include "JEO:raw.h"

#define ERROR_X	250

#define PLAY_NEW	 0
#define PLAY_NEXT	-1
#define PLAY_SAME	-2

extern BYTE User[50];
extern BYTE Licens[21];
extern UBYTE Ver[];
extern BYTE cur_preset;
extern UBYTE max_samples;
extern BOOL save_flag;

#define NOT_LOADED			0
#define LOADED_MONO			1
#define LOADED_STEREO		2
#define LOADED_INSTRUMENT	3
#define LOADED_WAVE			4

#define BREV		25
#define DATA		26
#define PRINT		 2
#define NOPRINT		 3

#define DB		DisplayBeep (Scr)

#define StatusX		 75
#define StatusY		 22
#define OpsetX		543
#define OpsetY		 44

#define MAXASSIGN	 15		/* Max oppsett */
#define COMMENTLEN	 30
#define SHOWNAMELENGTH	 23		/* Show namelength */
#define NAMELENGTH	108

#define CHIPLENGTH	(102400 * 2)	/* Stereo version */

typedef struct
{
  BYTE  Path[NAMELENGTH],
      	Name[NAMELENGTH];
  char  OrigName[81],
        Copyright[81],
        Auth[81],
        Anno[81];
  UWORD volume;
  WORD  loop;
} Assign;

extern Assign far assign[];
extern Assign far dummyassign[];
extern char Version[];

extern char Ready[];
extern char Please[];
extern char Ok[];
extern char Message[];

typedef struct
{
  BYTE CurrentDir[NAMELENGTH];
  UWORD pqFlag;
} Prefs;

extern Prefs prefs[1];
extern BYTE next;

extern struct Window *PIPWnd, *pw;
extern struct RastPort *rp, *prp;
extern struct Screen *Scr;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct ReqToolsBase *ReqToolsBase;

extern struct NewWindow InfoWindow, PrinterNewWindowStructure1;

extern BOOL  editFlag;
extern UWORD editCol[];
extern BOOL  menuFlag;
extern BOOL  gadgetFlag;
extern BOOL  gadgetFlagEdit;
extern BOOL  loadAssignFlag;
extern UBYTE sampleFlag[];
extern ULONG start1[];
extern ULONG start2[];
extern BYTE *FastBuffer[];
extern UWORD period[];
extern LONG  size[];
extern LONG  freq[];
extern BYTE  Time[][13];
extern BYTE  ErrorBuffer[][COMMENTLEN + 1];
extern BYTE  Buf[500];
extern BYTE  rtDir[NAMELENGTH];
extern BYTE  Dummy[216];
extern BOOL  setupFlag;
extern BOOL  clearOneSampleFlag;
extern BOOL  opsetFlag[MAXASSIGN];
extern char  AssignName[15][11];

extern UWORD xTab;
extern UWORD yTab[];
extern BYTE Func[14][4];

extern int os_nerr;      /* Highest valid error number */

extern struct
{
  WORD err_no;
  BYTE *msg;
} os_errlist[];

WORD  PlayDiskSample (UWORD nr, struct Window *w, struct Task *mt, char *FileName, ULONG buf_size, BYTE mode);
VOID  CleanUp (BYTE *Streng);
VOID  Status (char *Streng);
BOOL  CheckRAM (VOID);
VOID  SaveAssign (UWORD nr);
VOID  CalcTime (WORD nr);
VOID  PlaySound (WORD nr);
VOID Load (UWORD nr);
BOOL  LoadAssign (UWORD nr);
VOID  InfoWin (VOID);
VOID  ClearNames (UWORD from, UWORD to);
VOID  MenuPick (USHORT code);
VOID  Edit (VOID);
VOID  RawKey (USHORT code);
VOID  SetUp (VOID);
VOID  JEOGadget (UWORD nr, BOOL flag);
VOID  GadgetUp (APTR Address);
VOID  FileReq (UWORD nr);
VOID  DeleteFileReq (VOID);
VOID  Gadgets (BOOL status);
VOID  LoadPrefs (VOID);
VOID  SavePrefs (VOID);
UWORD Print (VOID);
UWORD PrinterMenu (VOID);
UWORD PrinterGadgetUp (APTR Address);
VOID  Menus (BOOL status);
VOID  ShowName (WORD y, BYTE *Name, UWORD col);
VOID  Comment (UWORD nr);
BOOL  LoadDummyAssign (UWORD nr);
VOID  CheckSample (UWORD start, UWORD end);
WORD  LoadSample (UBYTE *FileName, UWORD nr);
VOID EditSampleInfo (UWORD nr);
VOID Do_volume (UWORD tmp_nr);
VOID Do_save (BOOL mode);

#define GREY	0
#define BLACK	1
#define WHITE	2
#define BLUE	3

extern UBYTE video_mode;
extern struct Gadget F1GD;
extern struct Gadget F2GD;
extern struct Gadget F3GD;
extern struct Gadget F4GD;
extern struct Gadget F5GD;
extern struct Gadget F6GD;
extern struct Gadget F7GD;
extern struct Gadget F8GD;
extern struct Gadget F9GD;
extern struct Gadget F10GD;
extern struct Gadget T1GD;
extern struct Gadget T2GD;
extern struct Gadget T3GD;
extern struct Gadget T4GD;

extern struct IntuiText PrinterIText5;

extern struct Gadget PrinterOpset1;
extern struct Gadget PrinterOpset2;
extern struct Gadget PrinterOpset3;
extern struct Gadget PrinterOpset4;
extern struct Gadget PrinterOpset5;
extern struct Gadget PrinterOpset6;
extern struct Gadget PrinterOpset7;
extern struct Gadget PrinterOpset8;
extern struct Gadget PrinterOpset9;
extern struct Gadget PrinterOpset10;
extern struct Gadget PrinterOpset11;
extern struct Gadget PrinterOpset12;
extern struct Gadget PrinterOpset13;
extern struct Gadget PrinterOpset14;
extern struct Gadget PrinterOpset15;

extern struct IntuiText PrinterIText5;
#define PrinterIntuiTextList1 PrinterIText5
