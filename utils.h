#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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

static char *HOME;
static char *UHOME;
static char *CDPATH;
static char *PWD;
static char *OLDPWD;
static char *CURPATH;

typedef struct Token {
    char *value;
    struct Token *prev;
    struct Token *next;
} Token;

void handle_dotdot(char *, char *);

int _dir_exists(char *);
void process_CDPATH(char *, char *, char *, char *);

void cd(char *, char *);