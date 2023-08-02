/*
-------------------------------------------------------------------------------------------
Program to print out n even numbers to the terminal 
SIGINT signal will print Yeah!
SIGHUP signal will print Ouch!

-------------------------------------------------------------------------------------------
Compile (without debugging)
    gcc -Wall -o even.o even.c -pedantic

Compile (with debug)
    gcc -Wall -o even.o even.c -pedantic -DDEBUG

-------------------------------------------------------------------------------------------
Run: 
    ./even.o <n>
-------------------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>

void print_even(int);

void sig_handler(int);

int main (int argc, char *argv[]){
    if (argc != 2){
        fprintf(stderr, "Usage: even <int>\n");
        exit(1);
    }
    /* Print PID for simulating SIGHUP using kill -HUP <pid_number> */
    #ifdef DEBUG
        printf("PID: %ld\n", (long)getpid());
    #endif
    int num_evens; 
    num_evens = atoi(argv[1]); 

    /* Pass signal handler */
    signal(SIGINT, sig_handler);
    signal(SIGHUP, sig_handler);
    
    /* Print loop */
    print_even(num_evens);

}

/* Function to print even numbers up to num_evens number
Sleep for 5 seconds after every print statement 

Args:
    num_evens (int): number of even numbers to print 
*/
void print_even(int num_evens){
    int count = 0, num = 0;
    while (count < num_evens){
        printf("%d\n", num);
        sleep(5);
        num = num + 2;
        count = count + 1;
    }
}


/* Function to handle signals: 
SIGINT - print Yeah!
SIGHUP - print Ouch!
*/
void sig_handler(int sig_num){
    switch (sig_num){
        case SIGINT:
            printf("Yeah!\n");
            break;
        case SIGHUP:
            printf("Ouch!\n");
            break;
    }
}