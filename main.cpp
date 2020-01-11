#include <stdio.h>
#include <string>
#include "Gantry.h"



int main(int argc, const char* argv[])
{
	Gantry gantry;
	gantry.initGantry();
	gantry.prepareCatch();
	gantry.catchCandy(2000, 0, 20000);
	printf("\nHello World\n\n");
}