;/*
sc5 -j73 -y -v -cfist -b1 -ma -ms PlayStereoSample
oml LIB:JEO.lib r PlayStereoSample.o
copy PlayStereoSample.c -P:
quit
*/

#include <JEO:JEO.h>
#include <proto/exec.h>
#include <exec/memory.h>
#include <devices/audio.h>
#include <proto/intuition.h>
#include <JEO:raw.h>

#define CHIPLENGTH	(100 * 1024)	/* 100 Mb CHIP ram */
#define OK		1
#define KEYPRESS	2

WORD PlayStereoSample (struct Window *w, BYTE *L_Sample, BYTE *R_Sample, ULONG length, UWORD period, UWORD volume)
{
  struct IOAudio *L_AIOptr1, *L_AIOptr2, *L_Aptr;
  struct IOAudio *R_AIOptr1, *R_AIOptr2, *R_Aptr;
  struct MsgPort *L_MsgPort, *R_MsgPort;
  struct Message *L_Message, *R_Message;
  struct IntuiMessage *Message;
  ULONG Class;
  USHORT Code;
  ULONG  L_device, R_device;
  BYTE  *L_sbase;
  BYTE  *R_sbase;
  UBYTE  chan1[] = {  1 };
  UBYTE  chan2[] = {  2 };
  ULONG ssize;
  ULONG wakebit;
  BYTE   oldpri;
  struct Task *mt;
  LONG flag = TRUE;
  WORD ret = TRUE;

  L_AIOptr1 = L_AIOptr2 = 0;
  R_AIOptr1 = R_AIOptr2 = 0;
  L_MsgPort = R_MsgPort = 0;
  L_device  = R_device  = 1;
 
  if (length <= CHIPLENGTH)
    ssize = length;
  else
    ssize = CHIPLENGTH;

  if (!(L_sbase = (BYTE *) AllocMem (ssize, MEMF_CHIP)))
  {
    ret = NOMEM;
    goto Error;
  }
  if (!(R_sbase = (BYTE *) AllocMem (ssize, MEMF_CHIP)))
  {
    ret = NOMEM;
    goto Error;
  }
  CopyMem (L_Sample, L_sbase, ssize);
  CopyMem (R_Sample, R_sbase, ssize);
  L_Sample += ssize;
  R_Sample += ssize;

  mt = FindTask (NULL);
  oldpri = SetTaskPri (mt, 10);

  if (!(L_AIOptr1 = (struct IOAudio *) 
    AllocMem (sizeof (struct IOAudio), MEMF_CHIP | MEMF_PUBLIC | MEMF_CLEAR)))
  {
    ret = NOMEM;
    goto Error;
  }
  if (!(R_AIOptr1 = (struct IOAudio *) 
    AllocMem (sizeof (struct IOAudio), MEMF_CHIP | MEMF_PUBLIC | MEMF_CLEAR)))
  {
    ret = NOMEM;
    goto Error;
  }

  if (!(L_AIOptr2 = (struct IOAudio *) 
    AllocMem (sizeof (struct IOAudio), MEMF_CHIP | MEMF_PUBLIC | MEMF_CLEAR)))
  {
    ret = NOMEM;
    goto Error;
  }
  if (!(R_AIOptr2 = (struct IOAudio *) 
    AllocMem (sizeof (struct IOAudio), MEMF_CHIP | MEMF_PUBLIC | MEMF_CLEAR)))
  {
    ret = NOMEM;
    goto Error;
  }

  if (!(L_MsgPort = CreatePort (0, 0)))
  {
    ret = PORT;
    goto Error;
  }
  if (!(R_MsgPort = CreatePort (0, 0)))
  {
    ret = PORT;
    goto Error;
  }

  /* Left channel */
  L_AIOptr1->ioa_Request.io_Command                = ADCMD_ALLOCATE;
  L_AIOptr1->ioa_Request.io_Message.mn_ReplyPort   = L_MsgPort;
  L_AIOptr1->ioa_Request.io_Message.mn_Node.ln_Pri = 127;
  L_AIOptr1->ioa_Request.io_Flags                  = ADIOF_NOWAIT;
  L_AIOptr1->ioa_AllocKey                          = 0;
  L_AIOptr1->ioa_Data                              = chan1;
  L_AIOptr1->ioa_Length                            = sizeof (chan1);

  /* Right channel */
  R_AIOptr1->ioa_Request.io_Command                = ADCMD_ALLOCATE;
  R_AIOptr1->ioa_Request.io_Message.mn_ReplyPort   = R_MsgPort;
  R_AIOptr1->ioa_Request.io_Message.mn_Node.ln_Pri = 127;
  R_AIOptr1->ioa_Request.io_Flags                  = ADIOF_NOWAIT;
  R_AIOptr1->ioa_AllocKey                          = 0;
  R_AIOptr1->ioa_Data                              = chan2;
  R_AIOptr1->ioa_Length                            = sizeof (chan2);

  L_device = OpenDevice ("audio.device", NULL, (struct IORequest *) L_AIOptr1, NULL);
  if (L_device != 0)
  {
    ret = DEV;
    goto Error;
  }
  R_device = OpenDevice ("audio.device", NULL, (struct IORequest *) R_AIOptr1, NULL);
  if (R_device != 0)
  {
    ret = DEV;
    goto Error;
  }

  L_AIOptr1->ioa_Request.io_Command = CMD_WRITE;
  L_AIOptr1->ioa_Request.io_Flags   = ADIOF_PERVOL;
  L_AIOptr1->ioa_Volume             = volume;
  L_AIOptr1->ioa_Period             = (UWORD)period;
  L_AIOptr1->ioa_Cycles             = 1;
  *L_AIOptr2 = *L_AIOptr1;
  L_AIOptr1->ioa_Data               = (BYTE *)L_sbase;
  L_AIOptr2->ioa_Data               = (BYTE *)L_sbase + (CHIPLENGTH / 2);
  L_Aptr = L_AIOptr2;

  R_AIOptr1->ioa_Request.io_Command = CMD_WRITE;
  R_AIOptr1->ioa_Request.io_Flags   = ADIOF_PERVOL;
  R_AIOptr1->ioa_Volume             = volume;
  R_AIOptr1->ioa_Period             = (UWORD)period;
  R_AIOptr1->ioa_Cycles             = 1;
  *R_AIOptr2 = *R_AIOptr1;
  R_AIOptr1->ioa_Data               = (BYTE *)R_sbase;
  R_AIOptr2->ioa_Data               = (BYTE *)R_sbase + (CHIPLENGTH / 2);
  R_Aptr = R_AIOptr2;

  if (length <= CHIPLENGTH)
  {
    L_AIOptr1->ioa_Length = length;
    R_AIOptr1->ioa_Length = length;
    BeginIO ((struct IORequest *) L_AIOptr1);
    BeginIO ((struct IORequest *) R_AIOptr1);
    wakebit = 0;

    FOREVER
    {
      wakebit = Wait ((1 << L_MsgPort->mp_SigBit) | (1 << w->UserPort->mp_SigBit));
      flag = FALSE;
      while (Message = (struct IntuiMessage *)GetMsg (w->UserPort))
      {
        Class = Message->Class;
        Code  = Message->Code;
        ReplyMsg ((struct Message *)Message);

        flag  = TRUE;
        if (Class == RAWKEY AND Code == ESC)
          flag = FALSE;
      }
      if (flag == TRUE)
        continue;
      else
        break;
    }
    L_Message = GetMsg (L_MsgPort);
    R_Message = GetMsg (R_MsgPort);
  }
  else		/* Longer than 100 Kb */
  {
    length -= CHIPLENGTH;
    L_AIOptr1->ioa_Length = (CHIPLENGTH / 2);
    L_AIOptr2->ioa_Length = (CHIPLENGTH / 2);
    R_AIOptr1->ioa_Length = (CHIPLENGTH / 2);
    R_AIOptr2->ioa_Length = (CHIPLENGTH / 2);
    BeginIO ((struct IORequest *) L_AIOptr1);
    BeginIO ((struct IORequest *) L_AIOptr2);
    BeginIO ((struct IORequest *) R_AIOptr1);
    BeginIO ((struct IORequest *) R_AIOptr2);

    while (length > 0)
    {
      FOREVER
      {
        wakebit = Wait ((1 << L_MsgPort->mp_SigBit) | (1 << w->UserPort->mp_SigBit));
        flag = OK;
        while (Message = (struct IntuiMessage *)GetMsg (w->UserPort))
        {
          Class = Message->Class;
          Code  = Message->Code;
          ReplyMsg ((struct Message *)Message);

          flag = KEYPRESS;
          if (Class == RAWKEY AND Code == ESC)
            flag = FALSE;
        }
        if (flag == KEYPRESS)
          continue;
        else
          break;
      }
      L_Message = GetMsg (L_MsgPort);
      R_Message = GetMsg (R_MsgPort);
      if (flag == FALSE)
        goto Error;

      if (L_Aptr == L_AIOptr1)
      {
        L_Aptr = L_AIOptr2;
        R_Aptr = R_AIOptr2;        
      }
      else
      {
        L_Aptr = L_AIOptr1;
        R_Aptr = R_AIOptr1;
      }

      if (length <= (CHIPLENGTH / 2))
      {
        L_Aptr->ioa_Length = length;
        R_Aptr->ioa_Length = length;
      }
      else
      {
        L_Aptr->ioa_Length = (CHIPLENGTH / 2);
        R_Aptr->ioa_Length = (CHIPLENGTH / 2);
      }

      CopyMem (L_Sample, L_Aptr->ioa_Data, L_Aptr->ioa_Length);
      CopyMem (R_Sample, R_Aptr->ioa_Data, R_Aptr->ioa_Length);

      length -= L_Aptr->ioa_Length;
      L_Sample += (CHIPLENGTH / 2);
      R_Sample += (CHIPLENGTH / 2);
      BeginIO ((struct IORequest *) L_Aptr);
      BeginIO ((struct IORequest *) R_Aptr);
    }
    FOREVER
    {
      wakebit = Wait ((1 << L_MsgPort->mp_SigBit) | (1 << w->UserPort->mp_SigBit));
      flag = OK;
      while (Message = (struct IntuiMessage *)GetMsg (w->UserPort))
      {
        Class = Message->Class;
        Code  = Message->Code;
        ReplyMsg ((struct Message *)Message);

        flag  = KEYPRESS;
        if (Class == RAWKEY AND Code == ESC)
          flag = FALSE;
      }
      if (flag == KEYPRESS)
        continue;
      else
        break;
    }
    L_Message = GetMsg (L_MsgPort);
    R_Message = GetMsg (R_MsgPort);
    if (flag == FALSE)
      goto Error;

    FOREVER
    {
      wakebit = Wait ((1 << L_MsgPort->mp_SigBit) | (1 << w->UserPort->mp_SigBit));
      flag = OK;
      while (Message = (struct IntuiMessage *)GetMsg (w->UserPort))
      {
        Class = Message->Class;
        Code  = Message->Code;
        ReplyMsg ((struct Message *)Message);

        flag  = KEYPRESS;
        if (Class == RAWKEY AND Code == ESC)
          flag = FALSE;
      }
      if (flag == KEYPRESS)
        continue;
      else
        break;
    }
    L_Message = GetMsg (L_MsgPort);
    R_Message = GetMsg (R_MsgPort);
  }

Error:
  SetTaskPri (mt, oldpri);
  if (L_sbase)    FreeMem (L_sbase, ssize);
  if (R_sbase)    FreeMem (R_sbase, ssize);
  if (!L_device)  CloseDevice ((struct IORequest *) L_AIOptr1);
  if (!R_device)  CloseDevice ((struct IORequest *) R_AIOptr1);
  if (L_MsgPort)  DeletePort (L_MsgPort);
  if (R_MsgPort)  DeletePort (R_MsgPort);
  if (L_AIOptr1)  FreeMem (L_AIOptr1, sizeof (struct IOAudio));
  if (L_AIOptr2)  FreeMem (L_AIOptr2, sizeof (struct IOAudio));
  if (R_AIOptr1)  FreeMem (R_AIOptr1, sizeof (struct IOAudio));
  if (R_AIOptr2)  FreeMem (R_AIOptr2, sizeof (struct IOAudio));
  return (ret);
}
