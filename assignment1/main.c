/*
Description: This is the main program for the 
pseudo shell terminal. This implements the given commands 
from command.c with a interactive and file I/O option.

Author: Nicholas David Fay

Date: October 19th 2019

Notes: Please keep in mind that my main contains some errors and
all command functions work properly.


Main has some bugs in the logic. The main program is not properly able
to handle errors and some inputs. Deeper explanation exists within the report 
submitted via canvas.
*/
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
	size_t number_read;
	size_t length = 0;
	char *token;	
	const char dil[2] = " ";
	FILE *fout;
	FILE *fin;
	int filemode;

	//check for if too many arguments are given
	if(argc > 3) 
	{
		printf("Error!: too many arguments given\n");
		return 1;
	}
	//if 3 arguments are given, it is trying to be run in filemode
	if(argc == 3) {
		//filemode needs the -f flag as the first arg after ./a.out
		if(strcmp(argv[1], "-f") == 0) {
			int fd;
			//check to see if the 3rd argument is an openable file
			fd = open(argv[2], O_RDONLY);
			if(fd != -1) {
				filemode = 1;
				close(fd);
			} else {
				printf("Error!: Not able to open file.\n");
				return;
			}
		// -f was not given invalid run command
		} else {
			printf("Error!: Unrecognized run command.\n");
			return;
		}
	}
	//if two args are given it appears that it is trying to be run in interactive mode
	if(argc == 2)
	{
		//check to see if the argument after ./a.out is -command
		if(strcmp(argv[1], "-command") != 0)
		{
			printf("Error!: input argument failure\n");
			return;
		}
		// if thats the given argument set the stream to be stdout (1 for sys call write)
		filemode = 0;
		STREAM = 1;
	}
	//if no arguments given then this is an invalid run command
	if(argc == 1) {
		printf("Error!: Invalid run command.\n");
		return;
	}
	//if the program is in interactive mode
	if(filemode == 0) 
	{														
		while(exit != 1)
		{
			int i = 0;
			int count = 0;
			int valid = 0;
			printf(">>> ");
			//get the line
			getline(&line, &length, stdin);
			//create an array
			char *array[sizeof(line)];
			//tokenize the line
			token = strtok(line, dil);
			//for each token in the line add it to the array, strip any newline chars
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
			//reset i for iterating through the array
			i = 0;
			int cmd_p, param1_p, param2_p;
			while(count > i) {
				char *param1;
				char *param2;
				//if the count of tokens is the same or smaller than the ith position, go back for user input
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
				else if(strcmp(array[i], " \0") == 0 || strcmp(array[i], "\0") == 0) {
					break;
				}
				else if(!valid) {
					printf("Error!: Unrecognized comnmand: %s.\n", command);
					break;
				}
				//check 2. get token and param1 position
				 //A either 1 or 0 checking command
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
				//check 3. get token and param2 position
				 //A either 1 or 0 checking command
				if(strcmp(array[i], "\0") == 0 || strcmp(array[i], ";") == 0) { //B either 1 or 0 for checking control code
					//check for the right command
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
		ssize_t number_read;
		size_t length = 0;
		fin = fopen(argv[2], "r");
		//open the output stream for output.txt
		fout = open("output.txt", O_WRONLY | O_CREAT, S_IRWXU);
		STREAM = fout; //set the stream to be the file descriptor of output.txt
		number_read = getline(&line, &length, fin);
		//after reading the first line
		//iterate through all the lines until there is nothing left to read
		do {
			int valid = 0;
			int i = 0;
			int count = 0;
			char *array[sizeof(line)];
			token = strtok(line, dil);
			//create an array of all tokens from the given line
			while(token != NULL) {
				array[i] = token;
				//strip the newline character from the line
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
			while(count > i) {
				char *param1;
				char *param2;
				//if the count of tokens is the same or smaller than the ith position, go back for user input
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
				else if(strcmp(array[i], " \0") == 0 || strcmp(array[i], "\0") == 0) {
					break;
				}
				else if(!valid) {
					char *error = "Error!: Unrecognized comnmand: ";
					write(STREAM, error, sizeof(char)*strlen(error));
					write(STREAM, command, sizeof(char)*strlen(command));
					write(STREAM, "\n", sizeof("\n"));
					break;
				}
				//check 2. get token and param1 position
				 //A either 1 or 0 checking command
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
					char *error = "Error!: Incorrect syntax. No control code found.";
					write(STREAM, error, sizeof(char)*strlen(error));
					write(STREAM, "\n", sizeof("\n"));
					break;
				}
				i++;
				//check 3. get token and param2 position
				 //A either 1 or 0 checking command
				if(strcmp(array[i], "\0") == 0 || strcmp(array[i], ";") == 0) { //B either 1 or 0 for checking control code
					//check for the right command
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
					char *error = "Error!: Incorrect syntax. No control code found.";
					write(STREAM, error, sizeof(char)*strlen(error));
					write(STREAM, "\n", sizeof("\n"));
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
		} while((number_read = getline(&line, &length, fin) != -1));
		free(line);
		fclose(fin);
		close(fout);
	} 	 
return 1; 
}
