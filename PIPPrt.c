 ;/*
sc5 -j73 -v -O PIPPrt
quit
*/

#include <-P:Header.h>
#include <devices/printer.h>
#include <devices/prtbase.h>

extern UWORD screenWidth, screenHeight;

VOID OpsetGadget (UWORD nr, BOOL flag)
{
  switch (flag)
  {
    case ON:
    {
      switch (nr)
      {
        case  1: if (!(PrinterOpset1.Flags & SELECTED))  { PrinterOpset1.Flags  ^= SELECTED; RefreshGList (&PrinterOpset1,  pw, NULL, 1); } break;
        case  2: if (!(PrinterOpset2.Flags & SELECTED))  { PrinterOpset2.Flags  ^= SELECTED; RefreshGList (&PrinterOpset2,  pw, NULL, 1); } break;
        case  3: if (!(PrinterOpset3.Flags & SELECTED))  { PrinterOpset3.Flags  ^= SELECTED; RefreshGList (&PrinterOpset3,  pw, NULL, 1); } break;
        case  4: if (!(PrinterOpset4.Flags & SELECTED))  { PrinterOpset4.Flags  ^= SELECTED; RefreshGList (&PrinterOpset4,  pw, NULL, 1); } break;
        case  5: if (!(PrinterOpset5.Flags & SELECTED))  { PrinterOpset5.Flags  ^= SELECTED; RefreshGList (&PrinterOpset5,  pw, NULL, 1); } break;
        case  6: if (!(PrinterOpset6.Flags & SELECTED))  { PrinterOpset6.Flags  ^= SELECTED; RefreshGList (&PrinterOpset6,  pw, NULL, 1); } break;
        case  7: if (!(PrinterOpset7.Flags & SELECTED))  { PrinterOpset7.Flags  ^= SELECTED; RefreshGList (&PrinterOpset7,  pw, NULL, 1); } break;
        case  8: if (!(PrinterOpset8.Flags & SELECTED))  { PrinterOpset8.Flags  ^= SELECTED; RefreshGList (&PrinterOpset8,  pw, NULL, 1); } break;
        case  9: if (!(PrinterOpset9.Flags & SELECTED))  { PrinterOpset9.Flags  ^= SELECTED; RefreshGList (&PrinterOpset9,  pw, NULL, 1); } break;
        case 10: if (!(PrinterOpset10.Flags & SELECTED)) { PrinterOpset10.Flags ^= SELECTED; RefreshGList (&PrinterOpset10, pw, NULL, 1); } break;
        case 11: if (!(PrinterOpset11.Flags & SELECTED)) { PrinterOpset11.Flags ^= SELECTED; RefreshGList (&PrinterOpset11, pw, NULL, 1); } break;
        case 12: if (!(PrinterOpset12.Flags & SELECTED)) { PrinterOpset12.Flags ^= SELECTED; RefreshGList (&PrinterOpset12, pw, NULL, 1); } break;
        case 13: if (!(PrinterOpset13.Flags & SELECTED)) { PrinterOpset13.Flags ^= SELECTED; RefreshGList (&PrinterOpset13, pw, NULL, 1); } break;
        case 14: if (!(PrinterOpset14.Flags & SELECTED)) { PrinterOpset14.Flags ^= SELECTED; RefreshGList (&PrinterOpset14, pw, NULL, 1); } break;
        case 15: if (!(PrinterOpset15.Flags & SELECTED)) { PrinterOpset15.Flags ^= SELECTED; RefreshGList (&PrinterOpset15, pw, NULL, 1); } break;
      }
      break;
    }
    case OFF:
    {
      switch (nr)
      {
        case  1: if (PrinterOpset1.Flags & SELECTED)  { PrinterOpset1.Flags  ^= SELECTED; RefreshGList (&PrinterOpset1,  pw, NULL, 1); } break;
        case  2: if (PrinterOpset2.Flags & SELECTED)  { PrinterOpset2.Flags  ^= SELECTED; RefreshGList (&PrinterOpset2,  pw, NULL, 1); } break;
        case  3: if (PrinterOpset3.Flags & SELECTED)  { PrinterOpset3.Flags  ^= SELECTED; RefreshGList (&PrinterOpset3,  pw, NULL, 1); } break;
        case  4: if (PrinterOpset4.Flags & SELECTED)  { PrinterOpset4.Flags  ^= SELECTED; RefreshGList (&PrinterOpset4,  pw, NULL, 1); } break;
        case  5: if (PrinterOpset5.Flags & SELECTED)  { PrinterOpset5.Flags  ^= SELECTED; RefreshGList (&PrinterOpset5,  pw, NULL, 1); } break;
        case  6: if (PrinterOpset6.Flags & SELECTED)  { PrinterOpset6.Flags  ^= SELECTED; RefreshGList (&PrinterOpset6,  pw, NULL, 1); } break;
        case  7: if (PrinterOpset7.Flags & SELECTED)  { PrinterOpset7.Flags  ^= SELECTED; RefreshGList (&PrinterOpset7,  pw, NULL, 1); } break;
        case  8: if (PrinterOpset8.Flags & SELECTED)  { PrinterOpset8.Flags  ^= SELECTED; RefreshGList (&PrinterOpset8,  pw, NULL, 1); } break;
        case  9: if (PrinterOpset9.Flags & SELECTED)  { PrinterOpset9.Flags  ^= SELECTED; RefreshGList (&PrinterOpset9,  pw, NULL, 1); } break;
        case 10: if (PrinterOpset10.Flags & SELECTED) { PrinterOpset10.Flags ^= SELECTED; RefreshGList (&PrinterOpset10, pw, NULL, 1); } break;
        case 11: if (PrinterOpset11.Flags & SELECTED) { PrinterOpset11.Flags ^= SELECTED; RefreshGList (&PrinterOpset11, pw, NULL, 1); } break;
        case 12: if (PrinterOpset12.Flags & SELECTED) { PrinterOpset12.Flags ^= SELECTED; RefreshGList (&PrinterOpset12, pw, NULL, 1); } break;
        case 13: if (PrinterOpset13.Flags & SELECTED) { PrinterOpset13.Flags ^= SELECTED; RefreshGList (&PrinterOpset13, pw, NULL, 1); } break;
        case 14: if (PrinterOpset14.Flags & SELECTED) { PrinterOpset14.Flags ^= SELECTED; RefreshGList (&PrinterOpset14, pw, NULL, 1); } break;
        case 15: if (PrinterOpset15.Flags & SELECTED) { PrinterOpset15.Flags ^= SELECTED; RefreshGList (&PrinterOpset15, pw, NULL, 1); } break;
      }
      break;
    }
  }
}

UWORD PrinterGadgetUp (APTR Address)
{
  UWORD nr;
  LONG i;

  nr = ((struct Gadget *)Address)->GadgetID;
  switch (nr)
  {
    case 0:			/* Udskrifts kvalitet */
    {
      if (prefs[0].pqFlag == BREV)
      {
        prefs[0].pqFlag = DATA;
        JEOWrite (prp, 252, 23, "Data", BLACK);
      }
      else
      {
        prefs[0].pqFlag = BREV;
        JEOWrite (prp, 252, 23, "Letter", BLACK);
      }
      break;
    }
    case  1: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case  2: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case  3: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case  4: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case  5: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case  6: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case  7: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case  8: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case  9: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case 10: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case 11: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case 12: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case 13: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case 14: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case 15: opsetFlag[nr] ^= 1; OpsetGadget (nr, opsetFlag[nr]); break;
    case 16:
    {
      for (i = 1; i <= MAXASSIGN; i++)
      {
        if (opsetFlag[i])		/* er allerede på */
          continue;
        OpsetGadget (i, ON);
        opsetFlag[i] = ON;
      }
      break;
    }
    case 17:
    {
      for (i = 1; i <= MAXASSIGN; i++)
      {
        if (!opsetFlag[i])		/* er allerede av */
          continue;
        OpsetGadget (i, OFF);
        opsetFlag[i] = OFF;
      }
      break;
    }
    case 18: return (PRINT); break;
    case 19: return (FALSE); break;
 }
  return (TRUE);
}

/***************************************************************************
------------------------------- Printer Stuff ------------------------------
***************************************************************************/

LONG printerDevice;

union printerIO
{
  struct IOStdReq ios;
  struct IODRPReq iodrp;
  struct IOPrtCmdReq iopc;
};

union printerIO *printerReq;
struct PrinterData *PD;
struct PrinterExtendedData *PED;
LONG oldTimeout;

union printerIO *CreatePrtReq (VOID)
{
  struct MsgPort *prtport;
  union printerIO *request;

  if (!(prtport = CreatePort (NULL, 0)))
    return (FALSE);
  if (!(request = (union printerIO*)CreateExtIO (prtport, sizeof (union printerIO))))
  {
    DeletePort (prtport);
    return (FALSE);
  }
  return (request);
}

VOID DeletePrtReq (union printerIO *request)
{
  struct MsgPort *prtport;

  prtport = request->ios.io_Message.mn_ReplyPort;
  DeleteExtIO ((struct IORequest *)request);
  DeletePort (prtport);
}

LONG OpenPrinter (union printerIO *request)
{
  printerDevice = OpenDevice ("printer.device", 0, (struct IORequest *)request, 0);
  return (printerDevice);
}

VOID CleanPrinter (VOID)
{
  if (PD)  PED->ped_TimeoutSecs = oldTimeout;
  if (!(printerDevice))  CloseDevice ((struct IORequest *)printerReq);
  if (printerReq)  DeletePrtReq (printerReq);
}

LONG PrintPrtCommand (union printerIO *request, UWORD command, UBYTE p0, UBYTE p1, UBYTE p2, UBYTE p3)
{
  request->iopc.io_Command = PRD_PRTCOMMAND;
  request->iopc.io_PrtCommand = command;
  request->iopc.io_Parm0 = p0;
  request->iopc.io_Parm1 = p1;
  request->iopc.io_Parm2 = p2;
  request->iopc.io_Parm3 = p3;
  return (LONG)(DoIO ((struct IORequest *)request));
}

LONG PrintRawString (union printerIO *request, UBYTE *buffer, ULONG count)
{
  request->ios.io_Command = PRD_RAWWRITE;
  request->ios.io_Data = (APTR)buffer;
  request->ios.io_Length = count;
  return (LONG)(DoIO ((struct IORequest *)request));
}

WORD PrintAssign (UWORD nr)
{
  BYTE FileName[NAMELENGTH * 2];
  LONG i, len = 0;
  LONG flag = FALSE;

  for (i = 0; i < max_samples; i++)
  {
    if (strlen (dummyassign[i].Name))
    {
      flag = TRUE;
      break;
    }
  }
  if (!flag)
    return (FALSE);

  if (PrintPrtCommand (printerReq, 1, 0, 0, 0, 0) != 0)	/* initalize */
    return (2);
  if (PrintPrtCommand (printerReq, prefs[0].pqFlag, 0, 0, 0, 0) != 0)/* NLQ on/off */
      return (3);
  if (PrintPrtCommand (printerReq, 8, 0, 0, 0, 0) != 0)	/* underline on */
    return (4);
  if (PrintPrtCommand (printerReq, 49, 0, 0, 0, 0) != -1)/* auto left justify */
    return (5);
  if (PrintPrtCommand (printerReq, 6, 0, 0, 0, 0) != 0)	/* italics on */
    return (6);
  if (PrintPrtCommand (printerReq, 10, 0, 0, 0, 0) != 0)/* boldface on */
    return (7);
  if (PrintRawString (printerReq, "\n\n\n\n", 4) != 0)
    return (8);
  if (PrintRawString (printerReq, AssignName[nr], strlen (AssignName[nr])) != 0)
    return (9);
  if (PrintPrtCommand (printerReq, 9, 0, 0, 0, 0) != 0)	/* underline off */
    return (10);
  if (PrintPrtCommand (printerReq, 7, 0, 0, 0, 0) != 0)	/* italics off */
    return (11);
  if (PrintRawString (printerReq, "\n\n", 2) != 0)
    return (12);
  for (i = 0; i < max_samples; i++)
  {
    strcpy (FileName, dummyassign[i].Path);
    len = strlen (FileName);
    if (len)
    {
      if (FileName[len - 1] != ':')
        strcat (FileName, "/");
    }
    if (PrintPrtCommand (printerReq, 10, 0, 0, 0, 0) != 0)	/* boldface on */
      return (13);
    if (PrintRawString (printerReq, Func[i], 3) != 0)
      return (14);
    if (PrintPrtCommand (printerReq, 11, 0, 0, 0, 0) != 0)	/* boldface off */
      return (15);
    if (PrintRawString (printerReq, "   ", 3) != 0)
      return (16);
    if (PrintRawString (printerReq, FileName, strlen (FileName)) != 0)
      return (17);
    if (PrintPrtCommand (printerReq, 10, 0, 0, 0, 0) != 0)	/* boldface on */
      return (18);
    if (PrintRawString (printerReq, dummyassign[i].Name, strlen (dummyassign[i].Name)) != 0)
      return (19);
    if (PrintRawString (printerReq, "\n", 1) != 0)
      return (20);
  }
  return (TRUE);
}

UWORD PrinterMenu (VOID)
{
  struct IntuiMessage *Message;
  ULONG class;
  USHORT Code;
  APTR Address;
  BOOL flag = TRUE;
  UWORD error = FALSE;

  Menus (OFF);
  Gadgets (OFF);
  rtSetWaitPointer (PIPWnd);

  PrinterNewWindowStructure1.Screen = Scr;

  PrinterNewWindowStructure1.LeftEdge = (screenWidth - PrinterNewWindowStructure1.Width)   / 2;
  PrinterNewWindowStructure1.TopEdge  = (screenHeight - PrinterNewWindowStructure1.Height) / 2;
  if (!(pw = OpenWindow (&PrinterNewWindowStructure1)))
  {
    error = FALSE;
    goto Error;
  }
  prp = pw->RPort;
  PrintIText (prp, &PrinterIntuiTextList1, 0, 0);
  if (prefs[0].pqFlag == BREV)
    JEOWrite (prp, 252, 23, "Letter", BLACK);
  else
    JEOWrite (prp, 252, 23, "Data", BLACK);
  CheckRAM ();
  ScreenToFront (Scr);

  while (flag)
  {
    WaitPort (pw->UserPort);
    while (Message = (struct IntuiMessage *)GetMsg (pw->UserPort)) 
    {
      class = Message->Class;
      Code = Message->Code;
      Address = Message->IAddress;

      ReplyMsg ((struct Message *)Message);
      switch (class) 
      {
        case GADGETUP:
          error = PrinterGadgetUp (Address);
          if (error == FALSE OR error == PRINT)
            flag = FALSE;
          break;
        case CLOSEWINDOW:
          flag = FALSE;
          break;
        case RAWKEY:
        {
          switch (Code)
          {
            case ESC:
              flag = FALSE;
              break;
          }
          break;
        }
      }
    }
  }
  CloseWindow (pw);
Error:
  return (error);
}

UWORD Print (VOID)
{
  LONG i = 0;
  WORD lf = 0;
  BOOL flag = FALSE;
  UWORD error = FALSE;

  Status (Please);
  error = PrinterMenu ();
  if (error != PRINT)
    return (NOPRINT);

  for (i = 1; i <= MAXASSIGN; i++)	/* ingen valgt!?! return */
  {
    if (opsetFlag[i])
    {
      flag = TRUE;
      break;
    }
  }
  if (!flag)  
    return (NOPRINT);

  flag = ON;
  while (flag)
  {
    switch (PrinterStatus ())
    {
      case OFF_LINE:
      {
        if (rtEZRequestTags ("Printer er ikke i 'ON LINE' stilling!!!", 
        "Prøv igjen|Avbryt" , NULL, NULL,
        RT_ReqPos, REQPOS_CENTERSCR, TAG_END))
          break;
        else
          return (NOPRINT);
      }
      case PAPER_OUT:
      {
        if (rtEZRequestTags ("Printer har ikke papir\neller printer er ikke tent!!!", 
        "Prøv igjen|Avbryt" , NULL, NULL,
        RT_ReqPos, REQPOS_CENTERSCR, TAG_END))
          break;
        else
          return (NOPRINT);
      }
      default:  flag = OFF;
    }
  }

  if (printerReq = CreatePrtReq ())
  {
    if (!(OpenPrinter (printerReq)));
    else
    {
      CleanPrinter ();
      return (FALSE);
    }
  }      
  else
  {
    CleanPrinter ();
    return (FALSE);
  }

  PD = (struct PrinterData *)printerReq->iodrp.io_Device;
  PED = &PD->pd_SegmentData->ps_PED;
  oldTimeout = PED->ped_TimeoutSecs;
  PED->ped_TimeoutSecs = 1;

  Status ("Printer...");
  flag = FALSE;
  for (i = 0; i < MAXASSIGN; i++)
  {
    flag = FALSE;
    if (!opsetFlag[i + 1])	/* ikke valgt */
      continue;
    if (!(LoadDummyAssign (i)))
      continue;
    if (opsetFlag[i + 1])
      flag = PrintAssign (i);
    if (!flag)			/* ingen samples */
      continue;
    if (flag >= 2)		/* error nr */
    {
      CleanPrinter ();
      return (FALSE);
    }
    lf++;
    if (lf == 3 OR lf == 6 OR lf == 9 OR lf == 12 OR lf == 15)
    {
      if (PrintRawString (printerReq, "\n\n\n\n\n\n\n\n\n\n\n\n", 12) != 0)
      {
        CleanPrinter ();
        return (FALSE);
      }
    }
  }
  CleanPrinter ();
  return (TRUE);
}
