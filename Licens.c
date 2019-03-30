;/*
Delete licens.o quiet
sc5 -j73 -O -v -ma licens
slink LIB:c.o+licens.o to licens LIB LIB:sc.lib verbose sc sd nd
copy Licens Work:c
quit
*/

#include <JEO:JEO.h>

VOID main (VOID)
{
  ULONG i, checkSum = 0;
  char User[21] = "Licens No.: ";
  char LicensNo[256];

  printf ("\nTast inn Licens No.: ");
  gets (LicensNo);
  if (strlen (LicensNo) != 6)
  {
    printf ("\nLicens No. må være 6 tegn langt!!!\n\n");
    exit (0);
  }
  strcat (User, LicensNo);
  for (i = 0; i < strlen (User); i++)
    checkSum += ((User[i] ^ ((i + 5) * 5)) * ((User[i] ^ 55)));

  printf ("\nStrengen er \'%s\'\n", User);
  printf ("Checksum er \'%ld\'\n\n", checkSum);
}
