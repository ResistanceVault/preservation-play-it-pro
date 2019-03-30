 ;/*
Delete 16->12.o
lc -j73 -v -O -ma -ms -q5e 16->12
blink LIB:c.o+"16->12.o" to 16->12 LIB LIB:JEO.lib LIB:lc.lib LIB:amiga.lib VERBOSE sc sd nd
quit
*/

#include <JEO/JEO.h>
#include <proto/exec.h>
#include <exec/memory.h>
#include <proto/dos.h>
#include <libraries/dos.h>

ULONG size;

WORD *Buf16 = 0;
WORD *Buf12 = 0;

VOID CleanUp ()
{
  if (Buf16)  FreeMem (Buf16, size);
  if (Buf12)  FreeMem (Buf12, size);
  exit (0);
}

BOOL Load (BYTE *Name)
{
  struct FileHandle *fh;

  printf ("Henter '%s'...\n", Name);

  if ((fh = (struct FileHandle *)Open (Name, MODE_OLDFILE)) != NULL)
  {
    Read ((BPTR) fh, Buf16, size);
    Close ((BPTR) fh);
    return (TRUE);
  }
  else
    return (FALSE);
}

BOOL Save (BYTE *Name)
{
  struct FileHandle *fh;

  printf ("Lagrer '%s'...\n", Name);
  if ((fh = (struct FileHandle *)Open (Name, MODE_NEWFILE)) != NULL)
  {
    Write ((BPTR) fh, Buf12, size);
    Close ((BPTR) fh);
    return (TRUE);
  }
  else
    return (FALSE);
}

VOID Convert ()
{
  ULONG i;

  printf ("Converterer...\n");
  for (i = 0; i < size / 2; i++)
    Buf12[i] = Buf16[i] >> 4;
}

VOID main (int argc, char **argv)
{
  if (argc != 3)
  {
    printf ("Bruk: 16->12 Indfil Udfil\n");
    CleanUp ();
  }
  if ((size = FileSize (argv[1])) == -1)
  {
    printf ("Kunne ikke hente '%s'!!!\n", argv[1]);
    CleanUp ();
  }
  if (!(Buf16 = AllocMem (size, MEMF_PUBLIC)))
  {
    printf ("Ikke nok hukommelse!!!\n");
    CleanUp ();
  }
  if (!(Buf12 = AllocMem (size, MEMF_PUBLIC)))
  {
    printf ("Ikke nok hukommelse!!!\n");
    CleanUp ();
  }
  if (!(Load (argv[1])))
  {
    printf ("Kunne ikke hente '%s'!!!\n", argv[1]);
    CleanUp ();
  }
  Convert ();
  if (!(Save (argv[2])))
  {
    printf ("Kunne ikke lagre '%s'!!!\n", argv[2]);
    CleanUp ();
  }
  CleanUp ();
}
