;/*
Delete CheckIFF.o quiet
sc5 -j73 -v -ma  CheckIFF
oml LIB:JEO.lib r CheckIFF.o
copy CheckIFF.c -JL:
quit
*/

#include <JEO:JEO.h>
#include <JEO:IFF.h>
#include <proto/dos.h>

#define DEBUG 0

char IFFMsg[7][24] =
{
  "File not found!",
  "Not an IFF file!",
  "Not a 8SVX file!",
  "Samplen er compressed!",
  "Octav error!",
  "Error in IFF structure!",
  "Hz is too high for me!"
};

ULONG JEOMakeID (UBYTE *Buffer)
{
  ULONG len = 0;

  len += Buffer[0] << 24;
  len += Buffer[1] << 16;
  len += Buffer[2] << 8;
  len += Buffer[3];

  return (len);
}

VOID ReadChunk (BPTR fh, UBYTE *Buffer)
{
  UBYTE Len[4];
  ULONG len;
	char Hold[1];

  Read (fh, Len, 4);
  len = JEOMakeID (Len);
  Read (fh, &Buffer[0], len);

	if (len % 2)	// Må vi lese en pad byte?
	{
	  Read (fh, &Hold[0], 1);
		if (DEBUG)
			printf ("Reading a PAD\n");
	}
}

UWORD CheckIFF (JEO8Header *JEOHeader, char *FileName)
{
  Voice8Header header;
  BPTR fh;
  ULONG fileSize;
  BOOL body = FALSE, header_flag;
  UWORD ret;
  UBYTE ReadHold[4], Len[4];
  ULONG id;
  ULONG formLen, vhdrLen, bodyLen;

  fileSize = FileSize (FileName);
  if (fileSize == 0)
    return (NOIFF);
  else if (fileSize == ERROR)
    return (NOFILE);

  vhdrLen = bodyLen = 0;
	header_flag = FALSE;
  if ((fh = Open (FileName, MODE_OLDFILE)) != NULL)
  {
    Read (fh, ReadHold, 4);
    if (JEOMakeID (ReadHold) != FORM)
    {
	    if (JEOMakeID (ReadHold) == RIFF)
	    {
		    Read (fh, Len, 4);	// hopp over
		    Read (fh, ReadHold, 4);
		    if (JEOMakeID (ReadHold) == ID_WAVE)
		    	ret = WAVE_FILE;
	    }
	    else
	    {
	    	if (DEBUG)
	    		printf ("No IFF or WAV\n");
	      ret = NOIFF;
	    }
      goto end;
    }

    Read (fh, Len, 4);
    formLen = JEOMakeID (Len);
    if (formLen + 8 != fileSize)
    {
    	formLen = fileSize - 8;	// Tar en sjanse...
	    if (formLen + 8 != fileSize)
	    {
				if (DEBUG)
					printf ("formLen + 8 (%ld) != fileSize (%ld)\n", formLen + 8, fileSize);
    	  ret = BADIFF;
      	goto end;
      }
    }

    Read (fh, ReadHold, 4);
    if (JEOMakeID (ReadHold) != ID_8SVX)
    {
			if (DEBUG)
				printf ("No 8SVX\n");
      ret = NO8SVX;
      goto end;
    }
    while (!body)
    {
      if (Read (fh, ReadHold, 4) != 4)
      {
        ret = BADIFF;
        goto end;
      }
      id = JEOMakeID (ReadHold);
      switch (id)
      {
        case ID_ATAK:      ret = BADIFF; goto end;
        case ID_RLSE:      ret = BADIFF; goto end;

        case ID_NAME:      ReadChunk (fh, JEOHeader->Name); if (DEBUG) printf ("Found NAME '%s'\n", JEOHeader->Name); break;
        case ID_Copyright: ReadChunk (fh, JEOHeader->Copyright); if (DEBUG) printf ("Found Copyright '%s'\n", JEOHeader->Copyright); break;
        case ID_AUTH:      ReadChunk (fh, JEOHeader->Auth); if (DEBUG) printf ("Found AUTH '%s'\n", JEOHeader->Auth); break;
        case ID_ANNO:      ReadChunk (fh, JEOHeader->Anno); if (DEBUG) printf ("Found ANNO '%s'\n", JEOHeader->Anno); break;
        case ID_SEQN:
				{
					JEOHeader->seqn = TRUE;
					if (DEBUG)
					{
						printf ("Found SEQN\n");
//						ret = BADIFF;
//						goto end;
					}
					break;
				}
        case ID_CHAN:
        {
					if (DEBUG)
						printf ("Found CHAN\n"); 
          ReadChunk (fh, ReadHold);
          JEOHeader->chan = JEOMakeID (ReadHold);
          break;
        }
        case ID_VHDR:
        {
					if (DEBUG)
						printf ("Found VHDR\n"); 
          Read (fh, Len, 4);
          vhdrLen = JEOMakeID (Len);
          if (vhdrLen != sizeof (Voice8Header))
          {
						if (DEBUG)
							printf ("ERROR: VHDR\n"); 
            ret = BADIFF;
            goto end;
          }
          Read (fh, &header, sizeof (Voice8Header));
					header_flag = TRUE;
          break;
        }
        case ID_BODY:
        {
					if (DEBUG)
						printf ("Found BODY\n"); 
          body = TRUE;
          Read (fh, Len, 4);
          bodyLen = JEOMakeID (Len);
          break;
        }
      }
    }
  }
  else
    return (NOFILE);

	if (header_flag)
	{
	  if (!vhdrLen)		/* Kunne ikke finne VHDR ?? */
  	{
	    ret = BADIFF;
  	  goto end;
	  }
	  if (header.sCompression != sCmpNone)
  	{
	    ret = IFFCOMP;
  	  goto end;
	  }
	  if (header.ctOctave < 1)
  	{
    	ret = OCTERR;
	    goto end;
  	}
	  if (header.ctOctave > 1 AND JEOHeader->chan == STEREO) /* Instr. kan ikke være stereo */
	  {
	    ret = BADIFF;
	    goto end;
	  }
	  if (body)
	  {
	    fileSize = formLen + 8;
  	  JEOHeader->size = bodyLen;
   		JEOHeader->octave = header.ctOctave;
	    JEOHeader->start1 = fileSize - bodyLen;
	    if (DEBUG)
	    	printf ("start1 = %ld\n", JEOHeader->start1);
	    if (JEOHeader->chan == STEREO)
	      JEOHeader->start2 = bodyLen / 2 + JEOHeader->start1;
	    else
  	    JEOHeader->start2 = 0;

  	  JEOHeader->hz = header.samplesPerSec;
  	  JEOHeader->volume = header.volume;

	    ret = IFFOK;
  	  goto end;
	  }
  	else
	  {
  	  ret = BADIFF;
    	goto end;
	  }
 	}
  ret = BADIFF;

end:
  if (fh)
    Close (fh);

  return (ret);
}
