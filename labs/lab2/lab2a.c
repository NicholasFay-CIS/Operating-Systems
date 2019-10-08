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
	
	while(1) {
		printf(">>> ");
		getline(&line, &len, stdin);
		if(strcmp(line, "exit\n") == 0) {
			free(line);	
			exit(EXIT_SUCCESS);
		}
		if(strcmp(line, "\n") == 0) {
			printf(">>> ");
			getline(&line, &len, stdin);
		}
		if(strcmp(line, "\n") != 0) {
			if(strcmp(line, "exit\n") == 0) {
				free(line);
				exit(EXIT_SUCCESS);
			}
			token = strtok(line, s);
			while(token != NULL) {
				printf("T%d: %s\n", count, token);
				token = strtok(NULL, s);		
				count++;
			}
		}
		printf("\e[A");
		count = 0;
	}

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
