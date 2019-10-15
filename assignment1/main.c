#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "command.c"
#include <sys/syscall.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
	int exit = 0;
	setbuf(stdout, NULL);
	char *line = NULL;
	size_t nread;
	size_t len = 0;
	char *token;	
	const char dil[2] = " ";
	FILE *fout;
	FILE *fin;
	int filemode = 0;

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
			int valid = isCMD(token);		
			if(strcmp(token, "exit\n") == 0) 
			{	
				exit = 1;
				break;
			}			
			if(strcmp(token, "\n") == 0) 
			{
				break;
			}
			if(valid == 1) {
				if(strcmp(token, "ls") == 0)
				{	
					char *cmd = token;
					token = strtok(NULL, dil);
					if(strcmp(token, ";\n") == 0 || strcmp(token, ";") == 0) {
						listDir();
					}
					else {
						printf("Error! Unsupported arguments for command: ls\n"); 
					}
				}
				if(strcmp(token, "pwd") == 0) 
				{	 
					char *cmd = token;
					token = strtok(NULL, dil);
					if(strcmp(token, ";\n") == 0 || strcmp(token, ";") == 0) {
						showCurrentDir();
					}
				}
				if(strcmp(token, "rm") == 0)
				{
					int fd;
					token = strtok(NULL, dil);
					fd = open(token, O_RDONLY);
					if(fd != -1) {
						char *file = token;
						token = strtok(NULL, dil);
						if(strcmp(token, ";") == 0 || strcmp(token, ";\n") == 0) {
							deleteFile(file);
						}		
					}
					close(fd);
				}
				if(strcmp(token, "cd") == 0) 
				{
					token = strtok(NULL, dil);
					char *dir = token;
					if(strcmp(token, ";") != 0 || strcmp(token, ";\n") != 0)
					{	
						token = strtok(NULL, dil);
						if(token == NULL) {
							break;
						}
						else if(strcmp(token, ";") == 0 || strcmp(token, ";\n") == 0) {
							changeDir(dir);
						}
					
					}
						
				}
				if(strcmp(token, "cat") == 0)
				{
					token = strtok(NULL, dil);
					if(strcmp(token, ";") != 0 || strcmp(token, ";\n") != 0)
					{
						char *file = token;
						int fd;
						fd = open(token, O_RDONLY);
						if(fd != -1)
						{
							token = strtok(NULL, dil);
							if(strcmp(token, ";") == 0 || strcmp(token, ";\n") == 0)
							{
								displayFile(file);
							}
						}
					}
				}
				if(strcmp(token, "mkdir") == 0)
				{
					token = strtok(NULL, dil);
					char *dir = token;
					token = strtok(NULL, dil);
					if(token == NULL)
					{
						printf("Error!: Incorrect positional arguments for command: mkdir.\n");
					}
					else if(strcmp(token, ";\n") == 0){
						makeDir(dir);
					}
				}


			}else {
				printf("Error: command not found\n");
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
