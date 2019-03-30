;/*
Delete CS.o quiet
sc5 -j73 -v -ma CS
slink LIB:c.o+"CS.o" to CS LIB LIB:sc.lib LIB:JEO.lib VERBOSE sc sd nd
copy CS.c -P:
copy CS work:c
quit
*/

#include <JEO:JEO.h>

VOID main ()
{
  ULONG checkSum = 0;
  char Licens[256], Svar[256];

  strcpy (Licens, "Licens No.: ");

  printf ("\nTast inn licensnr.: ");
  gets (Svar);
  strcat (Licens, Svar);
  checkSum = CheckSum (Licens);

  printf ("\nLicensen er \'%s\'\n", Licens);
  printf ("Checksum er \'%08lx\'\n\n", checkSum);
}
