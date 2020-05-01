/*-----------------------------------------------------------------------------
File name: Lab6.c
Description: A simple lab showing the use of signals.
Author: Nicholas David Fay
-----------------------------------------------------------------------------*/

/*---------------------------Preprocessor Directives-------------------------*/
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
/*---------------------------------------------------------------------------*/

/*---------------------------------Functions---------------------------------*/

void signaler(pid_t pid, int signal) {
    printf("    Child process: %d - Received signal %d\n", pid, signal);
    kill(pid, signal);
}
/*---------------------------------------------------------------------------*/

/*--------------------------------Program Main-------------------------------*/
int main(void) 
{
    //variable declarations
    pid_t pid, w;
    int wstatus, eStatus;

    //create a child process
    pid = fork();
    if(pid < 0) {
        perror("Error! could not create a new process.\n");
        exit(EXIT_FAILURE);
    } else if(pid == 0) {
        //This code runs in the child process only
	printf("    Child process: %d - Starting...\n", getpid());
	//Add your while loop here
    int i = 1;
    for(i; i < 11; i++) {
        sleep(1);
        printf("    Child process: %d - Still alive after %d seconds\n", getpid(), i);
    }

    } else if(pid > 0) {
        //This code runs in the parent process only
	printf("Parent process: %d - Sending signals to child...\n", getpid());
	//Add code to send your signals  in a loop here
    int j = 0;
    printf("Parent process: %d - Waiting for child to complete...\n", getpid());
    for(j; j < 11; j++) {
        signaler(pid, SIGSTOP);
        sleep(3);
        signaler(pid, SIGCONT);
        sleep(1);
    }
	w = waitpid(pid, &wstatus, 0);
	printf("Parent process: %d - Finished\n", getpid());
    }
    
    //exit out of program
    return EXIT_SUCCESS;
}
/*----------------------------------------------------------------------------*/
