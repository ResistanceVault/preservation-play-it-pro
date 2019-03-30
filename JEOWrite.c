



VOID  JEOWrite (struct RastPort *rp, WORD x, WORD y, BYTE *String, UWORD col);
{
  SetAPen (rp, col);
  Move (rp, x, y)
  Text (rp, String, strlen (String));
}
