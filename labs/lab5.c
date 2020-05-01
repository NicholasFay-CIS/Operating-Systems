/*
* Description: A simple lab showing signal processing
*
* Author: Nicholas Fay
*
* Date: 10/21/2019
*
* Notes:
* 1. 5 Processes
* 2. Create Handler
* 3. Create signaler 
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
/*---------------------------------------------------------------------------*/

void hdl(int signal_number, siginfo_t *siginfo, void *context) {
	//Variables
	//create an integer for a signal
	int int_sig;
	//then create a pointer to that integers address, this is what we will
	int *pointer_signal = &int_sig;
	int wait_return;
	int added;
	sigset_t signal_set;
	//add signal to specified signal set created above
	added = sigaddset(&signal_set, signal_number);
	if(added != 0) {
		printf("Error: Signal Not added to set\n");
	}
	//check if the singal is SIGUSR1
	if(signal_number != SIGUSR1) {
		exit(-1); //exit if not
	// otherwise try to
	} else {
		printf("Child Process: %i - Received Signal: %d\n", getpid(), signal_number);
		//Call to sigwait
		wait_return = sigwait(&signal_set, pointer_signal);
		//If sigwait fails
		if(wait_return > 0) {
			printf("Error: Sigwait failed\n");
		}
	}
}

void signaler(pid_t *pid, int signal) {
	int i = 0;
	for(i; i < 5; i++) {
		kill(pid[i], signal);
	}
}

/*-----------------------------Program Main----------------------------------*/
int	main()
{
	//variable definitions
	pid_t pid[5]; 
	pid_t w;
	int wstatus, i;
	struct sigaction action;
	action.sa_sigaction = &hdl;
	action.sa_flags = SA_SIGINFO;
	i=0;
	//create 5 processes
	//sigaction(SIGUSR1, &action, NULL);
	for(i; i < 5; i++) {
		//create a new process
		pid[i] = fork();
		//printf("%d\n", pid[i]);

		if (pid[i] == 0) {
			//Child Process
			sigaction(SIGUSR1, &action, NULL);
			printf("	Child Process: %i - Starting infinite loop...\n", getpid());
			while(1) {
				i++;
				if(i%10000) {
					sleep(1);
					printf("	Child Process: %i - Running infinite loop...\n", getpid());
					i=0;
				}
			}
		}
	}
	//else this is an existing proc i.e. the parent
	sleep(3);
	signaler(pid, SIGUSR1);
	sleep(3);
	signaler(pid, SIGUSR1);
	signaler(pid, SIGINT);
	for(i = 0; i < 5; i++) {
		printf("Parent Process: %i, Waiting for child to finish...\n", getpid());
		w = waitpid(pid[i], &wstatus, WUNTRACED | WCONTINUED);
		printf("All child processes joined. Exiting.\n");
	}
}
/*-----------------------------Program End-----------------------------------*/
