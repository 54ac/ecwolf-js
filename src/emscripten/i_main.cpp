#include <cstdlib>
#include <stdio.h>

int main(int argc, char *argv[])
{
	extern int WL_Main(int argc, char *argv[]);
	exit(WL_Main(argc, argv));
}
