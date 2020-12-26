#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#define MAX_CMD_ARG 20
#define BUFSIZ 256

const char *prompt = "myshell> ";

int is_background = 0;

static struct sigaction act;

void fatal(char *str){
	perror(str);
	exit(1);
}

void error_message(char * str){
	fputs(str, stderr);
	fputc('\n', stderr);
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
    case SIGCHLD :		//handle SIGCHLD
	while(waitpid(-1, 0, WNOHANG)>0);
	break;
  }
}

//making redirection.
void redirection_maker(char *cmd){
	char *file_name;
	int cmdlen = strlen(cmd);
	int file_descripter, i;
	const mode_t permission = 0644;

	for(i=0; i<cmdlen; i++){
		if(cmd[i] == '>'){			//make output redirection
			cmd[i] = '\0';
			file_name = strtok(&cmd[i+1], " \t");

			i += strlen(file_name)-1;
			file_descripter = open(file_name, O_WRONLY | O_CREAT | O_TRUNC , permission);

			if( file_descripter == -1 )
				fatal("redirection_maker >");
			
			dup2(file_descripter, 1);
			close(file_descripter);
		}
		else if(cmd[i] == '<'){		//make input redirection
			cmd[i] = '\0';
			file_name = strtok(&cmd[i+1], " \t");

			i += strlen(file_name)-1;
			file_descripter = open(file_name, O_RDONLY | O_CREAT, permission);

			if( file_descripter == -1 )
				fatal("redirection_maker, <");
			
			dup2(file_descripter, 0);
			close(file_descripter);
		}
	}
}

//execute block excluded '|'
void execute_cmd(char * cmd){
	char * cmd_vector[MAX_CMD_ARG];

	//make redirection
	redirection_maker(cmd);

	makelist(cmd, " \t", cmd_vector, MAX_CMD_ARG);
	
	execvp(cmd_vector[0], cmd_vector);
}

//execute everything included '|'
void execute_pipelist(char * cmd){
	int i=0, cnt=0, p[2];
	char *pipelist[MAX_CMD_ARG];

	//divide command line by '|'. so execute block by block
	cnt = makelist(cmd, "|", pipelist, MAX_CMD_ARG);

	for(i =0 ; i<cnt-1; i++){
		pipe(p);	//make pipe		
		switch(fork()){
			case -1: 
				fatal("execute_pipelist()");
				break;
			case 0: 
				close(p[0]);
				dup2(p[1], 1);	//dup to stdout
				execute_cmd(pipelist[i]);
				break;
			default:
				close(p[1]);
				dup2(p[0], 0);	//dup to stdin
				break;
		}
	}

	execute_cmd(pipelist[i]);
}


int main(int argc, char**argv){
  int i=0, c;
  int cmdlen=0;	//number of argument in commandline
  pid_t pid;
  char cmdtemp[BUFSIZ];
  char* cmdvector[MAX_CMD_ARG];
  char  cmdline[BUFSIZ];

  act.sa_handler = sig_handler;
  sigemptyset(&act.sa_mask);
  sigaction(SIGCHLD, &act, 0);

  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);  //ignore SIGINT, SIGQUIT in the shell process

  while (1) {
	cmdline[0] = '\0';		//for unkown bug. it prevent at least unintended input
  	fputs(prompt, stdout);
	fgets(cmdline, BUFSIZ, stdin);
	cmdline[strlen(cmdline)-1] = '\0';

	strcpy(cmdtemp, cmdline);
	
	//check whether the command input is background
	is_background = 0;
	for(i=0; i<strlen(cmdline); i++) if(cmdline[i] == '&'){
		is_background=1;
		cmdline[i] = ' ';
		cmdtemp[i] = ' ';
		break;
	}

	cmdlen = makelist(cmdline, " \t", cmdvector, MAX_CMD_ARG);

	//input error
	if(cmdlen<=0) {
		printf("%d\n", cmdlen);
		fputs("input error. input again plz\n", stderr);
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
  	
	switch(pid=fork()){
	case 0:			
		//do not ignore SIGINT, SIGQUIT if it is foreground process
		if(!is_background){		
			signal(SIGINT, SIG_DFL);		
			signal(SIGQUIT, SIG_DFL);	
		}
		execute_pipelist(cmdtemp);
		fatal("main()1");
	case -1:
  		fatal("main()2");
	default:		
		if(is_background) {
			while ((c = getchar()) != '\n' && c != EOF);		//flush stdin
			break;
		}
		wait(NULL);
	}
  } // end while
  return 0;
}


