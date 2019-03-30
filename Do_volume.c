;/*
sc5 -j73 -v Do_volume.c
quit
*/

#include <JEO:JEO.h>
//#include "JEO:raw.h"	
//#include <-P:PIPWindow.h>
//#include <exec/memory.h>
//#include <proto/dos.h>
#include <-p:header.h>

VOID Do_volume (UWORD tmp_nr)
{
	UBYTE value = 64, i;

	if (JEOReqRequest (Message, "Scan all presets for maximum volume adjust?", "Scan|Cancel"))
	{
		for (i = 0; i < MAXASSIGN; i++)
		{
			if (LoadAssign (i))
			{
				int s;

				for (s = 0; s < 14; s++)
				{
					if (assign[s].Name[0])	// Noe her?
					{
						if (64 - assign[s].volume < value)
							value = 64 - assign[s].volume;
					}
				}
			}
		}
		if (value > 0)	// Noe å hente?
		{
			sprintf (Dummy, "Sure you want to increse all volumes by %ld?", value);
			if (JEOReqRequest (Message, Dummy, "Increse volumes|Cancel"))
			{
				for (i = 0; i < MAXASSIGN; i++)
				{
					if (LoadAssign (i))
					{
						int s;

						for (s = 0; s < 14; s++)
						{
							if (assign[s].Name[0])	// Noe her?
							{
								assign[s].volume += value;
								SaveAssign (i);
							}
						}
					}
				}
			}
		}
		else
			JEOReqRequest (Message, "Volume is allready set to maximum (64) somewhere", Ok);
		
		Load (tmp_nr);
	}
	ClearIntuition (PIPWnd);
}
