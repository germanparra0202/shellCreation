/* Author: German Parra 
 * Operating Systems Project Two
 * Silicon Valley Semester */

/* Directives */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/wait.h>
#include<sys/types.h> 
#include<ctype.h>
#include<signal.h>
#include<time.h>

/* Function Prototypes */
void startCommand(char* words[]);
void waitCommand(int status);
void runCommand(char* words[], int status);
void killCommand(int num);
void stopCommand(int num);
void continueCommand(int num);

/* Main Function */
int main() {

	// Start the "myshell>" to signify shell is running
	//int count = 0;
	printf("myshell> ");

	// Varaibles for getting input 
	// const char delimeter[2] = " "; // delimiter for splitting up inputs 
	char* token; // the smaller string 
	int arg = 1;
	int status = 0;

	// char[30] to store stdin
	char inputString[4096];

	while(fgets(inputString, 4096, stdin) != NULL) {

		// Print "myshell> " for every iteration 
		/*if(count > 0) {
			printf("myshell> ");
		}
		count = count + 1;*/

		// Allow for a new buffer value intead of the previous one 
		fflush(stdout);

		// Split up into tokens to get command 
		token = strtok(inputString, " \t\n"); // gets first word
		char *words[100]; // to get the inputs given for commands/args
		words[0] = token; 

		if( (strcmp(words[0], "quit") == 0) || (strcmp(words[0], "exit") == 0) ) {
			printf("Exited Successfully\n");
			exit(0);
		}

		//printf("%s\n", words[0]);

		// Iterating through the arguments 
		while(token != NULL) {

			token = strtok(0, " \t\n");
			words[arg] = token;			
			arg = arg + 1;
		}

		// Set last value of words pointer array to NULL
		words[arg-1] = NULL;

		// Strcmp to know what command it is 

		// start 
		if(strcmp(words[0], "start") == 0) {
			
			//char* target[] = strdup(words);
			startCommand(words);
		}

		// wait 
		else if(strcmp(words[0], "wait") == 0) {
			
			//char* target[] = strdup(words);
			waitCommand(status);
		}

		// run
		else if(strcmp(words[0], "run") == 0) {
			
			//char* target[] = strdup(words);
			runCommand(words, status);
		}

		else if(strcmp(words[0], "kill") == 0) {
			
			//char* target[] = strdup(words);
			// the number in the first place is the process to kill 
			killCommand(atoi(words[1]));
		}
		
		else if(strcmp(words[0], "stop") == 0) {
			
			//char* target[] = strdup(words);
			stopCommand(atoi(words[1]));
		}
		
		else if(strcmp(words[0], "continue") == 0) {
			
			//char* target[] = strdup(words);
			continueCommand(atoi(words[1]));
		}

		else {
			printf("myshell: unknown command %s\n", words[0]);
			
		}

		// Reset arg back to 1, to reset the array of pointers, reprint myshell>
		arg = 1;
		printf("myshell> ");
	}

	return 0;
}

/* Function Definitions */
void startCommand(char* words[]) {
	
	/* int i = 0;
	while(words[i] != NULL) {
		printf("%s ", words[i]);
		i = i + 1;
	}
	printf("\n");*/
	char* command = words[1];
	words++;

	// Start the new progrmam with given arguments 
	pid_t pidValue = fork();

	// If the fork fails for whatever reason, give message 
	if(pidValue < 0) { // fork failed; exit
    	fprintf(stderr, "myshell: fork failed, %s\n", strerror(errno));
        exit(1);
    } 

	// Starting the new command using execvp 
	else if(pidValue == 0) {

		// Print message that process started to user
		printf("myshell: %d started\n", (int) getpid());

		// Run the command via execvp
		int status = execvp(command, words);
		
		//printf("%d\n", status);
		if(status == -1) {
			printf("myshell: error with execvp, %s\n", strerror(errno));
			exit(1);
		}
	}

	// parent is taking too long, child running before in parallel
	sleep(1);

	/*else {
		//pidValue = wait(NULL);
		printf("this is parent\n");
	}*/

	//printf("myshell> ");
}

void waitCommand(int status) {

	pid_t rc = wait(&status);
	int signal = WTERMSIG(status);

	if(rc == -1) {
		printf("myshell: no processes left\n");
	}

	else {
		
		if(signal == 0) {
			printf("myshell: process %d exited normally with status %d.\n", rc, signal);
		}

		else {
			printf("myshell: process %d exited abnormally with signal %d: %s.\n", rc, signal, strsignal(signal));
		}
	}

	//printf("myshell> ");
}

void runCommand(char* words[], int status) {

	/*int i = 0;
	while(words[i] != NULL) {
		printf("%s ", words[i]);
		i = i + 1;
	}
	printf("\n");*/

	char* command = words[1];
	words++;

	// Get rid of the "run" for words[0]
	/* i = 0;
	while(words[i] != NULL) {
		strcpy(words[i], words[i+1]);
		//words[i] = words[i+1];
		printf("%s\n", words[i]);
	}*/

	// Run the actual command using fork()/execvp()
	int rc = fork();

	if(rc < 0) { // fork failed 
		fprintf(stderr, "myshell: fork failed\n");
        exit(1);
	}

	// If fork worked 
	else if(rc == 0) {
		// this is the child process
		int status_code = execvp(command, words);
		
		// Error
		if(status_code == -1) {
			printf("myshell: Problem executing %s\n", words[1]);
			exit(1);
		}

		// exited succesfully 
		exit(0);
	}

	else {
		// this is the parent 
		int status;
		waitpid(rc, &status, 0);

		if(status == -1) {
			printf("myshell: no processes left\n");
		}

		else {
			if(WTERMSIG(status) == 0) {
				printf("myshell: process %d exited normally with status 0.\n", rc);
			}

			else {
				int signal = WTERMSIG(status);
				printf("myshell: process %d exited abnormally with signal %d: %s.\n", rc, signal, strsignal(signal));
			}
		}
	}

	//printf("myshell> ");
}

void killCommand(int pid) {

	// get the value returned by kill
	int killVal = kill(pid, SIGKILL);

	if(killVal == 0) {
        printf("myshell: Process %d killed\n", pid);
    }

	// if it returned -1 
    else if(killVal == -1) {
        printf("myshell: This process doesn't exist\n");
    }
}

void stopCommand(int pid) {

	// get the value returned by kill
	int stopVal = kill(pid, SIGSTOP);

	if(stopVal == 0) {
        printf("myshell: Process %d stopped.\n", pid);
    }

	// if it returned -1 
    else if(stopVal == -1) {
        printf("myshell: This process doesn't exist\n");
    }
}

void continueCommand(int pid) {

	// get the value returned by kill
	int conVal = kill(pid, SIGCONT);

	if(conVal == 0) {
        printf("myshell: Process %d continued.\n", pid);
    }

	// if it returned -1 
    else if(conVal == -1) {
        printf("myshell: This process doesn't exist\n");
    }
}
