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
	if(argc == 2)
	{
		if(strcmp(argv[1], "-command") != 0)
		{
			printf("Error: input argument failure\n");
			return;
		}
	}														
	while(exit != 1)
	{	
		int good = 0;
		char *param;
		char *param2;
		int valid = 0;
		printf(">>> ");
		getline(&line, &len, stdin);
		char *array[sizeof(line)];
		int i = 0;
		int count = 0;
		token = strtok(line, dil);
		while(token != NULL) {
			array[i] = token;
			if(array[i][strlen(array[i])-1] == '\n')
			{
				array[i][strlen(array[i])-1] = 0;
			}
			i++;
			count++;
			token = strtok(NULL, dil);
		}
		array[i] = "\0";
		i = 0;
		while(array[i] != "\0") {

			valid = isCMD(array[i]);
			if(strcmp(array[i], "exit") == 0 || strcmp(array[i], "exit\n") == 0) {
				exit = 1;
				break;
			}
			else if(strcmp(array[i], "\n") == 0)
			{
				break;
			}
			else if(strcmp(array[i], "ls") == 0)
			{	
				valid = isCMD(array[i+1]);
				if(strcmp(array[i+1], ";") != 0 && strcmp(array[i+1], "\0") != 0) 
				{
					printf("Error!: Unsupported arguments for command: ls\n");	
					break;
				}
				if(strcmp(array[i], "ls") == 0 && valid != 1) 
				{
					listDir();
				} else {
					printf("Error!: Unsupported arguments for command: ls\n");
					break;
				}
			}
			else if(strcmp(array[i], "pwd") == 0)
			{
				valid = isCMD(array[i+1]);
				if(strcmp(array[i+1], ";") != 0 && strcmp(array[i+1], "\0") != 0)
				{
					printf("Error!: Unsupported arguments for command: pwd\n");
					break;
				}
				if(strcmp(array[i], "pwd") == 0 && valid != 1)
				{
					showCurrentDir();
				} else {
					printf("Error!: Unsupported arguments for command: pwd\n");
					break;
				}
			}
			else if(strcmp(array[i], "cd") == 0)
			{
				valid = isCMD(array[i+1]);
				if((strcmp(array[i+2], ";") == 0 && count == 3) || count == 1 )
				{
					printf("Error!: Unsupported arguments for command: cd\n");
					break;
				}
				if((strcmp(array[i+2], "\0") == 0) || (count > 3 && strcmp(array[i+2], ";") == 0))
				{	char *param = array[i+1];
					changeDir(param);
					good = 1;
				} else {
					printf("Error!: Unsupported arguments for command: cd\n");
					break;
				}
			}
			else if(strcmp(array[i], "cat") == 0)
			{
				if((strcmp(array[i+2], ";") == 0 && count == 3) || count == 1)
				{
					printf("Error!: Unsupported arguments for command: cat\n");
					break;
				}
				if((strcmp(array[i+2], "\0") == 0) || (count > 3 && strcmp(array[i+2], ";") == 0))
				{
					char *param = array[i+1];
					displayFile(param);
					good = 1;
				} else {
					printf("Error!: Unsupported arguments for command: cat\n");
					break;
				}
			}
			
			else if(strcmp(array[i], "rm") == 0)
			{
				if((strcmp(array[i+2], ";") == 0 && count == 3) || count == 1)
				{
					printf("Error!: Unsupported arguments for command: rm\n");
					break;
				}
				if((strcmp(array[i+2], "\0") == 0) || (count > 3 && strcmp(array[i+2], ";") == 0))	
				{	char *param = array[i+1];
					deleteFile(param);
					good = 1;
				} else {
					printf("Error!: Unsupported arguments for command: rm\n");
					break;
				}
			}
			
			else if(strcmp(array[i], "mkdir") == 0)
			{
				if((strcmp(array[i+2], ";") == 0 && count == 3) || count == 1)
				{
					printf("Error!: Unsupported arguments for command: mkdir\n");
					break;
				}
				if((strcmp(array[i+2], "\0") == 0) || (count > 3 && strcmp(array[i+2], ";") == 0))
				{		
					char *param = array[i+1];
					makeDir(param);
					good = 1;
				} else {
					printf("Error!: Unsupported arguments for command: mkdir\n");
					break;
				}
			}		
			else if(strcmp(array[i], "mv") == 0)
			{
				if(strcmp(array[i+1], "\0") == 0 || strcmp(array[i+2], "\0") == 0) 
				{
					printf("Error!: Unsupported arguments for command: mv\n");
					break;
				}
				if((strcmp(array[i+3], "\0") == 0) || (count > 4 && strcmp(array[i+3], ";") == 0))
				{		
					char *param = array[i+1];
					char *param2 = array[i+2];
					moveFile(param, param2);
					good = 1;
				} else {
					printf("Error!: Unsupported arguments for command: mv\n");
					break;
				}
			}
			
			else if(strcmp(array[i], "cp") == 0)
			{
				if(strcmp(array[i+1], "\0") == 0 || strcmp(array[i+2], "\0") == 0) 
				{
					printf("Error!: Unsupported arguments for command: cp\n");
					break;
				}
				if((strcmp(array[i+3], "\0") == 0) || (count > 4 && strcmp(array[i+3], ";") == 0))
				{		
					char *param = array[i+1];
					char *param2 = array[i+2];
					moveFile(param, param2);
					good = 1;
				} else {
					printf("Error!: Unsupported arguments for command: cp\n");
					break;
				}
			}
			else if(strcmp(array[i], ";") == 0) {
				good = 0;
			}
			else {
				if(good != 1 && strcmp(array[i], ";") != 0) { 
					printf("else: %s\n", array[i]);
				}
			}
			i++;
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
