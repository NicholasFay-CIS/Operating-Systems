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
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "command.h"
#include <dirent.h>
//shared variable to change the stream
int STREAM;

void listDir() {
	struct dirent *dentry;
	char cwd[PATH_MAX];
	DIR *directory;
	//get current working directory
	getcwd(cwd, sizeof(cwd));
	//open the directory
	directory = opendir(cwd);
	//if the directory cannot be opened error
	directory = NULL;
	if(!directory) {
		char *error = "Error!: Cannot read from empty directory";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
		return;
	}
	dentry = readdir(directory);
	//if the directory can be opened. Iterate through the contents of the directory and print them
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
	char cwd[PATH_MAX];
	//since getcwd returns a char* we can check to see if the
	//return value is NULL for error checking purposes
	char *error_checker;
	//get the current working directory and store it in the buffer
	error_checker = getcwd(cwd, sizeof(cwd));
	//if it could not get the cwd, error and return
	if(error_checker == NULL) {
		char *error = "Error!: Error showing current directory";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
		return;
	}
	//print to the given stream the current directory
	write(STREAM, cwd, sizeof(char)*strlen(cwd));
	write(STREAM, "\n", sizeof("\n"));
	return; 
}

void makeDir(char *dirName) {
	
	struct dirent *dentry;
	char cwd[PATH_MAX];
	DIR *directory;

	getcwd(cwd, sizeof(cwd));
	directory = opendir(cwd);
	// after the current directory is obtained and is open, if it cannot be read from error
	if(!directory) {
		char *error = "Error!: Cannot read from empty directory";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
		//return after error
		return;
	}
	dentry = readdir(directory);
	//otherwise itrerate through the directory and check if the directory already exists
	do {
		if(strcmp(dirName, dentry->d_name) == 0)
		{
			//if the directory already exists then print and error and close the directory
			char *error2 = "Error! Directory already exists";
			write(STREAM, error2, sizeof(char)*strlen(error2));
			write(STREAM, "\n", sizeof("\n"));
			//close the directory before returning
			closedir(directory);
			return;
		}
	} while(dentry = readdir(directory));
	//otherwise create the new directory
	closedir(directory);
	mkdir(dirName, S_IRWXU);
	return;
}

void changeDir(char *dirName) {
	//create var for determination of failure
	//create buffer for the given dirname path
	int did_change;
	char path[PATH_MAX];
	//ensure the path is const char * for sys call	
	sprintf(path, "%s", dirName);
	//change directory	
	did_change = chdir(path);
	//since chdir returns 0 for success, if is not zero, then it was not able to change
	if(did_change != 0) {
		char* error = "Error!: Unable to change to given directory";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
	}
	return;
}

void deleteFile(char * filepath)
{
	int success;
	struct dirent *dentry;
	char cwd[PATH_MAX];
	DIR *directory;

	//current working directory
	getcwd(cwd, sizeof(cwd));
	directory = opendir(cwd);
	//if the directory cannot be opened error.
	if(!directory) {
		char *error = "Error!: Cannot open directory";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
		return;
		//printf("ERROR: Cannot read from empty directory");
	}
	//read the directory and ensure the file to be deleted exists
	dentry = readdir(directory); 
	do {
		//if the file exists delete it and close the directory
		if(strcmp(filepath, dentry->d_name) == 0)
		{
			//syscall to unlink the file
			success = unlink(filepath);
			if(success == -1) {
				char *error = "Error!: Cannot delete given file";
				write(STREAM, error, sizeof(char)*strlen(error));
				write(STREAM, "\n", sizeof("\n"));
			}
			closedir(directory);
			return;
		}
	} while(dentry = readdir(directory));
	//else close the directory
	closedir(directory);
	//print the error if the file doesnt exist
	char *error2 = "Error!: No such file exists.";
	write(STREAM, error2, sizeof(char)*strlen(error2));
	write(STREAM, "\n", sizeof("\n"));
	return;
}

void copyFile(char *sourcePath, char *destinationPath)
{
	int fd, fd2;
	int num_chars;
	struct dirent *dentry;
	char cwd[PATH_MAX];
	char buffer[PATH_MAX];
	DIR *directory;

	//current working directory
	getcwd(cwd, sizeof(cwd));
	directory = opendir(cwd);
	//if the directory cannot be opened error.
	if(!directory) {
		char *error = "Error!: Cannot open directory";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
		return;
		//return after error
	}
	//read the directory and ensure the file to be deleted exists
	dentry = readdir(directory); 
	do {
		//if the file exists delete it and close the directory
		if(strcmp(destinationPath, dentry->d_name) == 0)
		{
			char *error = "Error!: given destination already exists";
			write(STREAM, error, sizeof(char)*strlen(error));
			write(STREAM, "\n", sizeof("\n"));
			closedir(directory);
			return;
		}
	} while(dentry = readdir(directory));
	//else close the directory
	closedir(directory);
	//open the sourcepath and open the destination path
	fd = open(sourcePath, O_RDONLY);
	//since we are creating a new file, open it with write only and create option with all permissions
	fd2 = open(destinationPath, O_WRONLY | O_CREAT, S_IRWXU);
	if (fd == -1) {
		//if file is not able to be opened
		char *error = "Error!: Issue opening source file";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
		close(fd2);
		return;
	}
	//read line by line the source file and place it in the destination file 
	num_chars = read(fd, buffer, PATH_MAX);
	do {
		write(fd2, buffer, num_chars);
	} while((num_chars = read(fd, buffer, PATH_MAX)) > 0);
	close(fd);
	close(fd2);
	return;
}

void displayFile(char *filename) {
	int fd;
	int num_chars;
	char buffer[PATH_MAX];
	
	fd = open(filename, O_RDONLY);	
	if(fd == -1) {
		//if the file cant be opened
		char *error = "Error!: File cannot be opened.";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
		return;
	}
	num_chars = read(fd, buffer, PATH_MAX);
	do{
		//write to the given stream
		write(STREAM, buffer, num_chars);
	} while((num_chars = read(fd, buffer, PATH_MAX)) > 0);
	//print a newline char 
	write(STREAM, "\n", sizeof("\n"));
	close(fd);
	return;
}

void moveFile(char *sourcePath, char *destinationPath)
{
	int fd, fd2;
	int num_chars;
	char buffer[PATH_MAX];
	struct dirent *dentry;
	char cwd[PATH_MAX];
	DIR *directory;

	//current working directory
	getcwd(cwd, sizeof(cwd));
	directory = opendir(cwd);
	//if the directory cannot be opened error.
	if(!directory) {
		char *error = "Error!: Cannot open directory";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
		return;
	}
	//read the directory and ensure the file to be deleted exists
	dentry = readdir(directory); 
	do {
		//if the file exists delete it and close the directory
		if(strcmp(destinationPath, dentry->d_name) == 0)
		{
			char *error = "Error!: given destination already exists";
			write(STREAM, error, sizeof(char)*strlen(error));
			write(STREAM, "\n", sizeof("\n"));
			closedir(directory);
			return;
		}
	} while(dentry = readdir(directory));
	//else close the directory
	closedir(directory);
	fd = open(sourcePath, O_RDONLY);
	fd2 = open(destinationPath, O_WRONLY | O_CREAT, S_IRWXU);
	//if the source file cannot be opened
	if (fd == -1) {
		//print an error
		char *error = "Error!: Issue opening file(s)";
		write(STREAM, error, sizeof(char)*strlen(error));
		write(STREAM, "\n", sizeof("\n"));
		close(fd2);
		return;
	}
	num_chars = read(fd, buffer, PATH_MAX);
	do {
		write(fd2, buffer, num_chars);
	} while((num_chars = read(fd, buffer, PATH_MAX)) > 0);
	//rename(sourcePath, destinationPath); was what
	//I originally went with but thought this gave me better practice
	//plus I was told not to use rename then that changed on friday 
	//after I wrote this
	close(fd);
	close(fd2);
	//delete the source file
	deleteFile(sourcePath);
	return;
}

int isCMD(char *token)
{
	//array of commands
	char *cmd[9] = {"ls", "pwd", "mkdir", "cd", "cp", "mv", "rm", "cat" };	
	int i = 0;
	for(i; i < 8; i++)
	{
		//if the array contains the given token, return 1 otherwise return 0
		if(strcmp(cmd[i], token) == 0)
		{	
			return 1;
		}
	}
	return 0;
}

											
