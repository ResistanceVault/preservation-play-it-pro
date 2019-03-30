;/*
Delete IFF.o quiet
sc5 -j73 -b1 -v -y -ma -ms -q5e IFF
slink LIB:c.o+IFF.o to IFF LIB LIB:JEO.lib LIB:sc.lib LIB:amiga.lib VERBOSE sc sd nd
quit
*/

#include <proto/exec.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <proto/dos.h>
#include "JEO:JEO.h"

#define LOADED_MONO	0
#define LOADED_STEREO	1

#define IFFOK	       -1

#define NOIFF		0
#define NO8SVX		1
#define IFFCOMP		2
#define OCTERR		3
#define BADIFF		4

BYTE *IFFMsg[] =
{
  "Ikke en IFF fil!!!",
  "Ikke en 8SVX fil!!!",
  "IFF filen er kompromert!!!",
  "Oktav error!!!",
  "Forkert IFF fil!!!"
};

typedef struct
{
  BYTE  Form[4];
  LONG  formLength;
  BYTE  Vhdr[8];
  LONG  vhdrLength;
  ULONG oneShotHiSamples,
        repeatHiSamples,
        samplesPerHiCycle;
  UWORD samplesPerSec;
  UBYTE ctOctave,
        sCompression;
  LONG  volume;
  BYTE  Buffer[150];
} Voice8Header;

Voice8Header header;

BOOL sampleFlag;
LONG size;
UWORD period;
UWORD freq;
LONG L_start;

WORD CheckHeader (BYTE *Name)
{
  LONG fileLen = 0;
  LONG i = 0;
  BOOL body = FALSE;

  if (strncmp (header.Form, "FORM", 4) != 0)
    return (NOIFF);
  if (strncmp (header.Vhdr, "8SVXVHDR", 8) != 0)
    return (NO8SVX);
  if (header.sCompression != 0)
    return (IFFCOMP);
  if (header.ctOctave != 1)
    return (OCTERR);

  sampleFlag = LOADED_MONO;
  FOREVER
  {
    if (header.Buffer[i] == 'C')
      if (header.Buffer[i + 1] == 'H')
        if (header.Buffer[i + 2] == 'A')
          if (header.Buffer[i + 3] == 'N')
            sampleFlag = LOADED_STEREO;
    if (header.Buffer[i] == 'B')
      if (header.Buffer[i + 1] == 'O')
        if (header.Buffer[i + 2] == 'D')
          if (header.Buffer[i + 3] == 'Y')
          {           
            body = TRUE;
            break;
          }
    i++;
    if (i >= 300)
      break;
  }

  size = header.oneShotHiSamples + header.repeatHiSamples;

  fileLen = header.formLength + 8;
  if (sampleFlag == LOADED_STEREO)
    fileLen -= size;
  L_start = fileLen - size;
 
  printf ("\nLength = %ld\n", fileLen);
  printf ("L_start = %ld\n\n", L_start);

  if (body == TRUE AND L_start > 0)
  {
    period = PALTICKS / header.samplesPerSec;
    if (period < 124)
      period = 124;
    freq = PALTICKS / period;
    return (IFFOK);
  }
  else
    return (BADIFF);
}

BOOL LoadHeader (BYTE *Name)
{
  struct FileHandle *fh;

  if ((fh = (struct FileHandle *)Open (Name, MODE_OLDFILE)) != NULL)
  {
    Read ((BPTR) fh, &header, sizeof (Voice8Header));
    Close ((BPTR) fh);
    return (TRUE);
  }
  else
    return (FALSE);
}

BOOL LoadSample (BYTE *Name)
{
  BOOL flag;

  flag = LoadHeader (Name);
  if (flag == FALSE)
  {
    printf ("Fil ikke fundet!!!\n");
    return (FALSE);
  }
  flag = CheckHeader (Name);
  if (flag != IFFOK)
  {
    printf ("%s\n", IFFMsg[flag]);
    return (FALSE);
  }
}

main ()
{
  BYTE Name[256];

  printf ("Tast inn navn: ");
  gets (Name);
  LoadSample (Name);

  printf ("FORMLength = %ld\n", header.formLength);
  printf ("VHDRLength = %ld\n\n", header.vhdrLength);

  printf ("oneShotHiSamples  = %ld", header.oneShotHiSamples);
  printf ("\nrepeatHiSamples   = %ld", header.repeatHiSamples);
  printf ("\nsamplesPerHiCycle = %ld", header.samplesPerHiCycle);
  printf ("\nsamplesPerSec     = %ld", header.samplesPerSec);
  printf ("\nctOctave          = %ld", header.ctOctave);
  printf ("\nsCompression      = %ld", header.sCompression);
  printf ("\nvolume            = %ld\n\n", header.volume);
}
