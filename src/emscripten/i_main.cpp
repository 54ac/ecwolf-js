#include <cstdlib>
#include <stdio.h>

int main(int argc, char *argv[])
{
	extern int WL_Main(int argc, char *argv[]);
	return WL_Main(argc, argv);
}
