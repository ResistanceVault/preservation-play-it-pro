;/*
sc5 -j73 -v -O PIP
quit
*/

#include <JEO:JEO.h>
#include <exec/memory.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <graphics/gfxbase.h>
#include <exec/tasks.h>

#include <libraries/reqtools.h>
#include <proto/reqtools.h>

#include "JEO:raw.h"	
#include <-P:PIPWindow.h>

#include <-p:header.h>

#define GREY	0
#define BLACK	1
#define WHITE	2
#define BLUE	3

#define BREV		25
#define DATA		26
#define PRINT		 2
#define NOPRINT		 3

UBYTE video_mode;

char Ready[] = "Ready.";
char Please[] = "Please wait...";
char Ok[] = "Ok";
char Message[] = "Message";

UBYTE max_samples;

#define DB		DisplayBeep (Scr)

#define MAXASSIGN	 15		/* Max oppsett */
#define SHOWNAMELENGTH	 23		/* Show namelength */
#define NAMELENGTH	108

/***************************************************************************
--------------------------------- extern -----------------------------------
***************************************************************************/

extern UBYTE ProgramVersion[];

extern struct Gadget F1GD;
/*
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
*/

extern struct Image Statusimg[];

UBYTE Ver[80];
UWORD screenWidth, screenHeight;
UWORD mainX;

/***************************************************************************
----------------------------------------------------------------------------
***************************************************************************/

VOID  Menus (BOOL status);
VOID  ShowName (WORD y, BYTE *Name, UWORD col);
VOID  ClearOneSample (VOID);
VOID  Load (UWORD nr);
VOID  Save (WORD nr);
VOID  CleanUp (BYTE *String);
VOID  SaveAssign (UWORD nr);
VOID  CalcTime (WORD nr);
VOID  PlaySound (WORD nr);
BOOL  LoadAssign (UWORD nr);
VOID  InfoWin (VOID);
VOID  ClearNames (UWORD from, UWORD to);
VOID  MenuPick (USHORT code);
VOID  Edit (VOID);
VOID  RawKey (USHORT code);
VOID  SetUp (VOID);
VOID  JEOGadget (UWORD nr, BOOL flag);
VOID  GadgetUp (APTR Address);
VOID  Comment (UWORD nr);
VOID  FileReq (UWORD nr);
VOID  DeleteFileReq (VOID);
VOID  Gadgets (BOOL status);
VOID  LoadPrefs (VOID);
VOID  SavePrefs (VOID);
UWORD Print (VOID);
UWORD PrinterMenu (VOID);
UWORD PrinterGadgetUp (APTR Address);

Assign far assign[14];
Assign far dummyassign[14];

Prefs prefs[1];

BYTE next = -1;

struct Window *pw = 0;
struct RastPort *rp = 0, *prp = 0;
struct ViewPort *vp = 0;
struct IntuitionBase *IntuitionBase = 0;
struct ReqToolsBase *ReqToolsBase = 0;

BOOL  editFlag, change_volume, change_loop, sample_info;
UWORD editCol[14];
BOOL  menuFlag;
BOOL  gadgetFlag;
BOOL  loadAssignFlag;
UBYTE sampleFlag[14];
ULONG start1[14];
ULONG start2[14];
//WORD  loop[14];
LONG  size[14];
LONG  freq[14];
BYTE  Time[14][13];
BYTE  ErrorBuffer[14][COMMENTLEN + 1];
BYTE  Buf[500];
BYTE  rtDir[NAMELENGTH];
BYTE  Dummy[216];
BOOL  setupFlag;
BOOL  clearOneSampleFlag;
BOOL  opsetFlag[MAXASSIGN];

APTR oldwinptr;
struct Process *pr;

/*
extern BPTR _Backstdout;
LONG _BackGroundIO = 1;	
LONG _stack = 4000;
BYTE *_procname = "PlayIt Professional";
LONG _priority = 2;
*/

char AssignName[15][11] =
{
  "Preset  1",
  "Preset  2",
  "Preset  3",
  "Preset  4",
  "Preset  5",

  "Preset  6",
  "Preset  7",
  "Preset  8",
  "Preset  9",
  "Preset 10",

  "Preset 11",
  "Preset 12",
  "Preset 13",
  "Preset 14",
  "Preset 15"
};

UWORD xTab = 56;

UWORD yTab[] =
{
  45-1, 60-2, 75-3, 90-4, 105-5, 120-6, 135-7, 150-8, 165-9, 180-10, 195-11, 210-12, 225-13, 240-14
};

BYTE Func[14][4] = 
{
  "F 1", "F 2", "F 3", "F 4", "F 5", "F 6", "F 7", "F 8", "F 9", "F10",
  "N 1", "N 2", "N 3", "N 4"
};

int os_nerr = 232;      /* Highest valid error number */

struct
{
  WORD err_no;
  BYTE *msg;
} os_errlist[] = 
{      
  103, "Insufficient free store!!!",
  105, "Task table full!!!",
  120, "Argument line invalid or too long!!!",
  121, "File is not an object module!!!",
  122, "Invalid resident library during load!!!",
  201, "No default directory!!!",
  202, "Object in use!!!",
  203, "Object already exists!!!",
  204, "Directory not found!!!",
  205, "Object not found!!!",
  206, "Invalid window!!!",
  207, "Object too large!!!",
  209, "Action not known!!!",
  210, "Invalid stream component name!!!",
  211, "Invalid lock!!!",
  212, "Object not of required type!!!",
  213, "Disk not validated!!!",
  214, "Disk write protected!!!",
  215, "Rename across devices attempted!!!",
  216, "Directory not empty!!!",
  217, "Too many levels!!!",
  218, "Device not mounted!!!",
  219, "Seek error!!!",
  220, "Comment too big!!!",
  221, "Disk full!!!",
  222, "File is protected from deletion!!!",
  223, "File is protected from writing!!!",
  224, "File is protected from reading!!!",
  225, "Not a DOS disk!!!",
  226, "No disk in drive!!!",
  209, "Packet request type unknown!!!",
  211, "Invalid object lock!!!",
  219, "Seek error!!!",
  232, "No more entries in directory"
};

/***************************************************************************
------------------------------- CleanUp ------------------------------------
***************************************************************************/

VOID CleanUp (BYTE *String)
{
  if (ReqToolsBase AND strlen (String) > 0)
    rtEZRequestTags (String, Ok , NULL, NULL, RT_ReqPos, REQPOS_CENTERSCR, TAG_END);

  if (ReqToolsBase)
		CloseLibrary ((struct Library *)ReqToolsBase);

  if (PIPWnd)
  {
		SavePrefs ();
    pr->pr_WindowPtr = oldwinptr;
    ClosePIPWindow ();
  }
  CloseDownScreen ();
  if (GfxBase)
	  CloseLibrary ((struct Library *)GfxBase);
  if (IntuitionBase)
		CloseLibrary ((struct Library *)IntuitionBase);

  LED (ON);
  exit (0);
}

BOOL CheckRAM (VOID)
{
  LONG availChip, availFast;
  BYTE AvailCHIP[15], AvailFAST[15];

  availChip = AvailMem (MEMF_CHIP);
  availFast = AvailMem (MEMF_FAST);
  sprintf (AvailCHIP, "%5ld Kb CHIP", availChip / 1024);
  sprintf (AvailFAST, "%5ld Kb FAST", availFast / 1024);
  JEOWrite (rp, 530, 72, AvailCHIP, BLACK);
  JEOWrite (rp, 530, 82, AvailFAST, BLACK);

  if (availChip < (CHIPLENGTH + 1024))
    return (FALSE);
  else
    return (TRUE);
}

VOID Gadgets (BOOL status)
{
  if (status == ON)
  {
    if (gadgetFlag == OFF)
    {
      AddGList (PIPWnd, &F1GD, NULL, max_samples, NULL);
      gadgetFlag = ON;
    }
  }
  else	/* OFF */
  {
    if (gadgetFlag == ON)
    {
      RemoveGList (PIPWnd, &F1GD, max_samples);
      gadgetFlag = OFF;
    }
  }
}

VOID ReverseString (UBYTE *String)
{
	ULONG i, len, x;
	UBYTE c;

	x = len = strlen (String);
	x >>= 1;
	len--;
	for (i = 0; i < x; i++, len--)
	{
		c = String[i];
		String[i] = String[len];
		String[len] = c;
	}
}

VOID PlaySound (WORD nr)
{
  WORD ret, len;
  char FileName[216];
  char Volume[3], Loop[4];
  UBYTE vol;
  WORD loop;
  WORD count = 0;
  BYTE mode;
  ULONG buf_size;

	if (video_mode == NTSC)
	{
		if (nr <= 13 AND nr >= 11)
			return;
	}

  Gadgets (OFF);
  Menus (OFF);

	if ((nr != -1) OR  (nr != -2))	// Ikke en neste spill eller samme
	{
		if (change_volume)			// Forandre volume
		{
		  if (sampleFlag[nr])		// Har vi henta en sample her?
	  	{
				JEOGadget (nr, ON);
				Status (Please);
				CheckRAM ();
				while (1)
				{
					sprintf (Volume, "%ld", assign[nr].volume);
					if (GetReqString (Volume, 2, "Enter new volume (1 - 64):"))
					{
						vol = atoi (Volume);
						if (vol < 1 OR vol > 64)
							JEOReqRequest (Message, "Volume must be between 1 and 64!\n\nPlease try again...", Ok);
						else
						{
							if (assign[nr].volume != vol)
							{
								Do_save (ON);
								assign[nr].volume = vol;
								Comment (nr);
							}
							break;
						}
					}
					else
						break;
				}
			}
			CheckRAM ();
			change_volume = OFF;
			Status (Ready);
			goto end;
		}
		else if (change_loop)			// Forandre loop
		{
		  if (sampleFlag[nr])		// Har vi henta en sample her?
  		{
				JEOGadget (nr, ON);
				Status (Please);
				CheckRAM ();
				sprintf (Loop, "%ld", assign[nr].loop);
				if (GetReqString (Loop, 3, "Enter # of loops (0 - 999):"))
				{
					loop = atoi (Loop);

					if (assign[nr].loop != loop)
					{
						Do_save (ON);
						assign[nr].loop = loop;
						Comment (nr);
					}
				}
			}
			CheckRAM ();
			change_loop = OFF;
			Status (Ready);
			goto end;
		}
		else if (sample_info)			// Edit sample info
		{
		  if (sampleFlag[nr])		// Har vi henta en sample her?
  		{
				JEOGadget (nr, ON);
				Status (Please);
				EditSampleInfo (nr);
				CheckRAM ();
			}
			CheckRAM ();
			sample_info = OFF;
			Status (Ready);
			goto end;
		}
	  else if (editFlag)		// Hent en sample
  	{
    	JEOGadget (nr, ON);
	    Status (Please);

	    CheckRAM ();
  	  if (strlen (assign[nr].Path) > 0)		// Har vi allerede noe her?
    	  strcpy (rtDir, assign[nr].Path);	// jepps ;)
	    FileReq (nr);
  	  CheckRAM ();
	    editFlag = OFF;
  	  Status (Ready);
	    goto end;
	  }
		if (clearOneSampleFlag == ON)
	  {
  	  JEOGadget (nr, OFF);
	    ClearNames (nr, nr + 1);
  	  clearOneSampleFlag = OFF;
			Do_save (ON);
    	return;
	  }
	}
  if (nr == -1) // Vi skal spille neste
  {
  	UBYTE i;

		change_volume = change_loop =	sample_info = editFlag = OFF; // Slår av alt!
  	mode = PLAY_NEXT;
  	for (i = next + 1;; i++)	// Vi scanner 2 ganger for sikkerhetsskyld
  	{
  		count++;
  		if (count > 28)
  			goto end;			// Vi fant ingen :(

			if (i > 14)
				i = 0;
  		if (*(assign[i].Name))	// Har vi noe her?
  		{
				if (editCol[i] == BLACK)	// Har vi en godkjent sample?
				{
  				nr = next = i;
	  			break;
	  		}
  		}
  	}
  }
  else if (nr == -2) // Vi skal spille samme
  {
		change_volume = change_loop =	sample_info = editFlag = OFF; // Slår av alt!
  	mode = PLAY_SAME;
  	if (next == -1)	// Vi har ikke spilt noen
  		goto end;
		nr = next;
	}
	else
		mode = PLAY_NEW;

  if (sampleFlag[nr])	// Vi spiller
  {
		next = nr;	// Just in case
    if (!CheckRAM ())
    {
      DB;
      Status ("Not enough CHIP RAM!");
      goto end;
    }
//    ModifyIDCMP (PIPWnd, RAWKEY);	/* Check only neccessary */
    ClearIntuition (PIPWnd);
    rtSetWaitPointer (PIPWnd);
    JEOGadget (nr, ON);
    LED (OFF);
    strcpy (FileName, assign[nr].Path);
    len = strlen (FileName);
    if (FileName[len - 1] != ':')
      strcat (FileName, "/");
    strcat (FileName, assign[nr].Name);

		if (sampleFlag[nr] == LOADED_WAVE)
		{
		  sprintf (Dummy, "Playing \"%s\" (Extern player). Loop = %ld...", assign[nr].Name, assign[nr].loop);
		  Status (Dummy);
			sprintf (Dummy, "C:Play16 \"%s\" FILTER=OFF LOOPS=%ld VOLUME=%ld >nil:", FileName, assign[nr].loop, (assign[nr].volume * 15625) / 10000);
			Execute (Dummy, NULL, NULL);
//  	  ModifyIDCMP (PIPWnd, MOUSEBUTTONS+GADGETUP+MENUPICK+RAWKEY);
			Status (Ready);
		}
		else	// Da spiller vi internt
		{
			
			if (sampleFlag[nr] == LOADED_STEREO AND size[nr] > (150 * 1024))
			  SetRGB4 (&(Scr->ViewPort), 0, 0x0E, 0x00, 0x00);
			Status ("Seeking, please wait...");

			buf_size = 100 * 1024;
			if (assign[nr].loop == 1)	// Ikke loop
				buf_size = (freq[nr] * 2);	// ca 1/2 load i sek
			else										// vi skal loope!
			{
				if (sampleFlag[nr] == LOADED_MONO)
				{
					if (size[nr] <= (100 * 1024))
						buf_size = size[nr];
					else
						buf_size = 100 * 1024;
				}
			}
	    ret = PlayDiskSample (nr, PIPWnd, (struct Task *)pr, FileName, buf_size, mode);
//  	  ModifyIDCMP (PIPWnd, MOUSEBUTTONS+GADGETUP+MENUPICK+RAWKEY);
    	if (ret == TRUE)
	      Status (Ready);
  	  else
    	{
	      DB;
  	    switch (ret)
    	  {
      	  case NOMEM: Status ("Not enough memory!"); break;
        	case PORT: Status ("Couldn't allocate audio ports!"); break;
	        case DEV: Status ("Couldn't open audio.device!"); break;
  	      case FALSE:
    	    {
      	    sprintf (Dummy, "Couldn't find file '%s'!", assign[nr].Name);
        	  Status (Dummy);
          	break;
          }
        }
      }
    }
  }

end:
  JEOGadget (nr, OFF);
  Menus (ON);
  Gadgets (ON);
  ClearPointer (PIPWnd);
  ClearIntuition (PIPWnd);
}

VOID Edit (VOID)
{
  struct IntuiMessage *Mess;
  ULONG class;
  USHORT Code;
  APTR Address;

  Status ("Please choose a sample...");
  while (editFlag OR change_volume OR change_loop OR sample_info)
  {
    WaitPort (PIPWnd->UserPort);
    while (Mess = (struct IntuiMessage *)GetMsg (PIPWnd->UserPort)) 
    {
      class = Mess->Class;
      Code = Mess->Code;
      Address = Mess->IAddress;
      ReplyMsg ((struct Message *)Mess);

      switch (class) 
      {
        case RAWKEY:
          RawKey (Code);
          break;
        case GADGETUP:
//					change_volume = change_loop = sample_info = OFF;
//					editFlag = ON;
          GadgetUp (Address);
          Gadgets (ON);
          break;
        case MOUSEBUTTONS:
          if (Code == MENUDOWN)
            editFlag = change_volume = change_loop = sample_info = OFF;
          break;
      } 
    } 
  }
  ClearIntuition (PIPWnd);
  Status (Ready);
}

VOID MenuPick (USHORT code)
{
  switch (MENUNUM (code))
  {
    case 0:		/* Project */
    {
      switch (ITEMNUM (code))
      {
        case 0:		// Informasjon
        {
          InfoWin ();
          break;
        }
        case 2:		// Hent lyd
        {
          Menus (OFF);
          editFlag = ON;
          Edit ();
          Menus (ON);
          break;
        }
        case 4:		// Sett lydstyrke
        {
          Menus (OFF);
					change_volume = ON;
          Edit ();
          Menus (ON);
          break;
        }
        case 5:		// Øk volume
        {
          Menus (OFF);
          Do_volume (cur_preset);
          Menus (ON);
          break;
        }
        case 7:		// Sett loop
        {
          Menus (OFF);
					change_loop = ON;
          Edit ();
          Menus (ON);
          break;
        }
        case 9:		// Edit lyd informasjon
        {
          Menus (OFF);
					sample_info = ON;
          Edit ();
          Menus (ON);
          break;
        }
        case 11:		// Slett alle lyder
        {
          ClearNames (0, max_samples);
          break;
        }
        case 12:	// Slett en lyd
        {
          ClearOneSample ();
          break;
        }
        case 14:		// Slett fil
        {
          Menus (OFF);
          Gadgets (OFF);
          DeleteFileReq ();
          Menus (ON);
          Gadgets (ON);
          break;
        }
/*
        case 14:		// Lagre prefs
        {
          SavePrefs ();
          break;
        }
        case 16:	// Print
        {
          switch (Print ())
          {
            case FALSE:
              DB;
              Status ("Printer error!");
              break;
            case TRUE:
              Status (Ready);
              break;
            case NOPRINT:
              Status (Ready);
              break;
          }
          ClearPointer (PIPWnd);
          Menus (ON);
          Gadgets (ON);
          ClearIntuition (PIPWnd);
          break;
        }
*/
        case 16:	// Quit
        {
          Status (Please);
          Menus (OFF);
          Gadgets (OFF);
          if (JEOReqRequest (Message, "Quit PlayIt Professional?\nAre you sure?", "Quit|Cancel"))
            CleanUp ("");
          Menus (ON);
          Gadgets (ON);
          Status (Ready);
        }
      }
      break;
    }
    case 1:		/* Load */
    {
      switch (ITEMNUM (code))
      {
        case  0: Load  (0); break;
        case  1: Load  (1); break;
        case  2: Load  (2); break;
        case  3: Load  (3); break;
        case  4: Load  (4); break;
        case  6: Load  (5); break;
        case  7: Load  (6); break;
        case  8: Load  (7); break;
        case  9: Load  (8); break;
        case 10: Load  (9); break;
        case 12: Load (10); break;
        case 13: Load (11); break;
        case 14: Load (12); break;
        case 15: Load (13); break;
        case 16: Load (14); break;
      }
      break;
    }
    case 2:		// Save
    {
      switch (ITEMNUM (code))
      {
        case  0: Save  (-1); break;	//	Current

        case  2: Save  (0); break;	// 1
        case  3: Save  (1); break;	// 2
        case  4: Save  (2); break;	// 3
        case  5: Save  (3); break;	// 4
        case  6: Save  (4); break;	// 5

        case  8: Save  (5); break;	// 6
        case  9: Save  (6); break;	// 7
        case 10: Save  (7); break;	// 8
        case 11: Save  (8); break;	// 9
        case 12: Save  (9); break;	//10

        case 14: Save (10); break;	//11
        case 15: Save (11); break;	//12
        case 16: Save (12); break;	//13
        case 17: Save (13); break;	//14
        case 18: Save (14); break;	//15
      }
    }
  }
}

VOID RawKey (USHORT code)
{
  switch (code)
  {
    case ESC:
    {
      if (editFlag)
        editFlag = OFF;
      if (change_volume)
        change_volume = OFF;
      if (change_loop)
        change_loop = OFF;
      if (sample_info)
        sample_info = OFF;
      if (clearOneSampleFlag)
        clearOneSampleFlag = OFF;
      break;
    }
    case F1:  PlaySound  (0); break;
    case F2:  PlaySound  (1); break;
    case F3:  PlaySound  (2); break;
    case F4:  PlaySound  (3); break;
    case F5:  PlaySound  (4); break;
    case F6:  PlaySound  (5); break;
    case F7:  PlaySound  (6); break;
    case F8:  PlaySound  (7); break;
    case F9:  PlaySound  (8); break;
    case F10: PlaySound  (9); break;
    case NUMERIC1: PlaySound (10); break;
    case NUMERIC2: PlaySound (11); break;
    case NUMERIC3: PlaySound (12); break;
    case NUMERIC4: PlaySound (13); break;
    case RETURN:	 PlaySound (-1); break;
    case SPACE:		 PlaySound (-2); break;
		case RIGHT_ARR:
		{
			if (cur_preset == -1 OR cur_preset == 14)	// Ingen ennå eller det siste?
				Load (0);	// Det første preset
			else
				Load (cur_preset + 1);
			break;
		}
		case LEFT_ARR:
		{
			if (cur_preset == -1)	// Ingen ennå?
				Load (0);	// Det første preset
			else if (cur_preset == 0)
				Load (14);
			else
				Load (cur_preset - 1);
			break;
		}
  }
}

VOID GadgetUp (APTR Address)
{
  UWORD nr;

  nr = ((struct Gadget *)Address)->GadgetID;

	if (video_mode == NTSC)
	{
		if (nr <= 13 AND nr >= 11)
			return;
	}

  if (clearOneSampleFlag == ON)	// Slette en?
  {
    JEOGadget (nr, OFF);
    ClearNames (nr, nr + 1);
		Do_save (ON);
    return;
  }
  if ((sampleFlag[nr]) OR editFlag == ON OR change_volume == ON OR change_loop == ON OR sample_info == ON)
  {
    PlaySound (nr);
    return;
  }
  else
  {
    JEOGadget (nr, OFF);
    return;
  }
}

VOID SetUp (VOID)
{
  LONG i;

	save_flag = OFF;
	sprintf (Version, "PlayItPro %s", ProgramVersion);
  setupFlag = TRUE;
  DrawImage (rp, Statusimg, 0, 12);  

	if (video_mode == PAL)
	  StyleIt2 (rp,   0,  31, 500, 233, WHITE, BLACK, GADGET_STYLE); // Main
	else
	  StyleIt2 (rp,   0,  31, 500, 191, WHITE, BLACK, GADGET_STYLE); // Main
  StyleIt2 (rp, 533,  35, 631,  49, BLACK, WHITE, GADGET_STYLE); // Preset
  StyleIt2 (rp, 524,  63, 640,  86, BLACK, WHITE, GADGET_STYLE); // Free RAM
  JEOWrite (rp, OpsetX, OpsetY, "Preset --", BLACK);
  cur_preset = -1;

  for (i = 1; i <= MAXASSIGN; i++)
    opsetFlag[i] = OFF;

  editFlag = change_volume = change_loop = sample_info = OFF;
  clearOneSampleFlag = FALSE;

  for (i = 0; i < max_samples; i++)
  {
    ErrorBuffer[i][0] = 0;
    assign[i].Path[0] = 0;
    assign[i].Name[0] = 0;
    assign[i].volume = 45;
    assign[i].loop = 1;
    size[i] = 0;
    freq[i] = 0;
    sampleFlag[i] = NOT_LOADED;
    editCol[i] = BLUE;
    Comment (i);
    JEOWrite (rp, 15, yTab[i], Func[i], BLACK);
  }

  LoadPrefs ();
  InfoWin ();
  setupFlag = FALSE;
  LED (OFF);
}

/***************************************************************************
---------------------------------- main ------------------------------------
***************************************************************************/

VOID __stdargs __main (char *Line)
//VOID main (VOID)
{
  struct IntuiMessage *Mess;
  ULONG class;
  USHORT Code;
  APTR Address;
//  struct Screen wbs;

	mainX = 0;
  screenWidth = 640;

  if (!(pr = (struct Process *)FindTask (NULL)))
    CleanUp ("Task error!");
  oldwinptr = pr->pr_WindowPtr;
  if (!(pr->pr_FileSystemTask = DeviceProc ("RAM")))
    CleanUp ("Task error!");

  if (!(ReqToolsBase = (struct ReqToolsBase *)OpenLibrary (REQTOOLSNAME, REQTOOLSVERSION)))
    CleanUp ("Couldn't open reqtools.library!");

  if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary ("intuition.library", 0)))
    CleanUp ("Couldn't open intuition.library!");
  if (!(GfxBase = (struct GfxBase *)OpenLibrary ("graphics.library", 0)))
    CleanUp ("Couldn't open graphics.library!");
  video_mode = CheckMode ();	// PAL or NTSC?
	if (video_mode == PAL)
	{
		screenHeight = 256;
		max_samples = 14;
	}
	else
	{
		screenHeight = 200;
		max_samples = 11;
	}

  if (SetupScreen ())
    CleanUp ("Couldn't open screen!");
  if (OpenPIPWindow ())
    CleanUp ("Couldn't open window!");

  sprintf (Ver, "PlayItPro v%s - Copyright Jan Erik Olausen 1992 © 1999", ProgramVersion);
  SetWindowTitles (PIPWnd, NULL, Ver);

  rp = PIPWnd->RPort;
  pr->pr_WindowPtr = PIPWnd;

  SetDrMd (rp, JAM2);
  gadgetFlag = ON;
  menuFlag = ON;

  SetUp ();

	CheckRAM ();
	Load (0);
  FOREVER
  {
    WaitPort (PIPWnd->UserPort);
    while (Mess = (struct IntuiMessage *)GetMsg (PIPWnd->UserPort)) 
    {
      class = Mess->Class;
      Code = Mess->Code;
      Address = Mess->IAddress;

      ReplyMsg ((struct Message *)Mess);
      switch (class) 
      {
        case GADGETUP:
					editFlag = ON;
          GadgetUp (Address);
          break;
        case RAWKEY:
          RawKey (Code);
          break;
        case MENUPICK:
          MenuPick (Code);
          break;
      } 
    } 
    CheckRAM ();
  }
}
