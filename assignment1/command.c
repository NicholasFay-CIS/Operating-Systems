#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "command.h"
#include <dirent.h>

void listDir() {	
	struct dirent *dentry;
	char curr_dir[1000];
	DIR *directory;

	getcwd(curr_dir, sizeof(curr_dir));
	directory = opendir(curr_dir);
	if(!directory) {
		printf("ERROR: Cannot read from empty directory");
	}
	while(dentry = readdir(directory)) 
	{
		printf("%s ", dentry->d_name);
	}
	closedir(directory);	
	return;	
}

void showCurrentDir() {
	char curr_dir[PATH_MAX];
	getcwd(curr_dir, sizeof(curr_dir));
	printf("%s\n", curr_dir);
	return; 
}

void makeDir(char *dirName) {
	char *token;
	const char dil[2] = " ";
	int count = 0;
	
	token = strtok(dirName, dil);
	while(token != NULL) 
	{ 
		if(count == 1) 
		{	int i = 0;
			for(i; token[i] != '\0'; i++) {
				if(token[i] == '\n') {
					token[i] = 0;
				}
			}
			mkdir(token, S_IRWXU);
		}
		token = strtok(NULL, dil);
		count++;
	}
	return;
}


void changeDir(char *dirName) {
	char *token;
	const char *dil = " ";
	int count = 0;
	char path[300];
	
	token = strtok(dirName, dil);
	while(token != NULL)
	{	
		if(count == 1)
		{	printf("%s\n", token);
			sprintf(path, "%s", token);
			printf("%s\n", path);	
			chdir(path);
		}
		count++;
		token = strtok(NULL, dil);
	}
	return;
}


void displayFile(char *filename) {
	int fd;
	int nchars;
	char buffer[1000];
	int count = 0;
	char *token;
	
	const char* dil = " ";
	
	fd = open(filename, O_RDONLY);	
	while((nchars = read(fd, buffer, 1000)) > 0) {
		write(1, buffer, nchars);
	}
	printf("\n");
	close(fd);
}

int isCMD(char *token)
{
	char *cmd[9] = {"ls", "pwd", "mkdir", "cd", "cp", "mv", "rm", "cat" };	
	int i = 0;
	for(i; token[i] != '\0'; i++) {
		if(token[i] == '\n') {
			token[i] = 0;
		}
	}
	i = 0;
	for(i; i < 9; i++)
	{
		if(strcmp(cmd[i], token) == 0)
		{
			return 1;
		}
	}
	return 0;
}
											
