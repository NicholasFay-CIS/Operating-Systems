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
	int filemode;

	if(argc > 3) 
	{
		printf("Error: too many arguments given\n");
		return 1;
	} 
	if(argc == 3) {
		if(strcmp(argv[1], "-f") == 0) {
			int fd;
			fd = open(argv[2], O_RDONLY);
			if(fd != -1) {
				filemode = 1;
			} else {
				printf("Not able to open file. File descriptor is invalid");
			}
		} else {
			printf("Unrecognized run command.\n");
			return;
		}
	}
	if(argc == 2)
	{
		if(strcmp(argv[1], "-command") != 0)
		{
			printf("Error: input argument failure\n");
			return;
		}
		filemode = 0;
		STREAM = 1;
	} 
	if(argc == 1) {
		printf("Error!:Invalid run commands.");
		return;
	}
	if(filemode == 0) 
	{														
		while(exit != 1)
		{
			int valid = 0;
			printf(">>> ");
			getline(&line, &len, stdin);
			int i = 0;
			int count = 0;
			char *array[sizeof(line)];
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
			int cmd_p, param1_p, param2_p;
			while(array[i] != NULL) {
				char *param1;
				char *param2;
				if(i >= count) {
					break;
				}
				if(strcmp(array[i], "exit") == 0) {
					exit = 1;
					break;
				}
				if(strcmp(array[i], "\n") == 0)
				{
					break;
				}
				int is_cc;
				//check 1. get token cmd position:
				char *command = array[i];
				cmd_p = i;
				valid = isCMD(command); //A either 1 or 0 for checking command
				if(valid) {
					i++;
				}
				if(strcmp(array[i], " \0") == 0 || strcmp(array[i], "\0") == 0) {
					break;
				}
				if(!valid) {
					printf("Error!: Unrecognized comnmand: %s.\n", command);
					break;
				}
				//check 2. get token and param1 position
				 //A either 1 or 0 checking command
				if(count <= i || strcmp(array[i], "\0") == 0 || strcmp(array[i], ";") == 0) { //B either 1 or 0 for checking control code
					if(strcmp(command, "ls") == 0) {
						listDir();
					}
					if(strcmp(command, "pwd") == 0) {
						showCurrentDir();
					}
					i++;
					continue;
				}
				param1 = array[i];
				valid = isCMD(param1);
				if(valid) {
					printf("Error!: Incorrect syntax. No control code found.\n");
					break;
				}
				i++;
				//check 2. get token and param1 position
				 //A either 1 or 0 checking command
				if(strcmp(array[i], "\0") == 0 || strcmp(array[i], ";") == 0) { //B either 1 or 0 for checking control code
					if(strcmp(command, "cd") == 0) {
						changeDir(array[i-1]);
					}
					if(strcmp(command, "rm") == 0) {
						deleteFile(array[i-1]);
					}
					if(strcmp(command, "cat") == 0) {
						displayFile(array[i-1]);
					}
					if(strcmp(command, "mkdir") == 0) {
						makeDir(array[i-1]);
					}
					i++;
					continue;
				}
				param2 = array[i];
				valid = isCMD(param1);
				if(valid) {
					printf("Error!: Incorrect syntax. No control code found.\n");
					break;
				}
				if(strcmp(command, "mv") == 0) {
						moveFile(array[i-1],array[i]);
				}
				if(strcmp(command, "cp") == 0) {
						copyFile(array[i-1], array[i]);
				}
				i++;
				continue;
 			}	
		}
		free(line);
		return 1;
	}
	if(filemode == 1) {
		FILE *fin;
		int fout;
		char *line = NULL;
		ssize_t nread;
		size_t len = 0;
		fin = fopen(argv[2], "r");
		//fout = freopen("output.txt", "w+", stdout);
		fout = open("output.txt", O_WRONLY | O_CREAT, S_IRWXU);
		STREAM = fout;
		nread = getline(&line, &len, fin);
		do {
			int valid = 0;
			int i = 0;
			int count = 0;
			char *array[sizeof(line)];
			token = strtok(line, dil);
			while(token != NULL) {
				array[i] = token;
				if(array[i][strlen(array[i])-1] == '\n')
				{
					array[i][strlen(array[i])-1] = 0;
				}
				i++;
				token = strtok(NULL, dil);
			}
			array[i] = "\0";
			i = 0;
			int cmd_p, param1_p, param2_p;
			while(array[i] != NULL) {
				char *param1;
				char *param2;
				if(strcmp(array[i], "exit") == 0) {
					exit = 1;
					break;
				}
				if(strcmp(array[i], "\n") == 0)
				{
					break;
				}
				int is_cc;
				//check 1. get token cmd position:
				char *command = array[i];
				cmd_p = i;
				valid = isCMD(command); //A either 1 or 0 for checking command
				if(valid) {
					i++;
				}
				if(strcmp(array[i], "\0") == 0) {
					break;
				}
				if(!valid) {
					printf("Error!: Unrecognized syntax. \n");
					break;
				}
				//check 2. get token and param1 position
				 //A either 1 or 0 checking command
				if(array[i] == NULL) {
					is_cc = 1;
					if(strcmp(command, "ls") == 0) {
						listDir();
					}
					if(strcmp(command, "pwd") == 0) {
						showCurrentDir();
					}
					i++;
					continue;
				}
				if(strcmp(array[i], "\0") == 0 || strcmp(array[i], ";") == 0) { //B either 1 or 0 for checking control code
					if(strcmp(command, "ls") == 0) {
						listDir();
					}
					if(strcmp(command, "pwd") == 0) {
						showCurrentDir();
					}
					i++;
					continue;
				}
				param1 = array[i];
				valid = isCMD(param1);
				if(valid) {
					printf("Error!: Incorrect syntax. No control code found.\n");
					break;
				}
				i++;
				//check 2. get token and param1 position
				 //A either 1 or 0 checking command
				if(array[i] == NULL) {
					if(strcmp(command, "cd") == 0) {
						changeDir(array[i-1]);
					}
					if(strcmp(command, "rm") == 0) {
						deleteFile(array[i-1]);
					}
					if(strcmp(command, "cat") == 0) {
						displayFile(array[i-1]);
					}
					if(strcmp(command, "mkdir") == 0) {
						makeDir(array[i-1]);
					}
					i++;
					continue;
				}
				if(strcmp(array[i], "\0") == 0 || strcmp(array[i], ";") == 0) { //B either 1 or 0 for checking control code
					if(strcmp(command, "cd") == 0) {
						changeDir(array[i-1]);
					}
					if(strcmp(command, "rm") == 0) {
						deleteFile(array[i-1]);
					}
					if(strcmp(command, "cat") == 0) {
						displayFile(array[i-1]);
					}
					if(strcmp(command, "mkdir") == 0) {
						makeDir(array[i-1]);
					}
					i++;
					continue;
				}
				param2 = array[i];
				valid = isCMD(param1);
				if(valid) {
					printf("Error!: Incorrect syntax. No control code found.\n");
					break;
				}
				if(strcmp(command, "mv") == 0) {
						moveFile(array[i-1],array[i]);
				}
				if(strcmp(command, "cp") == 0) {
						copyFile(array[i-1], array[i]);
				}
				i++;
				continue;
 			}
		} while((nread = getline(&line, &len, fin) != -1));
		free(line);
		fclose(fin);
		close(fout);
	} 	 
}
