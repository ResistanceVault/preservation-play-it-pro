;/*
Delete PIPSum.o quiet
sc5 -j73 -v -O PIPSum
slink LIB:c.o+"PIPSum.o" to PIPSum LIB LIB:JEO.lib LIB:sc.lib LIB:amiga.lib VERBOSE sc sd nd
copy PIPSum.o -P:
copy PIPSum.c -P:
quit
*/

#include <JEO:JEO.h>

VOID main ()
{
  register ULONG checkSum = 0;
  register UBYTE Licens[256], Svar[256];

  strcpy (Licens, "Licens No.: ");

  printf ("\nTast inn licensnr.: ");
  gets (Svar);
  strcat (Licens, Svar);
  checkSum = CheckSum (Licens);

  printf ("\nLicensen er \'%s\'\n", Licens);
  printf ("Checksum er \'%08lx\'\n\n", checkSum);
}
