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
VOID ResetFile (VOID);
LONG ReadBuffer (BOOL buffer);
VOID SendBuffer (BOOL buffer);
BOOL WaitBuffer (BOOL buffer);
VOID SetUp (VOID);
VOID CleanUp (UBYTE *String);

struct Task *mt;
BYTE   oldpri;

struct FileHandle	*sampleFile;
ULONG			IDCMPSigMask = 0;
BYTE			*AudioBuffer[2];
UBYTE			allocSignal;
struct MsgPort 		AllocPort;
struct IOAudio		AllocIO;
UBYTE			chan1[] = { 1 };
UBYTE			chan2[] = { 2 };
struct MsgPort		AudioPort[2];
UBYTE			AudioSignal[2];
ULONG			AudioSigMask[2] = { 0, 0 };
	
struct IOAudio		AudIO[2];

#define BUFFERSIZE		(1024 * 100)

BOOL OpenFile (UBYTE *FileName)
{
  if ((sampleFile = (struct FileHandle *)Open (FileName, MODE_OLDFILE)) == NULL)
    return (FALSE);
  else
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
  AllocIO.ioa_Request.io_Message.mn_Node.ln_Pri	= 50;
  AllocIO.ioa_Request.io_Message.mn_ReplyPort	= &AllocPort;
  AllocIO.ioa_Data				= chan1;
  AllocIO.ioa_Length				= 1;

  if (OpenDevice ("audio.device", 0, (struct IORequest *)&AllocIO, NULL))
    return (FALSE);

  return (TRUE);    
}

BOOL AllocBuffers (VOID)
{
  BOOL buffer;

  for (buffer = 0; buffer < 2; buffer++)
  {
    if (!(AudioBuffer[buffer] = AllocMem (BUFFERSIZE, MEMF_PUBLIC | MEMF_CHIP)))
      return (FALSE);
  }
  return (TRUE);
}
		
/* Open a replyport for an audio channel/buffer */

BOOL OpenAudioPort (VOID)
{
  BOOL buffer;

  for (buffer = 0; buffer < 2; buffer++) 
  {
    AudioSignal[buffer] = AllocSignal (-1);

    if (AudioSignal[buffer] == -1)
      return (FALSE);

    AudioSigMask[buffer] = 1L << AudioSignal[buffer];

    AudioPort[buffer].mp_SigTask	= mt;
    AudioPort[buffer].mp_Flags		= PA_SIGNAL;
    AudioPort[buffer].mp_SigBit		= AudioSignal[buffer];
    AudioPort[buffer].mp_Node.ln_Type	= NT_MSGPORT;
    AddPort(&AudioPort[buffer]);
  }
  return (TRUE);
}

VOID SetupRequest (UWORD channel)
{
  BOOL buffer;
/*  ULONG unit = 1;*/

  for (buffer = 0; buffer < 2; buffer++)
  { 
    AudIO[buffer].ioa_Request.io_Command		= CMD_WRITE;
    AudIO[buffer].ioa_Request.io_Flags			= ADIOF_PERVOL;
    AudIO[buffer].ioa_Request.io_Message.mn_ReplyPort	= &AudioPort[buffer];
    AudIO[buffer].ioa_Request.io_Device			= AllocIO.ioa_Request.io_Device;
    AudIO[buffer].ioa_Request.io_Unit			= (struct Unit*)1;
    AudIO[buffer].ioa_AllocKey				= AllocIO.ioa_AllocKey;
    AudIO[buffer].ioa_Data				= AudioBuffer[buffer];
    AudIO[buffer].ioa_Length				= NULL;
    AudIO[buffer].ioa_Cycles				= 1;
    AudIO[buffer].ioa_Period				= 124;
    AudIO[buffer].ioa_Volume				= 64;
  }
}

VOID ResetFile ()
{
  Seek ((BPTR)sampleFile, 0, OFFSET_BEGINNING);
  AudIO[1].ioa_Length = 0;
}

LONG ReadBuffer (BOOL buffer)
{
  AudIO[buffer].ioa_Length = Read((BPTR)sampleFile, AudioBuffer[buffer], BUFFERSIZE);

  printf ("len = %ld\n", AudIO[buffer].ioa_Length);
  return (AudIO[buffer].ioa_Length != 0);
}

VOID SendBuffer (BOOL buffer)
{
  BeginIO ((struct IORequest *)&AudIO[buffer]);
}

BOOL WaitBuffer (BOOL buffer)
{
  ULONG signals, status = NULL;

  do
  {
    signals = Wait (AudioSigMask[buffer] /* | IDCMPSigMask*/);
/*
    if (signals & IDCMPSigMask)
      return (TRUE);
*/
    if (signals & AudioSigMask[buffer])
    {
      GetMsg (&AudioPort[buffer]);
      status |= AudioSigMask[buffer];
    }
  } while (status != (AudioSigMask[buffer]));

  return (FALSE);
}

/* Parse the command line for sample-names and options */

VOID CleanUp (UBYTE *String)
{
  BOOL buffer;

  if (sampleFile)
    Close ((BPTR)sampleFile);

  for (buffer = 0; buffer < 2; buffer++)
  {
    if (AudioPort[buffer].mp_SigTask)
      RemPort(&AudioPort[buffer]);

    if (AudioSignal[buffer] != -1L)
      FreeSignal(AudioSignal[buffer]);

    if (AudioBuffer[buffer])
      FreeMem(AudioBuffer[buffer], BUFFERSIZE);
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
  register BOOL buffer;
  register ULONG status, idcmp;

/*  IDCMPSigMask = 1L << mywindow->UserPort->mp_SigBit;*/

  if (!(OpenFile (FileName)))
    CleanUp ("OpenFile");

  printf ("Ready 1\n");

  buffer = 1;
  ResetFile ();
  ReadBuffer (0);
  SendBuffer (0);
  do
  {
    status = ReadBuffer (buffer);

    if (status)
      SendBuffer (buffer);
    buffer ^= 1;
    idcmp = WaitBuffer (buffer);

  }  while (!idcmp AND status);

  if (sampleFile)
  {
    Close ((BPTR)sampleFile);
    sampleFile = 0;
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
