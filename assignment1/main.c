#include <stdio.h>
#include <stdlib.h>
#include "command.h"
#include <sys/syscall.h>
#include <unistd.h>



int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	while(1) {
		printf(">>> ");
		getline(&line, &len, stdin);
		if(strcmp(line, "exit\n") == 0) {
			free(line);
			exit(EXIT_SUCCESS);
		}
		if(strcmp(line, "\n") != 0) {


		}
			


	}
	return 1;
} 
