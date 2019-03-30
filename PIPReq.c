;/*
sc5 -j73 -v -O PIPReq
quit
*/

#include "-P:Header.h"

VOID FileReq (UWORD nr)
{
  ULONG tags[] = { RTFI_Dir, (ULONG)rtDir, TAG_END };
  struct rtFileRequester *filereq;
  BYTE rtFileName[NAMELENGTH];
  BOOL flag = TRUE;

  strcpy (rtFileName, assign[nr].Name);

  while (flag)
  {
    if (filereq = rtAllocRequestA (RT_FILEREQ, NULL))
    {
      rtChangeReqAttrA (filereq, (struct TagItem *)tags);
      if (rtFileRequest (filereq, rtFileName, "Load sample", RT_ReqPos, REQPOS_CENTERSCR, RTFI_OkText, "Load", TAG_END))
      {
        if (strlen (filereq->Dir))
          strcpy (assign[nr].Path, filereq->Dir);
        else
          strcpy (assign[nr].Path, ":");

        strcpy (rtDir, filereq->Dir);

        JEOCopy (assign[nr].Name, rtFileName, NAMELENGTH, NULL);
        SetBPen (rp, BLUE);
        JEOWrite (rp, xTab, yTab[nr], "                       ", BLUE);
        ShowName (yTab[nr], assign[nr].Name, GREY);
        SetBPen (rp, GREY);
        SetBPen (rp, BLUE);
        JEOWrite (rp, xTab, yTab[nr], "                       ", BLUE);
        ShowName (yTab[nr], assign[nr].Name, GREY);
        SetBPen (rp, GREY);
        CheckRAM ();
        CheckSample (nr, nr + 1);
        flag = FALSE;
      }
      else
        flag = FALSE;

      rtFreeRequest (filereq);
    }
    else
      rtEZRequestTags ("Not enough memory!", Ok , NULL, NULL, RT_ReqPos, REQPOS_CENTERSCR, TAG_END);
  }
  CheckRAM ();
}

VOID DeleteFileReq (VOID)
{
  struct rtFileRequester *filereq;
  BYTE rtFileName[NAMELENGTH];
  BYTE Name[300];
  LONG len;
  WORD error;
  LONG i;

  Status (Please);
  rtFileName[0] = 0;
  if (filereq = rtAllocRequestA (RT_FILEREQ, NULL))
  {
    if (rtFileRequest (filereq, rtFileName, "Delete one file...",
                       RT_ReqPos, REQPOS_TOPLEFTSCR,
                       RTFI_Flags, FREQF_SAVE,
                       RTFI_OkText, "Delete", TAG_END))
    {
      strcpy (Name, filereq->Dir);
      rtFreeRequest (filereq);
      len = strlen (Name);
      if (Name[len - 1] != ':')
      {
        Name[len] = '/';
        Name[len + 1] = 0;
      }
      strcat (Name, rtFileName);
      if (Name[0] == '/')	// Vi får en error ;)
      	Name[0] = ':';
      sprintf (Buf, "Delete \"%s\"?\nAre you sure?", Name);
      if (JEOReqRequest (Message, Buf, "Delete file|Cancel"))
      {
        if (!(DeleteFile (Name)))
        {
          error = IoErr ();
          for (i = 0; i <= os_nerr; i++)
          {
            if (error == os_errlist[i].err_no)
              break;
          }
          sprintf (Buf, "Error %ld\n%s", error, os_errlist[i].msg);
          JEOReqRequest (Message, Buf, Ok);
          Status (Please);
        }
        else
        {
          sprintf (Buf, "\"%s\" is deleted.", Name);
          strcat (Name, ".info");
          DeleteFile (Name);
          Status (Buf);
        }
      }
      else
        Status (Please);
    }
    else
      Status (Please);
  }
  else
  {
    rtEZRequestTags ("Not enough memory!", Ok , NULL, NULL, RT_ReqPos, REQPOS_CENTERSCR, TAG_END);
    Status (Please);
  }
}
