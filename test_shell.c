#include "utils.h"
#include <sys/wait.h>

int main(void){
    int status; 
    int wait_pid;
    int fork_status; 
    char msg[10000];
    strcpy(msg, "cd: A");
    fork_status = fork();
    switch (fork_status){
        case -1:
            perror("Fail to fork.");
        case 0:{
            status = cd(".","-Z");
            if (status != 0)
                perror(msg);
            break;
        }
        default:
        {
            wait_pid = wait(0);
            printf("Execution finished\n");
            break;
        }
    }

    

}