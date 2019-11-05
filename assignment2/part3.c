#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

/* Signaler and Handler functions and globals */
/*---------------------------------------------------------------------------*/
int PROGRAM_COUNT;
pid_t pid[5];
int INDEX;

void signaler(pid_t *pid, int signal, int subtract) {
	int i = 0;
	for(i; i < PROGRAM_COUNT-subtract; i++) {
		printf("Received Signal %d to pid %d\n", signal, pid[i]);
		kill(pid[i], signal);
	}
}

void hdl(int signal_number) {
	printf("Handler: Process: %i - Received Signal: %d\n", getpid(), signal_number);
	pid_t wait_pid;
	int wait_status;
	while(1) {
		if(INDEX < 0) {
			INDEX = PROGRAM_COUNT-1;
		}
		//printf("Entering First loop\n");
		wait_pid = waitpid(pid[INDEX], &wait_status, WNOHANG);
		//printf("Wait status is %d\n", wait_status);
		if(wait_status != -1) {
			printf("Sending Signal 19 to pid: %d\n", pid[INDEX]);
			kill(pid[INDEX], SIGSTOP);
			INDEX--;
			break;
		} else {
			INDEX--;
		}
	}

	while(1) {
		//printf("Entering Second loop\n");
		if(INDEX < 0) {
			INDEX = PROGRAM_COUNT-1;
		}
		wait_pid = waitpid(pid[INDEX], &wait_status, WNOHANG);
		if(wait_status != -1) {
			printf("Sending Signal 18 to pid: %d\n", pid[INDEX]);
			kill(pid[INDEX], SIGCONT);
			break;
		} else {
			INDEX--;
		}
	}
}

int all_children_exited(pid_t *pid) {
	int arrray[PROGRAM_COUNT];
	pid_t wait_pid;
	int wait_status;
	int i;
	for(i = PROGRAM_COUNT; i > 0; i--) {
		wait_pid = waitpid(pid[i], &wait_status, WNOHANG);
		arrray[i] = wait_pid;
	}
	for(i = PROGRAM_COUNT; i > 0; i--) {
		if(arrray[i] == 0) {
			return 1;
		}
	}
	return 0;
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
	struct sigaction action2;
	action.sa_handler = hdl;
	//sigaction(SIGUSR1, &action2, NULL);
	sigaction(SIGALRM, &action, NULL);
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
	INDEX = PROGRAM_COUNT-1;
	fin = fopen(argv[1], "r");
	int j = 0;
	pid[program_count];
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
			sigaction(SIGUSR1, &action2, NULL);
			//sigwait call using signal_set, pointer
			wait_return = sigwait(&signal_set, pointer_signal);
			if(wait_return > 0) {
				printf("Error!: Sigwait failed\n");
			} /*else if( wait_return == 0) {
				if(*pointer_signal == 10) {
					printf("received siguser signal\n");
				}
			} */
			execvp(args_array[0], args_array);
			printf("\nError!: Invalid executable\n\n");
			exit(-1);
		}
		j++;
	}
	sleep(3);
	signaler(pid, SIGUSR1, 0);
	sleep(3);
	signaler(pid, SIGSTOP, 1);
	int k;	
	time_t now, elapsed;
	while(1) {
		int is_exited;
		//alarm(3);
		now = time(NULL);
		elapsed = time(NULL) + 3;
		alarm(3);
		while(1) {
			now = time(NULL);
			if(now > elapsed) {
				elapsed = time(NULL) + 3;
				break;
			}
		}
		is_exited = all_children_exited(pid);
		//printf("IS_EXITED: %d\n", is_exited);
		if(is_exited == 0) {
			break;
		}
	}
	//signaler(pid, SIGCONT, 0);
	return 1;
}