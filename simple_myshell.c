#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#define MAX_CMD_ARG 10
#define BUFSIZ 256

const char *prompt = "myshell> ";
char* cmdvector[MAX_CMD_ARG];
char  cmdline[BUFSIZ];

static struct sigaction act;

void fatal(char *str){
	perror(str);
	exit(1);
}

void error_message(char * str){
	fputs(str, stderr);
}

int makelist(char *s, const char *delimiters, char** list, int MAX_LIST){	
  int i = 0;
  int numtokens = 0;
  char *snew = NULL;

  if( (s==NULL) || (delimiters==NULL) ) return -1;

  snew = s + strspn(s, delimiters);	/* Skip delimiters */
  if( (list[numtokens]=strtok(snew, delimiters)) == NULL )
    return numtokens;
	
  numtokens = 1;
  
  while(1){
     if( (list[numtokens]=strtok(NULL, delimiters)) == NULL)
	break;
     if(numtokens == (MAX_LIST-1)) return -1;
     numtokens++;
  }
  return numtokens;
}

void sig_handler(int signo){
  switch(signo){
    case SIGCHLD :
	while(waitpid(-1, 0, WNOHANG)>0);
  	cmdline[0] = '\0';
	break;
  }
}

int main(int argc, char**argv){
  int i=0;
  int cmdlen=0;	//number of argument in commandline
  pid_t pid;
  int is_background = 0;

  act.sa_handler = sig_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_NOCLDSTOP;
  sigaction(SIGCHLD, &act, 0);

  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);  //ignore SIGINT, SIGQUIT in the shell process

  while (1) {

  	fputs(prompt, stdout);
	fgets(cmdline, BUFSIZ, stdin);
	cmdline[strlen(cmdline)-1] = '\0';

	for(i=0; i<MAX_CMD_ARG; i++)
		cmdvector[i] = NULL;

	cmdlen =	makelist(cmdline, " \t", cmdvector, MAX_CMD_ARG);

	//input empty line
	if(cmdlen==0) {
		error_message("\n");
		continue;
	}

	//cd <directory>
	if(strcmp(cmdvector[0], "cd")==0){
		//makesure there's 2 arguments
		if(cmdlen!=2) {
			error_message("need 2 arguments\n");
			continue;
		}

		//if path does not define a valid directory
		//if execute permission doesn't exist
		if(chdir(cmdvector[1])==-1){
			error_message("invalid path\n");
		}

		continue;
	}

	//exit
	if(strcmp(cmdvector[0], "exit")==0){
		exit(0);
	}
  	
	//check whether the command input is background
	is_background = 0;
	for(i=0; i<cmdlen; i++) if(strcmp(cmdvector[i],"&")==0){
		is_background=1;
		cmdvector[i] = NULL;
	}

	switch(pid=fork()){
	case 0:			
		if(!is_background){		//do not ignore SIGINT, SIGQUIT if it is foreground process
			signal(SIGINT, SIG_DFL);		
			signal(SIGQUIT, SIG_DFL);	
		}
		execvp(cmdvector[0], cmdvector);
		fatal("main()1");
	case -1:
  		fatal("main()2");
	default:
		if(is_background) break;
		wait(NULL);
	}
  } // end while
  return 0;
}
