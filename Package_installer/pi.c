#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	if(argc<2)
	{
		printf("Usage: pi <pkg1> <pkg2>...\n");
		return 1;
	}
	char cmd[100];
	snprintf(cmd, sizeof(cmd), "pkg install %s", argv[1]);
	system(cmd);
	return 0;
}
        
