/*********************************************************************
   Program  : miniShell                   Version    : 1.3
 --------------------------------------------------------------------
   skeleton code for linix/unix/minix command line interpreter
 --------------------------------------------------------------------
   File			: minishell.c
   Compiler/System	: gcc/linux

********************************************************************/

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "cd.h"
#define NUMTOKENS 20 /* max number of command tokens */
#define INSIZE 100   /* input buffer size */
char line[INSIZE];   /* command input buffer */

/*
        shell prompt
*/
void prompt(void) {
    // fprintf(stdout   , "\n msh> ");
    fflush(stdout);
}

int main(int argk, char* argv[], char* envp[])
/* argk - number of arguments */
/* argv - argument vector from command line */
/* envp - environment pointer */

{
    int fork_status;       /* value returned by fork sys call */
    int exec_status;       /* value returned by execv call */
    int wait_pid;          /* value returned by wait */
    char* args[NUMTOKENS]; /* array of pointers to command line tokens */
    char* sep = " \t\n";   /* command line token separators    */
    int i;                 /* parse index */

    /* prompt for and process one command line at a time  */
    while (1) { /* do Forever */
        /* Tokenise input command */
        prompt();
        fgets(line, INSIZE, stdin);
        fflush(stdin);

        if (feof(stdin)) { /* non-zero on EOF  */
            // fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(),
            // feof(stdin), ferror(stdin));
            exit(0);
        }
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
            continue; /* to prompt */

        args[0] = strtok(line, sep);
        for (i = 1; i < NUMTOKENS; i++) {
            args[i] = strtok(NULL, sep);
            if (args[i] == NULL) break;
        }
        /* assert i is number of tokens + 1 */
        /* fork a child process to exec the command in v[0] */

        switch (fork_status = fork()) {
            case -1: /* fork returns error to parent process */
            {
                perror("Fork failed");
                break;
            }
            case 0: /* code executed only by child process */
            {   
                if (strcmp(args[0],"cd")==0)
                    exec_status = execute_cd(args[0], args);
                else
                    exec_status = execvp(args[0], args);
                
                if (exec_status != 0) {
                    perror("Forked process status failed. Child process terminated");
                    _exit(1);
                }
            }
            default: /* code executed only by parent process */
            {
                wait_pid = wait(0);
                // printf("%s done \n", args[0]);
                break;
            }
        } /* switch */
    }     /* while */
    return 0;
} /* main */
