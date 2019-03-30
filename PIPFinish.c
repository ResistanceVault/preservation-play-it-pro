;/*
sc5 -j73 -v -O PIPFinish
quit
*/

#include <-P:Header.h>
#include <JEO:IFF.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>

#define BLANK	 0, ""
#define TEXT_END	-1, ""

extern UWORD screenWidth, screenHeight;
BYTE cur_preset;
BOOL save_flag;

struct
{
  WORD col;
  BYTE *Text;

} StartUpText[] =
  {
    BLUE,	Version,
    BLANK,
    BLACK,	"Copyright 1992 © 1998",
    BLANK,
    BLACK,	"Jan Erik Olausen",
    BLACK,	"jolausen@online.no",
    BLANK,
    WHITE,  "PlayItPro is FREEWARE!",
    TEXT_END
};

VOID Do_save (BOOL mode)
{
	UBYTE col;

	save_flag = mode;
	if (save_flag)
		col = BLUE;
	else
		col = BLACK;

	JEOWrite (rp, OpsetX, OpsetY, AssignName[cur_preset], col);
}


VOID SaveAssign (UWORD nr)
{
  struct FileHandle *fh;
  BYTE Name[30];

  sprintf (Name, "S:PlayItPro.%s", AssignName[nr]);
  if ((fh = (struct FileHandle *)Open (Name, MODE_NEWFILE)) != NULL)
  {
    Write ((BPTR) fh, assign, sizeof (Assign) * max_samples);
    Close ((BPTR) fh);
		Do_save (OFF);
    cur_preset = nr;
  }
  else
  {
    DB;
    sprintf (Buf, "Error saving \"%s\"!", AssignName[nr]);
    rtEZRequestTags (Buf, Ok, NULL, NULL, RT_ReqPos, REQPOS_TOPLEFTSCR, TAG_END);
  }
  ClearIntuition (PIPWnd);
}

BOOL LoadAssign (UWORD nr)
{
  struct FileHandle *fh;
  BYTE Name[30];

	next = -1;
  sprintf (Name, "S:PlayItPro.%s", AssignName[nr]);
  if ((fh = (struct FileHandle *)Open (Name, MODE_OLDFILE)) != NULL)
  {
    Read ((BPTR) fh, assign, sizeof (Assign) * max_samples);
    Close ((BPTR) fh);
    loadAssignFlag = ON;
    cur_preset = nr;
    return (TRUE);
  }
  else
  {
    DB;
    sprintf (Buf, "Error loading \"%s\"!", AssignName[nr]);
    rtEZRequestTags (Buf, Ok, NULL, NULL, RT_ReqPos, REQPOS_TOPLEFTSCR, TAG_END);
    loadAssignFlag = OFF;
    return (FALSE);
  }
}

VOID CalcTime (WORD nr)
{
  UWORD min, sek, i, time;
  UBYTE Sek[3], Del[3];

  strcpy (Time[nr], "0:00.00");

  if (sampleFlag[nr])
  {
    time = (size[nr] * 100) / freq[nr];

    CalcDecimal (Time[nr], time);
    for (i = 0; i < strlen (Time[nr]); i++)
    {
      if (Time[nr][i] == '.')
      {
        Del[0] = Time[nr][i+1];
        Del[1] = Time[nr][i+2];
        Del[2] = '\0';
        break;
      }
    }
    sek = time / 100;
    min = sek / 60;
    sek = sek % 60;
    strcpy (Sek, "00");
    if (sek < 10 AND sek > 0)
    {
      stci_d (Sek, sek);
      Sek[1] = Sek[0];
      Sek[0] = '0';
      Sek[2] = '\0';
    }
    else if (sek > 0)
      stci_d (Sek, sek);

    sprintf (Time[nr], "%ld:%s.%s", min, Sek, Del);
  }
}

VOID JEOGadget (UWORD nr, BOOL flag)
{
  USHORT col = WHITE;

	if (video_mode == NTSC)
	{
		if (nr <= 13 AND nr >= 11)
			return;
	}

  switch (flag)
  {
    case ON:
    {
      switch (nr)
      {
        case  0: if (!(F1GD.Flags & SELECTED))  { F1GD.Flags  ^= SELECTED; RefreshGList (&F1GD,  PIPWnd, NULL, 1); } break;
        case  1: if (!(F2GD.Flags & SELECTED))  { F2GD.Flags  ^= SELECTED; RefreshGList (&F2GD,  PIPWnd, NULL, 1); } break;
        case  2: if (!(F3GD.Flags & SELECTED))  { F3GD.Flags  ^= SELECTED; RefreshGList (&F3GD,  PIPWnd, NULL, 1); } break;
        case  3: if (!(F4GD.Flags & SELECTED))  { F4GD.Flags  ^= SELECTED; RefreshGList (&F4GD,  PIPWnd, NULL, 1); } break;
        case  4: if (!(F5GD.Flags & SELECTED))  { F5GD.Flags  ^= SELECTED; RefreshGList (&F5GD,  PIPWnd, NULL, 1); } break;
        case  5: if (!(F6GD.Flags & SELECTED))  { F6GD.Flags  ^= SELECTED; RefreshGList (&F6GD,  PIPWnd, NULL, 1); } break;
        case  6: if (!(F7GD.Flags & SELECTED))  { F7GD.Flags  ^= SELECTED; RefreshGList (&F7GD,  PIPWnd, NULL, 1); } break;
        case  7: if (!(F8GD.Flags & SELECTED))  { F8GD.Flags  ^= SELECTED; RefreshGList (&F8GD,  PIPWnd, NULL, 1); } break;
        case  8: if (!(F9GD.Flags & SELECTED))  { F9GD.Flags  ^= SELECTED; RefreshGList (&F9GD,  PIPWnd, NULL, 1); } break;
        case  9: if (!(F10GD.Flags & SELECTED)) { F10GD.Flags ^= SELECTED; RefreshGList (&F10GD, PIPWnd, NULL, 1); } break;
        case 10: if (!(T1GD.Flags & SELECTED))  { T1GD.Flags  ^= SELECTED; RefreshGList (&T1GD,  PIPWnd, NULL, 1); } break;
        case 11: if (!(T2GD.Flags & SELECTED))  { T2GD.Flags  ^= SELECTED; RefreshGList (&T2GD,  PIPWnd, NULL, 1); } break;
        case 12: if (!(T3GD.Flags & SELECTED))  { T3GD.Flags  ^= SELECTED; RefreshGList (&T3GD,  PIPWnd, NULL, 1); } break;
        case 13: if (!(T4GD.Flags & SELECTED))  { T4GD.Flags  ^= SELECTED; RefreshGList (&T4GD,  PIPWnd, NULL, 1); } break;
      }
      if (nr < max_samples)
      {
        SetBPen (rp, BLUE);
        if (!sampleFlag[nr])
          col = GREY;
        ShowName (yTab[nr], assign[nr].Name, col);
        SetBPen (rp, GREY);
      }
      break;
    }
    case OFF:
    {
      switch (nr)
      {
        case  0: if (F1GD.Flags & SELECTED)  { F1GD.Flags  ^= SELECTED; RefreshGList (&F1GD,  PIPWnd, NULL, 1); } break;
        case  1: if (F2GD.Flags & SELECTED)  { F2GD.Flags  ^= SELECTED; RefreshGList (&F2GD,  PIPWnd, NULL, 1); } break;
        case  2: if (F3GD.Flags & SELECTED)  { F3GD.Flags  ^= SELECTED; RefreshGList (&F3GD,  PIPWnd, NULL, 1); } break;
        case  3: if (F4GD.Flags & SELECTED)  { F4GD.Flags  ^= SELECTED; RefreshGList (&F4GD,  PIPWnd, NULL, 1); } break;
        case  4: if (F5GD.Flags & SELECTED)  { F5GD.Flags  ^= SELECTED; RefreshGList (&F5GD,  PIPWnd, NULL, 1); } break;
        case  5: if (F6GD.Flags & SELECTED)  { F6GD.Flags  ^= SELECTED; RefreshGList (&F6GD,  PIPWnd, NULL, 1); } break;
        case  6: if (F7GD.Flags & SELECTED)  { F7GD.Flags  ^= SELECTED; RefreshGList (&F7GD,  PIPWnd, NULL, 1); } break;
        case  7: if (F8GD.Flags & SELECTED)  { F8GD.Flags  ^= SELECTED; RefreshGList (&F8GD,  PIPWnd, NULL, 1); } break;
        case  8: if (F9GD.Flags & SELECTED)  { F9GD.Flags  ^= SELECTED; RefreshGList (&F9GD,  PIPWnd, NULL, 1); } break;
        case  9: if (F10GD.Flags & SELECTED) { F10GD.Flags ^= SELECTED; RefreshGList (&F10GD, PIPWnd, NULL, 1); } break;
        case 10: if (T1GD.Flags & SELECTED)  { T1GD.Flags  ^= SELECTED; RefreshGList (&T1GD,  PIPWnd, NULL, 1); } break;
        case 11: if (T2GD.Flags & SELECTED)  { T2GD.Flags  ^= SELECTED; RefreshGList (&T2GD,  PIPWnd, NULL, 1); } break;
        case 12: if (T3GD.Flags & SELECTED)  { T3GD.Flags  ^= SELECTED; RefreshGList (&T3GD,  PIPWnd, NULL, 1); } break;
        case 13: if (T4GD.Flags & SELECTED)  { T4GD.Flags  ^= SELECTED; RefreshGList (&T4GD,  PIPWnd, NULL, 1); } break;
      }
      if (nr < max_samples)
        ShowName (yTab[nr], assign[nr].Name, editCol[nr]);
      break;
    }
  }
}

VOID Load (UWORD nr)
{
  ULONG i;

  Menus (OFF);
  Gadgets (OFF);
  if (LoadAssign (nr))
  {
		Do_save (OFF);
    rtSetWaitPointer (PIPWnd);
    ClearNames (0, max_samples);
    for (i = 0; i < max_samples; i++)
      ShowName (yTab[i], assign[i].Name, BLUE);

    CheckSample (0, max_samples);
    ClearPointer (PIPWnd);
    CheckRAM ();
  }
  Menus (ON);
  Gadgets (ON);
  Status (Ready);
}

VOID Save (WORD nr)
{
  Status (Please);
  Menus (OFF);
  Gadgets (OFF);
	if (nr == -1)	// Current?
	{
		if (cur_preset == -1)	// Ikke noe ennå?
		{
			DB;
		  JEOReqRequest (Message, "Sorry, no current preset!\nNothing saved.", Ok);
			goto end;
		}
		else
			nr = cur_preset;
	}
  sprintf (Buf, "Save as \"%s\"?\nAre you sure?", AssignName[nr]);
  if (JEOReqRequest (Message, Buf, "Save|cancel"))
    SaveAssign (nr);

end:
  Menus (ON);
  Gadgets (ON);
  Status (Ready);
}

VOID InfoWin (VOID)
{
  struct Window *w1;
  struct RastPort *rp1;
  struct IntuiMessage *Message;
  ULONG class;
  USHORT code;
  WORD x, y, maxLines;
  WORD maxLength = 0;
  BOOL flag = TRUE;
  LONG i;
  ULONG win1, win2;

  Status (Please);
  Menus (OFF);
  Gadgets (OFF);
  InfoWindow.Screen = Scr;

	maxLines = 0;
  for (i = 0; ; i++)
  {
    if (StartUpText[i].col == -1)
    {
      maxLines = i;
      break;
    }
    if (strlen (StartUpText[i].Text) > maxLength)
      maxLength = strlen (StartUpText[i].Text);
  }
  InfoWindow.Width    = (maxLength * 8) + 32;
  InfoWindow.Height   = (maxLines  * 8) + 16;
  InfoWindow.LeftEdge = (screenWidth - InfoWindow.Width)   / 2;
  InfoWindow.TopEdge  = (screenHeight - InfoWindow.Height) / 2;
  if (!(w1 = OpenWindow (&InfoWindow)))
    goto Error;
  rp1 = w1->RPort;
  y = 14;
  for (i = 0; i < maxLines; i++)
  {
    x = CenterWText (w1, StartUpText[i].Text);
    JEOWrite (rp1, x, y, StartUpText[i].Text, StartUpText[i].col);
    y += 8;
  }

  CheckRAM ();
  ScreenToFront (Scr);
  while (flag)
  {
    win1 = 1L << PIPWnd->UserPort->mp_SigBit;
    win2 = 1L << w1->UserPort->mp_SigBit;
    Wait (win1 | win2);
    while (Message = (struct IntuiMessage *)GetMsg (w1->UserPort)) 
    {
      class = Message->Class;
      code = Message->Code;

      ReplyMsg ((struct Message *)Message);
      switch (class) 
      {
        case RAWKEY:
        	if (code == RETURN OR code == ENTER OR code == ESC)
	          flag = FALSE;
          break;
        case MOUSEBUTTONS:
          flag = FALSE;
          break;
      }
    }
    while (Message = (struct IntuiMessage *)GetMsg (PIPWnd->UserPort)) 
    {
      class = Message->Class;
      code = Message->Code;

      ReplyMsg ((struct Message *)Message);
      switch (class) 
      {
        case RAWKEY:
	       	if (code == RETURN OR code == ENTER OR code == ESC)
  	        flag = FALSE;
          break;
        case MOUSEBUTTONS:
          flag = FALSE;
          break;
      }
    }
  }
	ClearIntuition (PIPWnd);
  CloseWindow (w1);
Error:
  ClearIntuition (PIPWnd);
  Menus (ON);
  Gadgets (ON);
  Status (Ready);
}

VOID LoadPrefs (VOID)
{
  struct FileHandle *fh;

  if ((fh = (struct FileHandle *)Open ("S:PlayItPro.prefs", MODE_OLDFILE)) != NULL)
  {
    Read ((BPTR)fh, prefs, sizeof (Prefs));
    Close ((BPTR)fh);
    if (prefs[0].pqFlag != BREV AND prefs[0].pqFlag != DATA)
      prefs[0].pqFlag = DATA;
    if (!setupFlag)
      Status ("Prefs loaded.");
    strcpy (rtDir, prefs[0].CurrentDir);
  }
  else
  {
    DB;
    prefs[0].pqFlag = DATA;
    Status ("Error loading prefs!");
  }
  ClearIntuition (PIPWnd);
}

VOID SavePrefs (VOID)
{
  struct FileHandle *fh;

  JEOCopy (prefs[0].CurrentDir, rtDir, NAMELENGTH, NULL);
  if ((fh = (struct FileHandle *)Open ("S:PlayItPro.prefs", MODE_NEWFILE)) != NULL)
  {
    Write ((BPTR)fh, prefs, sizeof (Prefs));
    Close ((BPTR)fh);
    Status ("Prefs saved.");
  }
  else
  {
    DB;
    Status ("Error saving prefs!");
  }
  ClearIntuition (PIPWnd);
}

VOID Status (char *Streng)
{
	char S[71];

	if (strlen (Streng) >= 70)
	{
		strncpy (S, Streng, 70);
		S[68] = '>';
		S[69] = '>';
		S[70] = 0;
	}
	else
		JEOCopy (S, Streng, 70, ' ');
  JEOWrite (rp, StatusX, StatusY, S, BLACK);
}


VOID Menus (BOOL status)
{
  if (status == ON)
  {
    if (menuFlag == OFF)
    {
      Forbid ();
      PIPWnd->Flags ^= RMBTRAP;
      Permit ();
      menuFlag = ON;
    }
  }
  else
  {
    if (menuFlag == ON)
    {
      Forbid ();
      PIPWnd->Flags ^= RMBTRAP;
      Permit ();
      menuFlag = OFF;
    }
  }
}

BOOL LoadDummyAssign (UWORD nr)
{
  struct FileHandle *fh;
  BYTE Name[30];

  sprintf (Name, "S:PlayItPro.%s", AssignName[nr]);
  if ((fh = (struct FileHandle *)Open (Name, MODE_OLDFILE)) != NULL)
  {
    Read ((BPTR) fh, dummyassign, sizeof (Assign) * max_samples);
    Close ((BPTR) fh);
    return (TRUE);
  }
  else
    return (FALSE);
}

BOOL LoadSample (UBYTE *FileName, UWORD nr)
{
  JEO8Header *header;
  WORD check;
  BOOL err = TRUE;

  if (!(header = AllocMem (sizeof (JEO8Header), MEMF_PUBLIC | MEMF_CLEAR)))
  {
    JEOCopy (ErrorBuffer[nr], "Not enough memory!", COMMENTLEN, ' ');
    return (FALSE);
  }

  check = CheckIFF (header, FileName);

  if (check != IFFOK)
  {
		if (check == WAVE_FILE)
		{
			if (loadAssignFlag == OFF)	// Vi henter ikke nytt preset?
			{
				assign[nr].volume = 45;	// Da setter vi standard volume og loop
				assign[nr].loop = 1;
			}
			freq[nr] = WAVE_FILE;
      sampleFlag[nr] = LOADED_WAVE;	// da bruker vi Play16
	    goto end;
		}
		else
		{
	    JEOCopy (ErrorBuffer[nr], IFFMsg[check], COMMENTLEN, ' ');
  	  if (editFlag == ON)
    	  Status (IFFMsg[check]);
	    err = FALSE;
  	  goto end;
  	}
  }
  else
  {
		if (loadAssignFlag == OFF)	// Vi henter ikke en ny sample???
		{
			assign[nr].volume = header->volume >> 10;
			if (assign[nr].volume == 0)
				assign[nr].volume = 45;
			assign[nr].volume = 45;	// Hmmmmmm....
			assign[nr].loop = 1;
		}
    if (header->chan == STEREO)
    {
      size[nr] = header->size / 2;
      sampleFlag[nr] = LOADED_STEREO;
    }
    else
    {
      size[nr] = header->size;
      sampleFlag[nr] = LOADED_MONO;
    }
    if (header->octave > 1)
      sampleFlag[nr] = LOADED_INSTRUMENT;
    freq[nr] = header->hz;
    start1[nr] = header->start1;

    if (header->chan == STEREO)
      start2[nr] = header->start2;
    else
      start2[nr] = header->start1;
		strcpy (assign[nr].OrigName, header->Name);
		strcpy (assign[nr].Copyright, header->Copyright);
		strcpy (assign[nr].Auth, header->Auth);
		strcpy (assign[nr].Anno, header->Anno);
  }

end:
  if (header)
    FreeMem (header, sizeof (JEO8Header));

  return (err);
}

VOID CheckSample (UWORD nr, UWORD max)
{
  WORD col, load;
  LONG i, len;
  UBYTE FileName[216];

  for (i = nr; i < max; i++)
  {
    if (strlen (assign[i].Name) > 0)
    {
      strcpy (FileName, assign[i].Path);
      len = strlen (FileName);
      if (FileName[len - 1] != ':')
        strcat (FileName, "/");
      strcat (FileName, assign[i].Name);

      sprintf (Buf, "Checking \"%s\"", FileName);
      Status (Buf);
      load = LoadSample (FileName, i);
    }
    else
      load = FALSE;

    if (load == TRUE)
      col = BLACK;     
    else
      col = BLUE;

    if (editFlag)
    {
      SetBPen (rp, BLUE);
      if (load == TRUE)
        col = WHITE;
      else
        col = GREY;
      JEOWrite (rp, xTab, yTab[i], "                       ", BLUE);
    }
    if (load == TRUE)
    {
      editCol[i] = BLACK;

//      if (loadAssignFlag == OFF)
//        assign[i].volume = 45;
      ShowName (yTab[i], assign[i].Name, col);
      SetBPen (rp, GREY);
      if (loadAssignFlag == OFF)
        Status (Please);
    }
    else
    {
      if (strlen (assign[i].Name) > 0)
        DB;

      sampleFlag[i] = NOT_LOADED;
      editCol[i] = BLUE;
			assign[i].loop = 1;
      assign[i].volume = 45;
      size[i] = 0;
      freq[i] = 0;

      if (strlen (assign[i].Name) > 0)
        ShowName (yTab[i], assign[i].Name, col);
      SetBPen (rp, GREY);
    }
    Comment (i);
  }
  loadAssignFlag = OFF;
  ClearIntuition (PIPWnd);
}

/*

      if (editFlag == ON)
        Status ("Not enough memory!");
      JEOCopy (ErrorBuffer[nr], "Not enough memory!", COMMENTLEN, ' ');

    if (loadAssignFlag == OFF)
      Status ("File not found!");
    JEOCopy (ErrorBuffer[nr], "File not found!", COMMENTLEN, ' ');
*/

VOID ClearOneSample (VOID)
{
  struct IntuiMessage *Message;
  ULONG class;
  USHORT Code;
  APTR Address;

  Status ("Choose sample to delete...");
  clearOneSampleFlag = ON;
  Menus (OFF);
  while (clearOneSampleFlag)
  {
    WaitPort (PIPWnd->UserPort);
    while (Message = (struct IntuiMessage *)GetMsg (PIPWnd->UserPort)) 
    {
      class = Message->Class;
      Code = Message->Code;
      Address = Message->IAddress;

      ReplyMsg ((struct Message *)Message);
      switch (class) 
      {
        case RAWKEY:
          RawKey (Code);
          break;
        case GADGETUP:
          GadgetUp (Address);
          Gadgets (ON);
          clearOneSampleFlag = OFF;
          break;
        case MOUSEBUTTONS:
          if (Code == MENUDOWN)
            clearOneSampleFlag = OFF;
          break;
      } 
    } 
  }
  Menus (ON);
  ClearIntuition (PIPWnd);
  Status (Ready);
}

VOID ClearNames (UWORD from, UWORD to)
{
  LONG i;
  BOOL flag = FALSE;

  Menus (OFF);
  Gadgets (OFF);

  if (loadAssignFlag == OFF AND clearOneSampleFlag == OFF)
  {
    Status (Please);
    if (!(JEOReqRequest (Message, "Delete all samples?", "Delete all|Cancel")))
    {
      Menus (ON);
      Gadgets (ON);
      CheckRAM ();
      Status (Ready);
      return;
    }
    flag = TRUE;
  }
	for (i = from; i < to; i++)
  {
    if (loadAssignFlag == OFF)
    {
      JEOCopy (assign[i].Path, "", NAMELENGTH, NULL);
      JEOCopy (assign[i].Name, "", NAMELENGTH, NULL);
    }
    JEOWrite (rp, xTab, yTab[i], "                       ", GREY);
    if (loadAssignFlag == OFF)
    {
      assign[i].volume = 45;
      assign[i].loop = 1;
    }
    size[i]   = 0;
    freq[i]   = 0;
    sampleFlag[i] = NOT_LOADED;
    editCol[i] = BLUE;
    JEOCopy (ErrorBuffer[i], "", COMMENTLEN, ' ');
    Comment (i);
  }
  if (!loadAssignFlag)
  {
    Menus (ON);
    Gadgets (ON);
  }
  CheckRAM ();
  if (flag)
    Status (Ready);
}

VOID ShowName (WORD y, BYTE *Name, UWORD col)
{
  strcpy (Dummy, Name);
  if (strlen (Dummy) > SHOWNAMELENGTH)
  {
    Dummy[SHOWNAMELENGTH - 2] = '>';
    Dummy[SHOWNAMELENGTH - 1] = '>';
    SetAPen (rp, col);
    Move (rp, xTab, y);
    Text (rp, Dummy, SHOWNAMELENGTH);
  }
  else
    JEOWrite (rp, xTab, y, Name, col);
}

VOID Comment (UWORD nr)
{
  UWORD col = BLUE;

  if (sampleFlag[nr])
  {
    col = BLACK;
		if (sampleFlag[nr] == LOADED_WAVE)
	    sprintf (ErrorBuffer[nr], "[%2ld] [%3ld] ?:??.??", assign[nr].volume, assign[nr].loop);
		else	  
		{
		  CalcTime (nr);
	    sprintf (ErrorBuffer[nr], "[%2ld] [%3ld] %7s", assign[nr].volume, assign[nr].loop, Time[nr]);
	  }
    if (sampleFlag[nr] == LOADED_MONO)
      strcat (ErrorBuffer[nr], " Mono       ");
    else if (sampleFlag[nr] == LOADED_STEREO)
      strcat (ErrorBuffer[nr], " Stereo     ");
    else if (sampleFlag[nr] == LOADED_INSTRUMENT)
      strcat (ErrorBuffer[nr], " Instrument ");
    else if (sampleFlag[nr] == LOADED_WAVE)
	    strcat (ErrorBuffer[nr], " A wave file");
  }
  if (strlen (assign[nr].Name) == 0)
    JEOCopy (ErrorBuffer[nr], "", COMMENTLEN, ' ');
  JEOWrite (rp, ERROR_X, yTab[nr], ErrorBuffer[nr], col);
  if (editFlag == ON AND sampleFlag[nr] == NOT_LOADED AND strlen (assign[nr].Name) > 0)
    Status (ErrorBuffer[nr]);
}
