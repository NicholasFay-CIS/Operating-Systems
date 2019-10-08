/*
* Description: Lab 2 String processing in C
*
* Author: Nicholas Fay
*
* Date: October 6th 2019
*
* Notes: 
* 1. <add notes we should consider when grading>
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*---------------------------------------------------------------------------*/

/*-----------------------------Program Main----------------------------------*/
int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);
	FILE *stream;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	const char s[2] = " ";
	char *token;
	int count = 0;
	char *input = NULL;
	
	stream = fopen(argv[1], "r");
	while ((nread = getline(&line, &len, stream)) != -1) {
		printf(">>> %s\n", line);
	}
	token = strtok(line, s);
	while(token != NULL) {
		printf("T%d: %s\n", count, token);
		token = strtok(NULL, s);
		count++;
	}
	printf(">>> ");
	getline(&input, &len, stdin);
	while(strcmp(input, "exit\n") != 0) {
		printf(">>> ");
		getline(&input, &len, stdin);
	}
	free(line);
	fclose(stream);
	exit(EXIT_SUCCESS);
	
	/* Main Function Variables */
	
	/* Allocate memory for the input buffer. */
	
	/*main run loop*/
		/* Print >>> then get the input string */

		/* Tokenize the input string */

		/* Display each token */
		
		/* If the user entered <exit> then exit the loop */

	
	/*Free the allocated memory*/

	return 1;
}
/*-----------------------------Program End-----------------------------------*/
