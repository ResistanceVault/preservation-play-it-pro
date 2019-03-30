/*
 *	"PLAY590.C"
 *
 *	Version v1.0 Revision 13: November 13th (!) 1990
 *
 *	(C) Copyright 1990 by Mark Everingham. All Rights Reserved.
 *
 */

/* System include files */

#include <fcntl.h>
#include <functions.h>
#include <exec/memory.h>
#include <devices/audio.h>
#include <libraries/dos.h>
#include <intuition/intuitionbase.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <workbench/icon.h>

/* Global structures & variables */

struct Unit *GetUnit();

struct FileHandle	*samplefile[2];

struct NewWindow MyNewWindow = {
	459, 0, 181, 10,
	0, 1,
	CLOSEWINDOW,
	WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE|RMBTRAP|NOCAREREFRESH,
	NULL, NULL,
	(UBYTE *) "Play590 v1.0",
	NULL, NULL,
	0, 0, 0, 0,
	WBENCHSCREEN
};
struct IntuitionBase	*IntuitionBase = NULL;
struct Window		*mywindow;
ULONG			IDCMPSigMask = 0L;

UBYTE			*audiobuffer[2][2];

UBYTE			allocSignal;
struct MsgPort 		AllocPort;
struct IOAudio		AllocIO;
UBYTE			ChannelMap[2][4] = {{1, 8, 2, 4}, {3, 5, 10, 12}};
UBYTE			ChannelMask[2] = {9, 6};

struct MsgPort		AudioPort[2][2];
UBYTE			AudioSignal[2][2];
ULONG			AudioSigMask[2][2] = {0, 0};
	
struct IOAudio		AudIO[2][2];

extern struct IconBase *IconBase;
struct DiskObject	*icon = NULL;

extern struct _dev *_devtab;

int			WorkBench;
int			Window, Speed, Files, Repeats, Stereo, Volume;
long			BufferSize;
UBYTE			OldFilter;

struct Task		*mytask;

/* Array of error messages */

#define NUM_ERRORMSGS		10

char *ErrorMessage[NUM_ERRORMSGS] = {
	"Play590 Error: Bad arguments",
	"Play590 Error: Unable to open intuition library",
	"Play590 Error: Unable to open window",
	"Play590 Error: Unable to open sample file",
	"Play590 Error: Out of memory",
	"Play590 Error: Unable to open audio device",
	"Play590 Error: Unable to open icon library",
	"Play590 Error: Illegal argument value",
	"Play590 Error: No sample files specified",
	"Play590 Useage: [run >nil:] Play590 <sample1> [sample2] [-opt 1]..[-opt n]"
};

/* Custom symbols */

#define ERROR_BADARGS		0
#define ERROR_OPENINTUITION	1
#define ERROR_OPENWINDOW	2
#define ERROR_OPENSAMPLE	3
#define ERROR_OUTOFMEM		4
#define ERROR_OPENAUDIO		5
#define ERROR_OPENICONLIB	6
#define ERROR_ILLEGALARG	7
#define ERROR_NOSAMPLES		8
#define ERROR_USEAGE		9

#define DEFAULT_SPEED		16000
#define DEFAULT_BUFFERSIZE	32768L

#define OPTION_WINDOW		'W'
#define OPTION_SPEED		'S'
#define OPTION_REPEATS		'R'
#define OPTION_BUFFER		'B'
#define OPTION_STEREO		'2'
#define OPTION_FILTER		'F'
#define OPTION_PRIORITY		'P'
#define OPTION_VOLUME		'V'

/* main() - Parse CLI/WB then load and play samples */

main(argc, argv)
	int argc;
	char *argv[];
{
	int buffer, loops = 0, status, idcmp;

	WorkBench = !argc;
	mytask = FindTask(0L);

	OldFilter = *((UBYTE *) 0xbfe001) & 2;

	if (WorkBench) {
		OpenIntuitionBase();
		ParseWorkbench((struct WBStartup *) argv);
	} else
		ParseCLI(argc, argv);

	OpenAll();

	if (!WorkBench) {
		_devtab[2].mode &= (0xffff ^ O_STDIO);
		close(0);
		close(1);
		close(2);
	}

	do {
		buffer=1;
		ResetFiles();
		ReadBuffers(0);
		SendBuffers(0);
		do {
			status = ReadBuffers(buffer);
			if (status)
				SendBuffers(buffer);
			buffer ^= 1;
			idcmp = WaitBuffers(buffer);
		} while (!idcmp && status);
		if (Repeats)
			++loops;
	} while (!idcmp && (!Repeats || loops != Repeats));
	
	CloseAll();

	exit(0);
}

/* Open system resources */

OpenAll()
{
	int channel;

	if (Window) {
		if (!IntuitionBase)
			OpenIntuitionBase();
		OpenMyWindow();
	}
	OpenAudioDevice();

	for (channel = 0; channel < Files; ++channel) {
		AllocBuffers(channel);
		OpenAudioPort(channel);
		SetupRequest(channel);
	}		
}

/* Close system resources */

CloseAll()
{
	int channel;

	for (channel = 0; channel < Files; ++channel) {
		CloseAudioPort(channel);
		FreeBuffers(channel);
		CloseFile(channel);
	}
	CloseAudioDevice();
	if (Window)
		CloseMyWindow();
	if (IntuitionBase)
		CloseLibrary(IntuitionBase);
	if (IconBase)
		CloseIconBase();

	*((UBYTE *) 0xbfe001) &= 253;
	*((UBYTE *) 0xbfe001) |= OldFilter;
}

/* Open the intuition library */

OpenIntuitionBase()
{
	IntuitionBase = (struct IntuitionBase *)
		OpenLibrary("intuition.library", 0L);
	if (!IntuitionBase)
		Quit(ERROR_OPENINTUITION);
}

/* Open Play590 title window */

OpenMyWindow()
{
	mywindow = OpenWindow(&MyNewWindow);

	if (!mywindow)
		Quit(ERROR_OPENWINDOW);

	IDCMPSigMask = 1L << mywindow->UserPort->mp_SigBit;
}

/* Close title window */

CloseMyWindow()
{
	if (mywindow)
		CloseWindow(mywindow);
}

/* Open the icon library */

OpenIconBase()
{
	IconBase = (struct IconBase *)
		OpenLibrary("icon.library", 0L);
	if (!IconBase)
		exit(ERROR_OPENICONLIB);
}

/* Open the icon library */

CloseIconBase()
{
	if (icon)
		FreeDiskObject(icon);
	if (IconBase)
		CloseLibrary(IconBase);
}

/* Open sample files/files */

OpenFiles(filename, Stereo)
	char *filename;
	int Stereo;
{
	int file;

	for (file = 0; file <= Stereo; ++file)
		if (Files < 2) {
			samplefile[Files] = Open(filename, MODE_OLDFILE);
			if (!samplefile[Files])
				Quit(ERROR_OPENSAMPLE);
			++Files;
		} else
			Quit(ERROR_BADARGS);
}

/* Close sample file */

CloseFile(channel)
	int channel;
{
	if (samplefile[channel])
		Close(samplefile[channel]);
}

/* Open port & audio.device and allocate channels */

OpenAudioDevice()
{
	allocSignal = AllocSignal(-1L);
	if (allocSignal == -1L)
		Quit(ERROR_OPENAUDIO);

	AllocPort.mp_SigTask		= mytask;
	AllocPort.mp_Flags		= PA_SIGNAL;
	AllocPort.mp_SigBit		= allocSignal;
	AllocPort.mp_Node.ln_Type	= NT_MSGPORT;
	
	AddPort(&AllocPort);

	AllocIO.ioa_Request.io_Command			= ADCMD_ALLOCATE;
	AllocIO.ioa_Request.io_Message.mn_Node.ln_Pri	= 50;
	AllocIO.ioa_Request.io_Message.mn_ReplyPort	= &AllocPort;
	AllocIO.ioa_Data				= &ChannelMap[Files - 1][0];
	AllocIO.ioa_Length				= 4L;

	if (OpenDevice("audio.device", 0L, &AllocIO, 0L))
		Quit(ERROR_OPENAUDIO);
}

/* Close audio.device and remove port */

CloseAudioDevice()
{
	if (AllocIO.ioa_Request.io_Device)
		CloseDevice(&AllocIO);

	if (AllocPort.mp_SigTask)
		RemPort(&AllocPort);

	if (allocSignal != -1L)
		FreeSignal(allocSignal);
}

/* Allocate channel double-buffers */

AllocBuffers(channel)
	int channel;
{
	int buffer;

	for (buffer = 0; buffer < 2; ++buffer) {
		audiobuffer[channel][buffer] = AllocMem(BufferSize, MEMF_PUBLIC|MEMF_CHIP);
		if (!audiobuffer[channel][buffer])
			Quit(ERROR_OUTOFMEM);
	}
}
		
/* Free memory used by buffers */

FreeBuffers(channel)
	int channel;
{
	int buffer;

	for (buffer = 0; buffer < 2; ++buffer)
		if (audiobuffer[channel][buffer])
			FreeMem(audiobuffer[channel][buffer], BufferSize);
}

/* Open a replyport for an audio channel/buffer */

OpenAudioPort(channel)
	int channel;
{
	int buffer;

	for (buffer = 0; buffer < 2; ++buffer) {
		AudioSignal[channel][buffer] = AllocSignal(-1L);
		if (AudioSignal[channel][buffer] == -1L)
			Quit(ERROR_OPENAUDIO);
		AudioSigMask[channel][buffer] = 1L << AudioSignal[channel][buffer];

		AudioPort[channel][buffer].mp_SigTask		= mytask;
		AudioPort[channel][buffer].mp_Flags		= PA_SIGNAL;
		AudioPort[channel][buffer].mp_SigBit		= AudioSignal[channel][buffer];
		AudioPort[channel][buffer].mp_Node.ln_Type	= NT_MSGPORT;
		AddPort(&AudioPort[channel][buffer]);
	}
}

/* Close audio replyport */

CloseAudioPort(channel)
	int channel;
{
	int buffer;

	for (buffer = 0; buffer < 2; ++buffer) {
		if (AudioPort[channel][buffer].mp_SigTask)
			RemPort(&AudioPort[channel][buffer]);

		if (AudioSignal[channel][buffer] != -1L)
			FreeSignal(AudioSignal[channel][buffer]);
	}
}

/* Setup AudioRequest structure ready for audio data */

SetupRequest(channel)
	int channel;
{
	int buffer;

	for (buffer = 0; buffer < 2; ++buffer) { 
		AudIO[channel][buffer].ioa_Request.io_Message.mn_ReplyPort	= &AudioPort[channel][buffer];
		AudIO[channel][buffer].ioa_Request.io_Device			= AllocIO.ioa_Request.io_Device;
		AudIO[channel][buffer].ioa_Request.io_Unit			= GetUnit(channel);
		AudIO[channel][buffer].ioa_Request.io_Command			= CMD_WRITE;
		AudIO[channel][buffer].ioa_Request.io_Flags			= ADIOF_PERVOL;
		AudIO[channel][buffer].ioa_AllocKey				= AllocIO.ioa_AllocKey;
		AudIO[channel][buffer].ioa_Data					= audiobuffer[channel][buffer];
		AudIO[channel][buffer].ioa_Length				= 0L;
		AudIO[channel][buffer].ioa_Cycles				= 1;
		AudIO[channel][buffer].ioa_Period				= (UWORD) (3579545L / Speed);
		AudIO[channel][buffer].ioa_Volume				= Volume;
	}
}

/* Get the unit number of a channel */

struct Unit *GetUnit(channel)
	int channel;
{
	ULONG units;

	units = (ULONG) AllocIO.ioa_Request.io_Unit;
	
	if (Files == 2)
		units &= ChannelMask[channel];

	return ((struct Unit *) units);
}

/* Reset the sample files to byte zero */

ResetFiles()
{
	int channel;

	for (channel = 0; channel < Files; ++channel) {
		Seek(samplefile[channel], 0L, OFFSET_BEGINNING);
		AudIO[channel][1].ioa_Length = 0L;
	}

}

/* Read one/two channel buffers from sample files */

ReadBuffers(buffer)
	int buffer;
{
	int channel;

	for (channel = 0; channel < Files; ++channel)
		AudIO[channel][buffer].ioa_Length = Read(samplefile[channel], audiobuffer[channel][buffer], BufferSize);

	return (AudIO[0][buffer].ioa_Length != 0L);
}

/* Send one/two channel buffers to audio.device */

SendBuffers(buffer)
	int buffer;
{
	int channel;

	for (channel = 0; channel < Files; ++channel)
		BeginIO(&AudIO[channel][buffer]);
}

/* Wait for a pair of audio buffers to finish playing */

WaitBuffers(buffer)
	int buffer;
{
	ULONG signals, status = 0L;

	do {
		signals = Wait(AudioSigMask[0][buffer] | AudioSigMask[1][buffer] | IDCMPSigMask);
	
		if (signals & IDCMPSigMask)
			return(TRUE);
	
		if (signals & AudioSigMask[0][buffer]) {
			GetMsg(&AudioPort[0][buffer]);
			status |= AudioSigMask[0][buffer];
		}
		if (signals & AudioSigMask[1][buffer]) {
			GetMsg(&AudioPort[1][buffer]);
			status |= AudioSigMask[1][buffer];
		}
	} while (status != (AudioSigMask[0][buffer] | AudioSigMask[1][buffer]));

	return(FALSE);
}

/* Parse the command line for sample-names and options */

ParseCLI(argc, argv)
	int argc;
	char *argv[];
{
	int arg;
	char optchar;

	if (argc == 1 || (argc == 2 && argv[1][0] == '?'))
		Quit(ERROR_USEAGE);

	if (argc < 2)
		Quit(ERROR_BADARGS);

	Window		= FALSE;
	Speed		= DEFAULT_SPEED;
	Files		= 0;
	Repeats		= 1;
	BufferSize	= DEFAULT_BUFFERSIZE;
	Stereo		= FALSE;
	Volume		= 64;

	for (arg = 1; arg < argc; ++arg)
		if (argv[arg][0] == '-') {
			optchar = toupper(argv[arg][1]);
			switch (optchar) {
				case OPTION_WINDOW:
					Window = TRUE;
					break;
				case OPTION_SPEED:
					Speed = atoi(&argv[arg][2]);
					if (Speed <= 0)
						Quit(ERROR_ILLEGALARG);
					break;
				case OPTION_REPEATS:
					Repeats = atoi(&argv[arg][2]);
					if (Repeats < 0)
						Quit(ERROR_ILLEGALARG);
					break;
				case OPTION_BUFFER:
					BufferSize = (long) atoi(&argv[arg][2]);
					if (!BufferSize)
						Quit(ERROR_ILLEGALARG);
					if (BufferSize < 0)
						BufferSize = 65536L + BufferSize;
					break;
				case OPTION_STEREO:
					Stereo = TRUE;
					break;
				case OPTION_FILTER:
					*((UBYTE *) 0xbfe001) |= 2;
					break;
				case OPTION_PRIORITY:
					SetTaskPri(mytask, (long) atoi(&argv[arg][2]));
					break;
				case OPTION_VOLUME:
					Volume = atoi(&argv[arg][2]);
					if (Volume < 0 || Volume > 64)
						Quit(ERROR_ILLEGALARG);
					break;
				default:
					Quit(ERROR_BADARGS);
			}
		} else
			OpenFiles(argv[arg], Stereo);

	if (!Files)
		Quit(ERROR_NOSAMPLES);
}

/* Null stub for standard Workbench startup code */

_wb_parse(pp, wbm)
	register struct Process *pp;
	struct WBStartup *wbm;
{
}

/* Parse the Workbench startup message for sample-names and options */

ParseWorkbench(wbmsg)
	struct WBStartup *wbmsg;
{
	struct WBArg *wbarg;
	int args, arg;
	char *filename;

	OpenIconBase();

	Window		= TRUE;
	Speed		= DEFAULT_SPEED;
	Files		= 0;
	Repeats		= 1;
	BufferSize	= DEFAULT_BUFFERSIZE;
	Stereo		= FALSE;
	Volume		= 64;

	args = wbmsg->sm_NumArgs;
	if (args > 3)
		Quit(ERROR_BADARGS);
	wbarg = wbmsg->sm_ArgList;

	for (arg = 0; arg < args; ++arg, ++wbarg)
		ParseWBArg(wbarg);

	if (!Files)
		Quit(ERROR_NOSAMPLES);
}

/* Parse an individual Workbench argument for options */

ParseWBArg(wbarg)
	struct WBArg *wbarg;
{
	struct FileLock *icondir, *olddir;
	char **toolarray;
	char *tooltype, *iconname;
	int namebar;

	icondir = (struct FileLock *) wbarg->wa_Lock;
	if (!icondir)
		return(FALSE);

	iconname = wbarg->wa_Name;
	olddir = CurrentDir(icondir);

	icon = GetDiskObject(iconname);
	if (!icon)
		return(FALSE);
	toolarray = icon->do_ToolTypes;

	if (tooltype = FindToolType(toolarray, "PRIORITY"))
		SetTaskPri(mytask, (long) atoi(tooltype));

	if (tooltype = FindToolType(toolarray, "WINDOW")) {
		if (MatchToolValue(tooltype, "ON"))
			Window = TRUE;
		else
			if (MatchToolValue(tooltype, "OFF"))
				Window = FALSE;
			else
				Quit(ERROR_ILLEGALARG);
	}
	if (tooltype = FindToolType(toolarray, "SPEED")) {
		Speed = atoi(tooltype);
		if (Speed <= 0)
			Quit(ERROR_ILLEGALARG);
	}
	if (tooltype = FindToolType(toolarray, "REPEATS")) {
		Repeats = atoi(tooltype);
		if (Repeats < 0)
			Quit(ERROR_ILLEGALARG);
	}
	if (tooltype = FindToolType(toolarray, "BUFFER")) {
		BufferSize = (long) atoi(tooltype);
		if (!BufferSize)
			Quit(ERROR_ILLEGALARG);
		if (BufferSize < 0)
			BufferSize = 65536L + BufferSize;
	}
	if (tooltype = FindToolType(toolarray, "MODE")) {
		if (MatchToolValue(tooltype, "MONO"))
			Stereo = FALSE;
		else
			if (MatchToolValue(tooltype, "STEREO"))
				Stereo = TRUE;
			else
				Quit(ERROR_ILLEGALARG);
	}
	if (tooltype = FindToolType(toolarray, "FILTER")) {
		if (MatchToolValue(tooltype, "ON"))
			*((UBYTE *) 0xbfe001) &= 253;
		else
			if (MatchToolValue(tooltype, "OFF"))
				*((UBYTE *) 0xbfe001) |= 2;
			else
				Quit(ERROR_ILLEGALARG);
	}
	if (tooltype = FindToolType(toolarray, "VOLUME")) {
		Volume = atoi(tooltype);
		if (Volume < 0 || Volume > 64)
			Quit(ERROR_ILLEGALARG);
	}

	if ((icon->do_Type == WBTOOL) && (tooltype = FindToolType(toolarray, "SAMPLE1")))
		OpenFiles(tooltype, Stereo);
	if ((icon->do_Type == WBTOOL) && (tooltype = FindToolType(toolarray, "SAMPLE2")))
		OpenFiles(tooltype, Stereo);

	if (icon->do_Type != WBTOOL)
		OpenFiles(iconname, Stereo);

	CurrentDir(olddir);
	FreeDiskObject(icon);
	icon = NULL;
}

/* Close the system resources and exit */

Quit(error)
	int error;
{
	if (WorkBench) {
		if (IntuitionBase) {
			DisplayBeep(IntuitionBase->ActiveScreen);
			SetWindowTitles(IntuitionBase->ActiveWindow, -1L, ErrorMessage[error]);
		}
	} else {
		if (_devtab[2].fd)
			Write(_devtab[2].fd, ErrorMessage[error], (long) strlen(ErrorMessage[error]));
			Write(_devtab[2].fd, "\n", 1L);
	}

	CloseAll();
	exit(RETURN_FAIL);
}
