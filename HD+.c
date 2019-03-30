#include <JEO:JEO.h>
#include <fcntl.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <exec/memory.h>
#include <devices/audio.h>
#include <intuition/intuitionbase.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>

VOID OpenAll(VOID);
BOOL OpenFile (UBYTE *FileName);
BOOL OpenAudioDevice (VOID);
BOOL AllocBuffers (VOID);
BOOL OpenAudioPort (VOID);
VOID SetupRequest (VOID);
VOID ResetFiles (VOID);
LONG ReadBuffers (BOOL buffer);
VOID SendBuffers (BOOL buffer);
BOOL WaitBuffers (BOOL buffer);
VOID SetUp (VOID);
VOID CleanUp (UBYTE *String);

struct Task *mt;
BYTE   oldpri;

ULONG			IDCMPSigMask = 0;

UBYTE			allocSignal;
struct MsgPort 		AllocPort;
struct IOAudio		AllocIO;

struct FileHandle	*sampleFile[2];
BYTE			*AudioBuffer[2][2];
struct MsgPort		AudioPort[2][2];
UBYTE			AudioSignal[2][2];
ULONG			AudioSigMask[2][2] = { 0, 0 };
struct IOAudio		AudIO[2][2];

UBYTE			chan[] = { 3 };

#define BUFFERSIZE		(1024 * 100)

BOOL OpenFile (UBYTE *FileName)
{
  if ((sampleFile[0] = (struct FileHandle *)Open (FileName, MODE_OLDFILE)) == NULL)
    return (FALSE);

  if ((sampleFile[1] = (struct FileHandle *)Open (FileName, MODE_OLDFILE)) == NULL)
    return (FALSE);

  return (TRUE);
}

BOOL OpenAudioDevice (VOID)
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

BOOL AllocBuffers (VOID)
{
  register WORD c, buffer;

  for (c = 0; c < 2; c++)
  {
    for (buffer = 0; buffer < 2; buffer++)
    {
      if (!(AudioBuffer[c][buffer] = AllocMem (BUFFERSIZE, MEMF_PUBLIC | MEMF_CHIP)))
        return (FALSE);
    }
  }
  return (TRUE);
}
		
BOOL OpenAudioPort (VOID)
{
  register WORD c, buffer;

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

VOID SetupRequest (VOID)
{
  register WORD c, buffer;
  register ULONG unit;

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
      AudIO[c][buffer].ioa_Period				= 124;
      AudIO[c][buffer].ioa_Volume				= 64;
    }
    unit = 2;	/* Right */
  }
}

VOID ResetFiles ()
{
  register WORD c;

  for (c = 0; c < 2; c++)
  {
    Seek ((BPTR)sampleFile[c], 0, OFFSET_BEGINNING);
    AudIO[c][1].ioa_Length = 0;	/* Just Buffer nr 2 */
  }
}

LONG ReadBuffers (WORD buffer)
{
  register WORD c;

  for (c = 0; c < 2; c++)
    AudIO[c][buffer].ioa_Length = Read((BPTR)sampleFile[c], AudioBuffer[c][buffer], BUFFERSIZE);

  return (AudIO[0][buffer].ioa_Length != 0);
}

VOID SendBuffers (WORD buffer)
{
  register WORD c;

  for (c = 0; c < 2; c++)
    BeginIO ((struct IORequest *)&AudIO[c][buffer]);
}

BOOL WaitBuffers (WORD buffer)
{
  ULONG signals, status = NULL;

  do
  {
    signals = Wait (AudioSigMask[0][buffer]/* | IDCMPSigMask*/);
/*
    if (signals & IDCMPSigMask)
      return(TRUE);
*/
    GetMsg (&AudioPort[0][buffer]);
    GetMsg (&AudioPort[1][buffer]);

    status |= AudioSigMask[0][buffer];
  } while (status != (AudioSigMask[0][buffer]));

  return (FALSE);
}

VOID CleanUp (UBYTE *String)
{
  register WORD c, buffer;

  for (c = 0; c < 2; c++)
  {
    if (sampleFile[c])
      Close ((BPTR)sampleFile[c]);

    for (buffer = 0; buffer < 2; buffer++)
    {
      if (AudioPort[c][buffer].mp_SigTask)
        RemPort(&AudioPort[c][buffer]);

      if (AudioSignal[c][buffer] != -1)
        FreeSignal(AudioSignal[c][buffer]);

      if (AudioBuffer[c][buffer])
      {
        FreeMem(AudioBuffer[c][buffer], BUFFERSIZE);
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

  SetTaskPri (mt, oldpri);

  printf ("%s\n", String);

  exit (0);
}

VOID PlayDiskSample (UBYTE *FileName/* struct Window *mywindow */)
{
  register WORD c, buffer;
  register ULONG status, idcmp;

/*  IDCMPSigMask = 1L << mywindow->UserPort->mp_SigBit;*/

  if (!(OpenFile (FileName)))
    CleanUp ("OpenFile");

  printf ("Ready 1\n");

  buffer = 1;
  ResetFiles ();
  ReadBuffers (0);
  SendBuffers (0);
  do
  {
    status = ReadBuffers (buffer);

    if (status)
      SendBuffers (buffer);
    buffer ^= 1;
    idcmp = WaitBuffers (buffer);

  }  while (!idcmp AND status);

  for (c = 0; c < 2; c++)
  {
    if (sampleFile[c])
    {
      Close ((BPTR)sampleFile[c]);
      sampleFile[c] = 0;
    }
  }
}

VOID main ()
{
  LED (OFF);
  mt = FindTask (NULL);
  oldpri = SetTaskPri (mt, 10);

  if (!(OpenAudioDevice ()))
    CleanUp ("OpenAudioDevice");
  if (!(AllocBuffers ()))
    CleanUp ("AllocBuffers");
  if (!(OpenAudioPort ()))
    CleanUp ("OpenAudioPort");
  SetupRequest ();

  PlayDiskSample ("Data:Samples/Running");
  PlayDiskSample ("Data:Samples/Fluer");
  LED (ON);
  CleanUp ("Ok");
}
