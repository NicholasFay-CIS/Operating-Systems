#include <stdio.h>
#include <malloc.h>

int main(int argc, char *argv[])
{
	char *array;
	int i = 0; 
	scanf("%s", &array);
	for (i; i < sizeof(array); i++) {
	
		printf("%s", &array[i]);
	}
return 0;
}
