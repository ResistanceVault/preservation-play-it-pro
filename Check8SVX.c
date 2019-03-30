;/*
sc5 -j73 -v -O Check8SVX
slink LIB:c.o+"Check8SVX.o" to Check8SVX LIB LIB:sc.lib LIB:JEO.lib
copy Check8SVX.c -P
copy Check8SVX C:
quit
*/

#include <JEO:JEO.h>

VOID main (int argc, char **argv)
{
  register JEO8Header *header = 0;
  register WORD err;

  if (argc != 2)
  {
    printf ("\nDu må taste inn et navn!!!\n\n");
    exit (0);
  }

  if (err = CheckIFF (header, argv[1]) != IFFOK)
  {
    printf ("%s\n", IFFMsg[err]);
    exit (0);
  }

  printf ("period = %ld\n", header->period);
  printf ("stereo = %ld\n", header->stereo);
  printf ("size   = %ld\n", header->size);
  printf ("hz     = %ld\n", header->hz);
  printf ("start1 = %ld\n", header->start1);
  printf ("start2 = %ld\n\n", header->start2);

  printf ("Name      = %s\n", header->Name);
  printf ("Copyright = %s\n", header->Copyright);
  printf ("Auth      = %s\n", header->Auth);
  printf ("Anno      = %s\n", header->Anno);
}
