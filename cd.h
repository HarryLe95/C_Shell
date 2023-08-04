#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#ifndef FILENAME_MAX
#define FILENAME_MAX 4096
#endif

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define SEP "/"

#define TRUE 1
#define FALSE 0
#define PATHNUM_MAX 100

char *HOME;
char *UHOME;
char *CDPATH;
char *PWD;
char *OLDPWD;
char *CURPATH;
int errno;

typedef struct Token {
    char *value;
    struct Token *prev;
    struct Token *next;
} Token;

void handle_dotdot(char *, char *);
void process_CDPATH(char *, char *, char *, char *);
int cd(char *, char *);
int execute_cd(char* file, char* argv[]);