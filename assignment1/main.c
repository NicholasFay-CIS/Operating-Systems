#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "command.c"
#include <sys/syscall.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
	int exit = 0;
	int is_cmd;
	setbuf(stdout, NULL);
	char *line = NULL;
	size_t nread;
	size_t len = 0;
	char *token;	
	const char dil[2] = " ";
	FILE *fout;
	FILE *fin;
	int filemode = 0;
	int valid = 0;	

	if(argc > 3) 
	{
		printf("Error: too many arguments given\n");
		return 1;
	} 
	if(argc == 3) 
	{
		int fd;
		fd = open(argv[2], O_RDONLY);
		if(fd != -1) 
		{
			fout = fopen("output.txt", "w");
			fin = fopen(argv[2], "r");
			filemode = 1;			
		} else {
		 	printf("Error: non text file given\n");
			return 1;
		}
	}														
	while(exit != 1)
	{
		printf(">>> ");
		getline(&line, &len, stdin);
		token = strtok(line, dil);
		while(token != NULL)
		{	
			if(strcmp(token, "exit\n") == 0) 
			{	
				exit = 1;
				break;
			}			
			if(strcmp(token, "\n") == 0) 
			{
				break;
			}
			if(strcmp(token, "ls") == 0)
			{	
				valid = isCMD(token);
				if(valid == 1) {
					char *cmd = token;
					token = strtok(NULL, dil);
					if(strcmp(token, ";\n") == 0 || strcmp(token, ";") == 0) {
						listDir();
					}
				}
			}
			if(strcmp(token, "pwd") == 0) 
			{	
				valid = isCMD(token);
				if(valid == 1) { 
					char *cmd = token;
					token = strtok(NULL, dil);
					if(strcmp(token, ";\n") == 0 || strcmp(token, ";") == 0) {
						showCurrentDir();
					}
				}
					
			}
			token = strtok(NULL, dil);
		}

	}
	if(filemode == 1) 
	{
		fclose(fin);
		fclose(fout);
	}
	free(line);
	return 1; 	 
}
