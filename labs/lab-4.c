#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv[]) {
	pid_t pid;
	pid = fork();
	if(pid == 0) {
		printf("Failure\n");
		return 1;
	}
	execvp("forloop.exe", *argv);  
	printf("Greetings from child/parent pid: %d\n", getpid());	
	return 1;
}
