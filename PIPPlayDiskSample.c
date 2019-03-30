;/*
sc5 -j73 -v -O PIPPlayDiskSample
quit
*/

#include <JEO:JEO.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <exec/memory.h>
#include <devices/audio.h>
#include <graphics/gfxbase.h>
#include <JEO:raw.h>
#include <-P:header.h>

BOOL AudioOpenFile (char *FileName);
BOOL AudioOpenDevice (struct Task *mt);
BOOL AudioAllocBuffers (UWORD nr);
BOOL AudioOpenPort (struct Task *mt);
VOID AudioSetupRequest (UWORD period, UWORD volume);
VOID AudioResetFiles (ULONG start1, ULONG start2, BOOL buffer);
LONG AudioReadBuffers (UWORD nr, BOOL buffer);
VOID AudioSendBuffers (BOOL buffer);
BOOL AudioWaitBuffers (struct Window *w, BOOL buffer);
VOID AudioSetup (struct Task *mt);
VOID AudioCleanUp (UWORD nr);

LONG tdevice = 1, teller = 0;

ULONG	IDCMPSigMask = 0;

UBYTE	allocSignal;
struct MsgPort 		AllocPort;
struct IOAudio		AllocIO;

BPTR sampleFH[2];
BYTE *AudioBuffer[2][2];
struct MsgPort AudioPort[2][2];
UBYTE AudioSignal[2][2];
ULONG	AudioSigMask[2][2] = { 0, 0 };
struct IOAudio AudIO[2][2];

UBYTE	chan[] = { 3 };

ULONG audio_buffersize;
ULONG audio_sec;

VOID AudioCleanUp (UWORD nr)
{
  WORD c, buffer;

  for (c = 0; c < 2; c++)
  {
    if (sampleFH[c])
      Close (sampleFH[c]);

    for (buffer = 0; buffer < 2; buffer++)
    {
      if (AudioPort[c][buffer].mp_SigTask)
        RemPort(&AudioPort[c][buffer]);

      if (AudioSignal[c][buffer] != -1)
        FreeSignal(AudioSignal[c][buffer]);

      if (AudioBuffer[c][buffer])
      {
        FreeMem(AudioBuffer[c][buffer], audio_buffersize);
        AudioBuffer[c][buffer] = 0;
      }
    }
  }

  if (AllocIO.ioa_Request.io_Device)
    CloseDevice((struct IORequest *)&AllocIO);

  if (AllocPort.mp_SigTask)
    RemPort(&AllocPort);

  if (allocSignal != -1L)
    FreeSignal(allocSignal);
}

BOOL AudioOpenFile (char *FileName)
{
  ULONG c;

  for (c = 0; c < 2; c++)
  {
    if (!(sampleFH[c] = Open (FileName, MODE_OLDFILE)))
      return (FALSE);
  }

  return (TRUE);
}

BOOL AudioOpenDevice (struct Task *mt)
{
  allocSignal = AllocSignal (-1);

  if (allocSignal == -1)
    return (FALSE);

  AllocPort.mp_SigTask		= mt;
  AllocPort.mp_Flags		= PA_SIGNAL;
  AllocPort.mp_SigBit		= allocSignal;
  AllocPort.mp_Node.ln_Type	= NT_MSGPORT;

  AddPort (&AllocPort);

  AllocIO.ioa_Request.io_Command		= ADCMD_ALLOCATE;
  AllocIO.ioa_Request.io_Message.mn_Node.ln_Pri	= 127;
  AllocIO.ioa_Request.io_Message.mn_ReplyPort	= &AllocPort;
  AllocIO.ioa_Data				= chan;
  AllocIO.ioa_Length				= 1;

  if (OpenDevice ("audio.device", 0, (struct IORequest *)&AllocIO, NULL))
    return (FALSE);

  return (TRUE);    
}

BOOL AudioAllocBuffers (UWORD nr)
{
  WORD c, buffer;

  for (c = 0; c < 2; c++)
  {
    for (buffer = 0; buffer < 2; buffer++)
    {
      if (!(AudioBuffer[c][buffer] = AllocMem (audio_buffersize, MEMF_PUBLIC | MEMF_CHIP)))
        return (FALSE);
    }
  }
  return (TRUE);
}
		
BOOL AudioOpenPort (struct Task *mt)
{
  WORD c, buffer;

  for (c = 0; c < 2; c++) 
  {
    for (buffer = 0; buffer < 2; buffer++) 
    {
      AudioSignal[c][buffer] = AllocSignal (-1);

      if (AudioSignal[c][buffer] == -1)
        return (FALSE);

      AudioSigMask[c][buffer] = 1L << AudioSignal[c][buffer];

      AudioPort[c][buffer].mp_SigTask		= mt;
      AudioPort[c][buffer].mp_Flags		= PA_SIGNAL;
      AudioPort[c][buffer].mp_SigBit		= AudioSignal[c][buffer];
      AudioPort[c][buffer].mp_Node.ln_Type	= NT_MSGPORT;
      AddPort(&AudioPort[c][buffer]);
    }
  }
  return (TRUE);
}

VOID AudioSetupRequest (UWORD period, UWORD volume)
{
  WORD c, buffer;
  ULONG unit;

  unit = 1;	/* Left */
  for (c = 0; c < 2; c++)
  {
    for (buffer = 0; buffer < 2; buffer++)
    { 
      AudIO[c][buffer].ioa_Request.io_Message.mn_ReplyPort	= &AudioPort[c][buffer];
      AudIO[c][buffer].ioa_Request.io_Device			= AllocIO.ioa_Request.io_Device;
      AudIO[c][buffer].ioa_Request.io_Unit			= (struct Unit*)unit;
      AudIO[c][buffer].ioa_Request.io_Command			= CMD_WRITE;
      AudIO[c][buffer].ioa_Request.io_Flags			= ADIOF_PERVOL;
      AudIO[c][buffer].ioa_AllocKey				= AllocIO.ioa_AllocKey;
      AudIO[c][buffer].ioa_Data					= AudioBuffer[c][buffer];
      AudIO[c][buffer].ioa_Length				= NULL;
      AudIO[c][buffer].ioa_Cycles				= 1;
      AudIO[c][buffer].ioa_Period				= period;
      AudIO[c][buffer].ioa_Volume				= volume;
    }
    unit = 2;	/* Right */
  }
}

VOID AudioResetFiles (ULONG start1, ULONG start2, BOOL buffer)
{
	BOOL tmp_buffer;

	tmp_buffer = buffer;
	tmp_buffer ^= 1;
  Seek (sampleFH[0], start1, OFFSET_BEGINNING);
  Seek (sampleFH[1], start2, OFFSET_BEGINNING);

  AudIO[0][tmp_buffer].ioa_Length = 0;	/* Just Buffer nr 2 */
  AudIO[1][tmp_buffer].ioa_Length = 0;	/* Just Buffer nr 2 */
}

LONG AudioReadBuffers (UWORD nr, BOOL buffer)
{
  WORD c;

  for (c = 0; c < 2; c++)
    AudIO[c][buffer].ioa_Length = Read (sampleFH[c], AudioBuffer[c][buffer], audio_buffersize);

  if (AudIO[0][buffer].ioa_Length != AudIO[1][buffer].ioa_Length)
    AudIO[0][buffer].ioa_Length = AudIO[1][buffer].ioa_Length;

  return (AudIO[1][buffer].ioa_Length != 0);
}

VOID AudioSendBuffers (BOOL buffer)
{
  WORD c;

  for (c = 0; c < 2; c++)
    BeginIO ((struct IORequest *)&AudIO[c][buffer]);
}

BOOL AudioWaitBuffers (struct Window *w, BOOL buffer)
{
  ULONG signals, status = NULL;
  struct IntuiMessage *Message;
  ULONG class;
  USHORT Code;
//  char Hebbe[70];

  do
  {
    signals = Wait (AudioSigMask[0][buffer] | IDCMPSigMask);
    GetMsg (&AudioPort[0][buffer]);
    GetMsg (&AudioPort[1][buffer]);

//		sprintf (Hebbe, "Sec: %ld", audio_sec++);
//		Status (Hebbe);
    if (signals & IDCMPSigMask)
    {
      while (Message = (struct IntuiMessage *)GetMsg (w->UserPort))
      {
        class = Message->Class;
        Code  = Message->Code;
        ReplyMsg ((struct Message *)Message);

        if (class == RAWKEY AND Code == ESC)
          return (TRUE);
      }
      continue;
    }
    status |= AudioSigMask[0][buffer];
  } while (status != AudioSigMask[0][buffer]);

  return (FALSE);
}

int Find_key (UWORD nr)
{
	if (nr >= 0 AND nr <= 9)	// F1-F10
		return (F1 + nr);
	else if (nr == 13)	// Numeric4 stemmer ikke inn i rekkefølgen!
		return (NUMERIC4);
	else
		return (NUMERIC1 + nr - 10);
}

WORD PlayDiskSample (UWORD nr, struct Window *w, struct Task *mt, char *FileName, ULONG buf_size, BYTE mode)
{
  struct IntuiMessage *Mess;
  ULONG class;
  USHORT code;
  WORD c, buffer;
  BOOL check_flag;
  ULONG status, idcmp;
//	char Hebbe[80];
	UWORD period;
	UWORD loop;
	UWORD key;

	loop = assign[nr].loop;
  IDCMPSigMask = 1 << w->UserPort->mp_SigBit;

	audio_sec = 0;
	if (video_mode == PAL)
	{
	  period = PAL_CLOCK_CONSTANT / freq[nr];
		if (period < 123)
			period = 123;
	}
	else
	{
	  period = NTSC_CLOCK_CONSTANT / freq[nr];
		if (period < 124)
			period = 124;
	}

	audio_buffersize = buf_size;

  if (!(AudioOpenDevice (mt)))
  {
    AudioCleanUp (nr);
    return (DEV);
  }
  if (!(AudioAllocBuffers (nr)))
  {
    AudioCleanUp (nr);
    return (NOMEM);
  }
  if (!(AudioOpenPort (mt)))
  {
    AudioCleanUp (nr);
    return (PORT);
  }
  AudioSetupRequest (period, assign[nr].volume);

  if (!(AudioOpenFile (FileName)))
  {
    AudioCleanUp (nr);
    return (FALSE);
  }

  buffer = 0;													// Første buffer
  CheckRAM ();

  AudioResetFiles (start1[nr], start2[nr], buffer);		// Finner startseek
  AudioReadBuffers (nr, buffer);					// Leser inn til buffer 0

	SetRGB4 (&(Scr->ViewPort), 0, 0x0A, 0x0A, 0x0A);
	Status ("Release key to start instant play or press <Esc> to abort...");
	if (mode == PLAY_NEW)
	 	key = Find_key (nr);
	check_flag = TRUE;
	while (check_flag)
	{
		WaitPort (w->UserPort);
	  while (Mess = (struct IntuiMessage *)GetMsg (w->UserPort)) 
	  {
	    class = Mess->Class;
	    code = Mess->Code;
	    ReplyMsg ((struct Message *)Mess);
	    switch (class) 
	    {
	      case RAWKEY:
	      {
	      	if (code == ESC)
	      	{
					  ClearIntuition (PIPWnd);
	      		goto end;
	      	}
	      	switch (mode)
	      	{
						case PLAY_NEW:
						{
							if (code == (key + RELEASED))
								check_flag = FALSE;
							break;
						}
						case PLAY_NEXT:
						{
							if (code == (RETURN + RELEASED))
								check_flag = FALSE;
							break;
						}
						case PLAY_SAME:
						{
							if (code == (SPACE + RELEASED))
								check_flag = FALSE;
							break;
						}
	      	}
	      	break;
	      }
	      case MOUSEBUTTONS:
	        check_flag = TRUE; break;
	    } 
	  }
  } 
  ClearIntuition (PIPWnd);

  sprintf (Buf, "Playing \"%s\" (%ld Hz). Loop = %ld...", assign[nr].Name, freq[nr], assign[nr].loop);
  Status (Buf);
  AudioSendBuffers (buffer);					// Spiller buffer 0
	buffer ^= 1;
  do
  {
    status = AudioReadBuffers (nr, buffer);	// Leser inn neste buffer
    if (status)
      AudioSendBuffers (buffer);				// Sender neste buffer
    else if (loop > 1 OR loop == 0)
		{
		  AudioResetFiles (start1[nr], start2[nr], buffer);		// Finner startseek
		  status = AudioReadBuffers (nr, buffer);
  		AudioSendBuffers (buffer);
			if (loop != 0)
			{
			  loop--;
			  sprintf (Buf, "Playing \"%s\" (%ld Hz). Loop = %ld...", assign[nr].Name, freq[nr], loop);
			  Status (Buf);
			}
		}	
		buffer ^= 1;
		idcmp = AudioWaitBuffers (w, buffer);	// Vi venter og sjekker etter ESC
  } while (!idcmp AND status);

end:
  for (c = 0; c < 2; c++)
  {
    if (sampleFH[c])
    {
      Close (sampleFH[c]);
      sampleFH[c] = 0;
    }
  }
  AudioCleanUp (nr);
  return (TRUE);
}
