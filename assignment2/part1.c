#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>

int number_of_lines;

int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);
	char *line_buffer = NULL;
	size_t number_read;
	size_t length = 0;
	char *token;
	char *args_array[10];
	int i = 0;
	const char dil[2] = " ";
	FILE *fin;

	//check arguments 
	if(argc == 1 || argc < 2) {
		printf("Error!: wrong command line arguments\n");
		return 1;
	}
	//open input file
	fin = fopen(argv[1], "r");
	int program_count = 0;
	while((number_read = getline(&line_buffer, &length, fin)) != -1) {
		program_count++;
	}
	//close the file so we can reopen it at the beg again
	fclose(fin);

	fin = fopen(argv[1], "r");
	int j = 0;
	pid_t pid[program_count];
	pid_t wait_pid;
	int wait_status;
	//number_of_lines = count;
	while((number_read = getline(&line_buffer, &length, fin)) != -1) {
		
		i = 0;
		for(i; i < 10; i++) {
			args_array[i] = NULL;
		}

		i = 0;
		token = strtok(line_buffer, dil);
		while(token != NULL) {
			if(token[strlen(token)-1] == '\n') {
				token[strlen(token)-1] = 0;
			}
			args_array[i] = token;
			token = strtok(NULL, dil);
			i++;
		}

		//fork call
		pid[j] = fork();

		//check for error
		if(pid[j] < 0) {
			exit(-1);
		}

		//its a child process
		if(pid[j] == 0) {
			
			execvp(args_array[0], args_array);
			printf("\nError!: Invalid executable\n\n");
			exit(-1);
		}
		//wait_pid = waitpid(pid[j], &wait_status, WUNTRACED | WCONTINUED);
		j++;
	}
	int k = 0;
	for(k; k < program_count; k++) {
		wait_pid = waitpid(pid[k], &wait_status, WUNTRACED | WCONTINUED);
	}
	return 1;
}