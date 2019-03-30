;/*
sc5 -j73 -v -O PIPSampleInfo
quit
*/

#include <-P:Header.h>
#include <JEO:IFF.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <-p:SI_gui.c>

int copy_size;
BOOL si_save;
ULONG tot_size = 0;
BOOL update;
char FileName[108];

VOID ActivateSIGD (UWORD nr)
{
  ActivateGadget (SampleInfoGadgets[nr], SampleInfoWnd, NULL);
}

VOID SIInitGadget (UWORD num, LONG tagtype, LONG tagvalue)
{
  GT_SetGadgetAttrs (SampleInfoGadgets[num], SampleInfoWnd, NULL, tagtype, tagvalue, TAG_DONE);
}

typedef struct
{
  char  OrigName[81],
        Copyright[81],
        Auth[81],
        Anno[81];
} TmpAssign;

TmpAssign *tmpAssign = 0;

/*
typedef struct
{
  ULONG oneShotHiSamples,
        repeatHiSamples,
        samplesPerHiCycle;
  UWORD samplesPerSec;
  UBYTE ctOctave,
        sCompression;
  Fixed volume;
} Voice8Header;
*/

BPTR si_fh, si_old;

BOOL Write_sample (char *Buffer, ULONG len)
{
	if (Write (si_fh, Buffer, len) == len)	// Ok!
	{
		tot_size += len;

		if (len % 2)	// Må vi legge til en pad byte?
		{
			Dummy[0] = 0;
			if (Write (si_fh, Dummy, 1) == 1)	// Ok!
				tot_size += 1;
			else
				return (FALSE);
		}
		return (TRUE);
	}
	else
		return (FALSE);
}

BOOL Save_sample_info (UWORD nr)
{
	Voice8Header *vh;
	ULONG size;
	UWORD len;
	char FileName[108];
	char *Body = 0;
	int filesize;
	char NewFilename[108];
	char Tmp_file[] = "T:PIP.tmp";
	BOOL ret = FALSE;
	ULONG body_size;
	ULONG header_size;
	
	tot_size = 0;

	if (!(vh = AllocMem (sizeof (Voice8Header), MEMF_CLEAR)))
		return (FALSE);

	rtSetWaitPointer (SampleInfoWnd);

	if (strlen (assign[nr].Name) > 0)
  {
		strcpy (FileName, assign[nr].Path);
		len = strlen (FileName);
		if (FileName[len - 1] != ':')
    	strcat (FileName, "/");
		strcat (FileName, assign[nr].Name);
	}

	strcpy (NewFilename, assign[nr].Path);
	len = strlen (NewFilename);
	if (NewFilename[len - 1] != ':')
   	strcat (NewFilename, "/");
	strcat (NewFilename, assign[nr].Name);
	strcat (NewFilename, ".pip");

	filesize = FileSize (FileName);
	if (filesize > 0)
	{
		if (si_old = Open (FileName, MODE_OLDFILE))
		{
		  Seek (si_old, 20, OFFSET_BEGINNING);	// Start på header
		  if (!Read (si_old, vh, sizeof (Voice8Header)))	// Leser HEADER
		  	goto end;
		}
	}
	else
		goto end;

	if (si_fh = Open (Tmp_file, MODE_NEWFILE))	// Vi lager til ram: først:
	{
		if (!Write_sample ("FORM", 4))
			goto end;
		size = 0;	// Vi skriver etter på
		if (!Write_sample ((char *)&size, 4))
			goto end;
		if (!Write_sample ("8SVXVHDR", 8))
			goto end;
		size = sizeof (Voice8Header);
		if (!Write_sample ((char *)&size, 4))
			goto end;
		
		if (!Write_sample ((char *)vh, sizeof (Voice8Header)))
			goto end;

		if (*assign[nr].OrigName)
		{
			if (!Write_sample ("NAME", 4))
				goto end;
			size = strlen (assign[nr].OrigName);
			if (!Write_sample ((char *)&size, 4))
				goto end;
			if (!Write_sample (assign[nr].OrigName, strlen (assign[nr].OrigName)))
				goto end;
		}

		if (*assign[nr].Auth)
		{
			if (!Write_sample ("AUTH", 4))
				goto end;
			size = strlen (assign[nr].Auth);
			if (!Write_sample ((char *)&size, 4))
				goto end;
			if (!Write_sample (assign[nr].Auth, strlen (assign[nr].Auth)))
				goto end;
		}

		if (*assign[nr].Copyright)
		{
			if (!Write_sample ("(c) ", 4))
				goto end;
			size = strlen (assign[nr].Copyright);
			if (!Write_sample ((char *)&size, 4))
				goto end;
			if (!Write_sample (assign[nr].Copyright, strlen (assign[nr].Copyright)))
				goto end;
		}

		if (*assign[nr].Anno)
		{
			if (!Write_sample ("ANNO", 4))
				goto end;
			size = strlen (assign[nr].Anno);
			if (!Write_sample ((char *)&size, 4))
				goto end;
			if (!Write_sample (assign[nr].Anno, strlen (assign[nr].Anno)))
				goto end;
		}

		if (sampleFlag[nr] == LOADED_STEREO)
		{
			if (!Write_sample ("CHAN", 4))
				goto end;
			size = 4;
			if (!Write_sample ((char *)&size, 4))
				goto end;
			size = STEREO;
			if (!Write_sample ((char *)&size, 4))
				goto end;
		}

		if (!Write_sample ("BODY", 4))
			goto end;
		body_size = filesize - start1[nr];	// Body size
		if (!Write_sample ((char *)&body_size, 4))
			goto end;
// ******************************* SLUTT PÅ RAMFILA! *************************
		header_size = copy_size = tot_size;
		if (!(Body = AllocMem (copy_size, MEMF_CLEAR)))
			goto end;	// Ja, vi kan tillate oss det...

	  Seek (si_fh, 0, OFFSET_BEGINNING);	// Start på header
		if (Read (si_fh, Body, copy_size) != copy_size)
			goto end;
		Close (si_fh);
		si_fh = 0;

		if (si_fh = Open (NewFilename, MODE_NEWFILE))	// Lagrer header i 1 blokk
		{
		  if (Write (si_fh, Body, copy_size) != copy_size)
		  	goto end;
		}
		else
			goto end;

		if (Body)
			FreeMem (Body, copy_size);

		copy_size = (2048 * 1024);	// Bør vel kunne gå...
		if (!(Body = AllocMem (copy_size, MEMF_CLEAR)))
		{
			copy_size = (1024 * 1024);	// Bør vel kunne gå...
			if (!(Body = AllocMem (copy_size, MEMF_CLEAR)))
			{
				copy_size = (512 * 1024);	// Bør vel kunne gå...
				if (!(Body = AllocMem (copy_size, MEMF_CLEAR)))
				{
					copy_size = (128 * 1024);	// Bør vel kunne gå...
					if (!(Body = AllocMem (copy_size, MEMF_CLEAR)))
						goto end;
				}
			}
		}

	  Seek (si_old, start1[nr], OFFSET_BEGINNING);	// Start på data
		FOREVER
		{
			size = Read (si_old, Body, copy_size);
			if (size > 0)
			{
				if (!Write_sample (Body, size))
					goto end;
				if (size != copy_size)	// Slutt på fila?
					break;
			}
			else
				break;	// Ikke mere igjen...			
		}
		size = tot_size - 8;
	  Seek (si_fh, 4, OFFSET_BEGINNING);	// Start på data
		if (!Write_sample ((char *)&size, 4))
			goto end;

		if (si_fh)
		{
			Close (si_fh);
			si_fh = 0;
		}
		if (si_old)
		{
			Close (si_old);
			si_old = 0;
		}

		if (DeleteFile (FileName))
		{
			sprintf (Dummy, "C:Rename \"%s\" TO \"%s\" >NIL:", NewFilename, FileName);
			Execute (Dummy, NULL, NULL);
			ret = TRUE;

			start1[nr] = header_size;
	    if (sampleFlag[nr] == LOADED_STEREO)
  	    start2[nr] = (body_size / 2) - header_size;
    	else
      	start2[nr] = start1[nr];

			update = TRUE;
		}
	}

end:
	if (si_fh)
		Close (si_fh);
	if (si_old)
		Close (si_old);
	if (vh)
		FreeMem (vh, sizeof (Voice8Header));
	if (Body)
		FreeMem (Body, copy_size);

	DeleteFile (Tmp_file);
	ClearPointer (SampleInfoWnd);

	return (ret);
}

BOOL SI_GadgetUp (APTR Address, UWORD nr)
{
  UWORD g_nr;
  struct Gadget *gadget;

  gadget = (struct Gadget *)Address;
  g_nr = ((struct Gadget *)Address)->GadgetID;

  switch (g_nr)
  {
		case GD_SI_NAME:
		{
			strcpy (tmpAssign->OrigName, GetString (gadget));
			if (strcmp (tmpAssign->OrigName, assign[nr].OrigName))
				si_save = TRUE;
			ActivateSIGD (GD_SI_AUTH);
			break;
		}
		case GD_SI_AUTH:
		{
			strcpy (tmpAssign->Auth, GetString (gadget));
			if (strcmp (tmpAssign->Auth, assign[nr].Auth))
				si_save = TRUE;
			ActivateSIGD (GD_SI_COPY);
			break;
		}
		case GD_SI_COPY:
		{
			strcpy (tmpAssign->Copyright, GetString (gadget));
			if (strcmp (tmpAssign->Copyright, assign[nr].Copyright))
				si_save = TRUE;
			ActivateSIGD (GD_SI_ANNO);
			break;
		}
		case GD_SI_ANNO:
		{
			strcpy (tmpAssign->Anno, GetString (gadget));
			if (strcmp (tmpAssign->Anno, assign[nr].Anno))
				si_save = TRUE;
			ActivateSIGD (GD_SI_NAME);
			break;
		}
		case GD_SI_SAVE:
		{
			if (si_save)
			{
				strcpy (assign[nr].OrigName, tmpAssign->OrigName);
				strcpy (assign[nr].Auth, tmpAssign->Auth);
				strcpy (assign[nr].Copyright, tmpAssign->Copyright);
				strcpy (assign[nr].Anno, tmpAssign->Anno);
				Status ("Saving sample...");
				if (!Save_sample_info (nr))
					JEOReqRequest (Message, "Error saving sample!", Ok);
			}
			return (TRUE);
		}
		case GD_SI_CANCEL:
			return (TRUE);
	}
	return (FALSE);
}

VOID InitSampleInfoData (UWORD nr)
{
  SIInitGadget (GD_SI_NAME, GTST_String, (ULONG)assign[nr].OrigName);
  SIInitGadget (GD_SI_AUTH, GTST_String, (ULONG)assign[nr].Auth);
  SIInitGadget (GD_SI_COPY, GTST_String, (ULONG)assign[nr].Copyright);
  SIInitGadget (GD_SI_ANNO, GTST_String, (ULONG)assign[nr].Anno);

	strcpy (tmpAssign->OrigName, assign[nr].OrigName);
	strcpy (tmpAssign->Auth, assign[nr].Auth);
	strcpy (tmpAssign->Copyright, assign[nr].Copyright);
	strcpy (tmpAssign->Anno, assign[nr].Anno);
}

VOID EditSampleInfo (UWORD nr)
{
  struct IntuiMessage *Mess;
  ULONG class;
//  USHORT Code;
  APTR Address;
  BOOL go_flag;

	if (sampleFlag[nr] == LOADED_WAVE)
	{
	  Status (Please);
		JEOReqRequest (Message, "Error: A .WAV file!", Ok);
		return;
	}

	si_save = update = FALSE;
  Status ("Edit sample info...");

	if (!(tmpAssign = AllocMem (sizeof (TmpAssign), MEMF_CLEAR)))
		return;

//	sprintf (Dummy, "         Name: %s\n       Author: %s\n    Copyright: %s\n  Annotations: %s",
//	assign[nr].OrigName, assign[nr].Auth, assign[nr].Copyright, assign[nr].Anno);
//	JEOReqRequest ("Sample info", Dummy, Ok);

  if (OpenSampleInfoWindow ())
    CleanUp ("Couldn't open window!");
	rtSetWaitPointer (PIPWnd);

	go_flag = TRUE;

	InitSampleInfoData (nr);
	ActivateSIGD (GD_SI_NAME);

  while (go_flag)
  {
		WaitPort (SampleInfoWnd->UserPort);
	  while (Mess = (struct IntuiMessage *)GetMsg (SampleInfoWnd->UserPort)) 
  	{
     	class = Mess->Class;
//	   	Code = Mess->Code;
  	 	Address = Mess->IAddress;
     	ReplyMsg ((struct Message *)Mess);

	    switch (class) 
  	  {
    	  case GADGETUP:
        	if (SI_GadgetUp (Address, nr))	// Skal vi avslutte?
        		go_flag = FALSE;
       		break;
				case	IDCMP_CLOSEWINDOW:
					go_flag = FALSE;
					break;
			}
		}
	}
	CloseSampleInfoWindow ();
  ClearPointer (PIPWnd);
	if (tmpAssign)
		FreeMem (tmpAssign, sizeof (TmpAssign));

/*
	if (update)
	{
	  JEOGadget (nr, OFF);
		Load (cur_preset);
	}
*/
}
