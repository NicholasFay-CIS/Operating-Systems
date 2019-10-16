#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "command.h"
#include <dirent.h>
int STREAM;


void listDir() {
	struct dirent *dentry;
	char curr_dir[PATH_MAX];
	DIR *directory;

	getcwd(curr_dir, sizeof(curr_dir));
	directory = opendir(curr_dir);
	if(!directory) {
		//printf("ERROR: Cannot read from empty directory");
		char *error = "ERROR: Cannot read from empty directory";
		write(STREAM, error, sizeof(char)*strlen(curr_dir));
		write(STREAM, "\n", sizeof("\n"));
		return;
	}
	dentry = readdir(directory);
	do {
		char *filename = dentry->d_name;
		write(STREAM, filename, sizeof(char)*strlen(filename));
		write(STREAM, " ", sizeof(" "));

	} while(dentry = readdir(directory));
	closedir(directory);
	write(STREAM, "\n", sizeof("\n"));	
	return;	
}

void showCurrentDir() {
	char curr_dir[PATH_MAX];
	getcwd(curr_dir, sizeof(curr_dir));
	//printf("%s\n", curr_dir);
	write(STREAM, curr_dir, sizeof(char)*strlen(curr_dir));
	write(STREAM, "\n", sizeof("\n"));
	return; 
}

void makeDir(char *dirName) {
	
	struct dirent *dentry;
	char curr_dir[PATH_MAX];
	DIR *directory;

	getcwd(curr_dir, sizeof(curr_dir));
	directory = opendir(curr_dir);
	if(!directory) {
		char *error = "ERROR: Cannot read from empty directory";
		write(STREAM, error, sizeof(char)*strlen(curr_dir));
		write(STREAM, "\n", sizeof("\n"));
		//printf("ERROR: Cannot read from empty directory");
		return;
	}
	dentry = readdir(directory);
	//while(dentry = readdir(directory)) 
	do {
		if(strcmp(dirName, dentry->d_name) == 0)
		{
			char *error2 = "Error! Directory already exists";
			write(STREAM, error2, sizeof(char)*strlen(error2));
			write(STREAM, "\n", sizeof("\n"));
			//printf("Error! Directory %s already exists\n", dirName);
			closedir(directory);
			return;
		}
	} while(dentry = readdir(directory));
	closedir(directory);	
	mkdir(dirName, S_IRWXU);
	return;
}


void changeDir(char *dirName) {
	char path[PATH_MAX];	
	sprintf(path, "%s", dirName);	
	chdir(path);
	return;
}

void deleteFile(char * filepath)
{
	struct dirent *dentry;
	char curr_dir[PATH_MAX];
	DIR *directory;

	getcwd(curr_dir, sizeof(curr_dir));
	directory = opendir(curr_dir);
	if(!directory) {
		char *error = "ERROR: Cannot read from empty directory";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
		return;
		//printf("ERROR: Cannot read from empty directory");
	}
	dentry = readdir(directory); 
	do {
		if(strcmp(filepath, dentry->d_name) == 0)
		{
			unlink(filepath);
			closedir(directory);
			return;
		}
	} while(dentry = readdir(directory));
	closedir(directory);
	//printf("Error!: No such file exists.\n");
	char *error2 = "Error!: No such file exists.";
	write(STREAM, error2, sizeof(char)*strlen(error2));
	write(STREAM, "\n", sizeof("\n"));
	return;
}

void copyFile(char *sourcePath, char *destinationPath)
{
	int fd, fd2;
	int nchars;
	char buffer[PATH_MAX];
	
	fd = open(sourcePath, O_RDONLY);
	fd2 = open(destinationPath, O_WRONLY | O_CREAT, S_IRWXU);
	if (fd == -1 || fd2 == -1) {
		//printf("Error!: Issue opening file(s).\n");
		char *error = "Error!: Issue opening source file";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
		close(fd2);
		return;
	} 
	nchars = read(fd, buffer, PATH_MAX);
	do {
		write(fd2, buffer, nchars);
	} while((nchars = read(fd, buffer, PATH_MAX)) > 0);
	close(fd);
	close(fd2);
	return;
}

void displayFile(char *filename) {
	int fd;
	int nchars;
	char buffer[PATH_MAX];
	
	fd = open(filename, O_RDONLY);	
	if(fd == -1) {
		//printf("Error!: File %s cannot be opened.\n", filename);
		char *error = "Error!: File cannot be opened.";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
		return;
	}
	nchars = read(fd, buffer, PATH_MAX);
	do{
		write(1, buffer, nchars);
	} while((nchars = read(fd, buffer, PATH_MAX)) > 0);
	//printf("\n");
	write(STREAM, "\n", sizeof("\n"));
	close(fd);
}

void moveFile(char *sourcePath, char *destinationPath)
{
	int fd, fd2;
	int nchars;
	char buffer[PATH_MAX];
	fd = open(sourcePath, O_RDONLY);
	fd2 = open(destinationPath, O_WRONLY | O_CREAT, S_IRWXU);
	if (fd == -1) {
		//printf("Error!: Issue opening file(s).\n");
		char *error = "Error!: Issue opening file(s)";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
		close(fd2);
		return;
	}
	nchars = read(fd, buffer, PATH_MAX);
	do {
		write(fd2, buffer, nchars);
	} while((nchars = read(fd, buffer, PATH_MAX)) > 0);
	//rename(sourcePath, destinationPath);
	close(fd);
	close(fd2);
	deleteFile(sourcePath);
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

											
