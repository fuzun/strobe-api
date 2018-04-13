#include <cstdio>
#include <ctime>

#include "strobe-core.h"

int main(void)
{
	strobe_core *Strobe_Core = new strobe_core();

	srand(time(NULL));
	while (1)
	{
		Strobe_Core->strobe();
		system("@cls||clear");
		Strobe_Core->debugHandler();
	}
	delete Strobe_Core;
	return 0;
}