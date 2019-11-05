#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

/*---------------------------------------------------------------------------*/
int PROGRAM_COUNT;

void hdl(int signal_number, siginfo_t *siginfo, void *context) {
	printf("Child Process: %i - Received Signal: %d\n", getpid(), signal_number);
}

void signaler(pid_t *pid, int signal) {
	int i = 0;
	for(i; i < PROGRAM_COUNT; i++) {
		printf("Received Signal %d to pid %d\n", signal, pid[i]);
		kill(pid[i], signal);
	}
}
/*----------------------------------------------------------------------------*/

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
	struct sigaction action;
	action.sa_sigaction = &hdl;
	action.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &action, NULL);
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
	PROGRAM_COUNT = program_count;
	fin = fopen(argv[1], "r");
	int j = 0;
	pid_t pid[program_count];
	pid_t wait_pid;
	int wait_status;
	//number_of_lines = count;
	while((number_read = getline(&line_buffer, &length, fin)) != -1) {
		int int_sig;
		//then create a pointer to that integers address, this is what we will
		int *pointer_signal = &int_sig;
		int wait_return;
		int added;
		sigset_t signal_set;
		//add signal to specified signal set created above
		added = sigaddset(&signal_set, 10);
		if(added != 0) {
			printf("Error: Signal Not added to set\n");
		}
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
			//sigaction(SIGUSR1, &action, NULL);
			wait_return = sigwait(&signal_set, pointer_signal);
			if(wait_return > 0) {
				printf("Error!: Sigwait failed\n");
			} /*else if( wait_return == 0) {
				if(*pointer_signal == 10) {
					printf("Received SIGUSR1 signal: 10\n");
				}
			} */
			execvp(args_array[0], args_array);
			printf("\nError!: Invalid executable\n\n");
			exit(-1);
		}
		j++;
	}
	sleep(3);
	signaler(pid, SIGUSR1);
	sleep(3);
	signaler(pid, SIGSTOP);
	sleep(3);
	signaler(pid, SIGCONT);
	int k = 0;
	for(k; k < program_count; k++) {
		//use zero for the untraced
		wait_pid = waitpid(pid[k], &wait_status, 0);
	}
	free(token);
	free(line_buffer);
	fclose(fin);
	return 1;
}