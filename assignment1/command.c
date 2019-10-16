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
	char curr_dir[PATH_MAX];
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
	printf("\n");	
	return;	
}

void showCurrentDir() {
	char curr_dir[PATH_MAX];
	getcwd(curr_dir, sizeof(curr_dir));
	printf("%s\n", curr_dir);
	return; 
}

void makeDir(char *dirName) {
	
	struct dirent *dentry;
	char curr_dir[PATH_MAX];
	DIR *directory;

	getcwd(curr_dir, sizeof(curr_dir));
	directory = opendir(curr_dir);
	if(!directory) {
		printf("ERROR: Cannot read from empty directory");
	}
	while(dentry = readdir(directory)) 
	{
		if(strcmp(dirName, dentry->d_name) == 0)
		{
			printf("Error! Directory %s already exists\n", dirName);
			closedir(directory);
			return;
		}
	}
	closedir(directory);	
	mkdir(dirName, S_IRWXU);
	return;
}


void changeDir(char *dirName) {
	char path[300];	
	sprintf(path, "%s", dirName);	
	chdir(path);
	return;
}

void deleteFile(char * filepath)
{
	unlink(filepath);
	return;
}

void copyFile(char *sourcePath, char *destinationPath)
{
	int fd, fd2;
	int nchars;
	char buffer[PATH_MAX];
	
	fd = open(sourcePath, O_RDONLY);
	fd2 = open(destinationPath, O_WRONLY | O_CREAT, S_IRWXU);
	while((nchars = read(fd, buffer, PATH_MAX)) > 0) {
		write(fd2, buffer, nchars);
	}
	close(fd);
	close(fd2);
	return;
}

void displayFile(char *filename) {
	int fd;
	int nchars;
	char buffer[PATH_MAX];
	
	fd = open(filename, O_RDONLY);	
	while((nchars = read(fd, buffer, PATH_MAX)) > 0) {
		write(1, buffer, nchars);
	}
	printf("\n");
	close(fd);
}

void moveFile(char *sourcePath, char *destinationPath)
{
	rename(sourcePath, destinationPath);
	return;
}

int isCMD(char *token)
{
	char *cmd[9] = {"ls", "pwd", "mkdir", "cd", "cp", "mv", "rm", "cat" };	
	int i = 0;
	for(i; i < 8; i++)
	{
		if(strcmp(cmd[i], token) == 0)
		{	
			return 1;
		}
	}
	return 0;
}

											
