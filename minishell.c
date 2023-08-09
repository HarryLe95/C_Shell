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
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#define NUMTOKENS 20 /* max number of command tokens */
#define INSIZE 100   /* input buffer size */
char line[INSIZE];   /* command input buffer */

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

typedef struct Token
{
    char *value;
    struct Token *prev;
    struct Token *next;
} Token;

/* CD Header Public methods */
void handle_dotdot(char *, char *);
void process_CDPATH(char *, char *, char *, char *);
int cd(char *, char *);
int execute_cd(char *file, char *argv[]);

/*shell prompt*/
void prompt(void)
{
    // fprintf(stdout   , "\n msh> ");
    fflush(stdout);
}

Token *_createToken(char *value)
{
    if (value == NULL)
    {
        return NULL;
    }
    Token *new_token = (Token *)malloc(sizeof(Token));
    new_token->value = value;
    new_token->prev = NULL;
    new_token->next = NULL;
    return new_token;
}

void _deleteToken(Token *token)
{
    if (token == NULL)
        return;
    _deleteToken(token->next);
#ifdef DEBUG
    printf("Free %s\n", token->value);
#endif
    free(token);
}

/* Join token to form a string and put at dest.

Dest is set to "" if token is NULL. It is the user's responsibility
to ensure that dest is allocated enough memory

Args:
    Token* node: linked list of tokens.
    char* dest: preallocated pointer to result.

*/
void joinToken(Token *node, char *dest)
{
    strcpy(dest, "");
    if (node == NULL)
        return;
    Token *current = node;
    while (current != NULL)
    {
        if (strcmp(dest, "") != 0)
            strcat(dest, "/");
        strcat(dest, current->value);
        current = current->next;
    }
}

void _printToken(Token *node)
{
    if (node != NULL)
    {
        printf("%s\n", node->value);
        _printToken(node->next);
    }
}

/* Resolve path contained . and ..

Dot components and any slashes that separate them from the next component
shall be deleted. For each dot-dot component, if there is a preceding component
and it is neither root nor dot-dot, the preceding component, all slashes
separating the preceding component from dot-dot, dot-dot and all slashes
separating dot-dot from the following component shall be deleted.

Args:
    char* path: path to resolve
    char* dest: pointer storing the dest result. User must ensure that memory is
    allocated for dest.

*/
void handle_dotdot(char *path, char *dest)
{
    strcpy(dest, "");
    /* Handle NULL input */
    if (path == NULL)
        return;

    /* Check if path has a leading / */
    int leading_slash = 0;
    if (path[0] == '/')
        leading_slash = 1;

    /* Tokenise using separator*/
    char cp_path[FILENAME_MAX];
    strcpy(cp_path, path);

    Token *root = _createToken(strtok(cp_path, SEP));
    Token *current_token = root;
    Token *previous_token = NULL;
    Token *next_token = NULL;

    while (current_token != NULL)
    {
        next_token = _createToken(strtok(NULL, SEP));
        previous_token = current_token;
        current_token = next_token;
        previous_token->next = next_token;
        if (next_token != NULL)
            next_token->prev = previous_token;
    }

#ifdef DEBUG
    printf("Pass tokenisation\n");
#endif

    /* Remove . and .. */
    current_token = root;
    previous_token = NULL;
    next_token = NULL;
    while (current_token != NULL)
    {
        next_token = current_token->next;
        /* Handle . */
        if (strcmp(current_token->value, ".") == 0)
        {
#ifdef DEBUG
            printf("Processing %s\n", current_token->value);
#endif
            if (next_token != NULL)
                next_token->prev = previous_token;
            if (previous_token != NULL)
                previous_token->next = next_token;
            current_token->next = NULL;
            if (current_token == root)
                root = next_token;
            _deleteToken(current_token);
            current_token = NULL;
        }

        /* Handle .. */
        if (current_token != NULL)
        {
            if (strcmp(current_token->value, "..") == 0)
            {
/* There is previous token that is not .. */
#ifdef DEBUG
                printf("Processing %s\n", current_token->value);
#endif
                if (previous_token != NULL &&
                    strcmp(previous_token->value, "..") != 0)
                {
                    Token *temp = previous_token->prev;
                    if (next_token != NULL)
                        next_token->prev = temp;
                    if (temp != NULL)
                        temp->next = next_token;
                    current_token->next = NULL;
                    if (previous_token == root)
                        root = next_token;
                    _deleteToken(previous_token);
                    current_token = NULL;
                    previous_token = temp;
                }
            }
        }

        /* Reset pointers */
        if (current_token != NULL)
            previous_token = current_token;
        current_token = next_token;

#ifdef DEBUG
        if (previous_token != NULL)
            printf("Prev: %s, ", previous_token->value);
        else
            printf("Prev: NULL, ");
        if (next_token != NULL)
            printf("Next: %s\n", next_token->value);
        else
            printf("Next: NULL\n");
#endif
    }

#ifdef DEBUG
    printf("Pass token filter\n");
    printf("Print token from root: \n");
    _printToken(root);
#endif
    /* Join tokens */
    if (root == NULL)
    {
        strcpy(dest, "");
    }
    else
    {
        joinToken(root, dest);
    }
    if (leading_slash == 1)
    { /* Give back leading slash if there is*/
        memmove(dest + 1, dest, strlen(dest) + 1);
        memcpy(dest, "/", 1);
    }

    /* Free memory */
    _deleteToken(root);

#ifdef DEBUG
    printf("Final string: %s\n", dest);
#endif
}

/* Check if a directory exist

Args:
    char* dir - directory to validate

Return:
    int - boolean result
*/
int dir_exists(char *dir)
{
    struct stat sb;
    if (dir == NULL)
        return FALSE;
    if (stat(dir, &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        return TRUE;
    }
    return FALSE;
}

/* Set CURPATH based on paths in CDPATH

Set CURPATH to be the first valid dir formed by joining path/dir for every path
in CDPATH. If no valid dir exists, set CURPATH = PWD/dir

Args:
    char* CDPATH - pointer to CDPATH - string of dirs separated by :
    char* PWD - pointer to pwd
    char* CURPATH - pointer to CURPATH - where result is stored. Memory must be
    allocated by user.
     char* dir - pointer to destination path (relative)

*/
void process_CDPATH(char *CDPATH, char *PWD, char *CURPATH, char *dir)
{
    strcpy(CURPATH, "");

    /* Handle NULL CDPATH */
    if (CDPATH == NULL || strcmp(CDPATH, "") == 0)
    {
        sprintf(CURPATH, "%s/%s", PWD, dir);
        return;
    }

    /* Non NULL CDPATH */
    char *token;
    char path[FILENAME_MAX];
    char CDPATH_CPY[FILENAME_MAX];

    /* Copy to avoid modifying the original string */
    strcpy(CDPATH_CPY, CDPATH);

    /* Split to subpaths */
    token = strtok(CDPATH_CPY, ":");
    do
    {
        /*
        Set path = token/dir if token is not NULL
            path = ./dir if token is NULL
        */
        if (token == NULL || strlen(token) == 0)
            sprintf(path, "./%s", dir);
        else
        {
            if (token[strlen(token) - 1] == '/')
                sprintf(path, "%s%s", token, dir);
            else
                sprintf(path, "%s/%s", token, dir);
        }
        /* Set CURPATH to path if path is a valid dir */
        if (dir_exists(path))
        {
            strcpy(CURPATH, path);
#ifdef DEBUG
            printf("CDPATH matched: %s\n", path);
#endif
            break;
        }
        /* Process the next token */
        token = strtok(NULL, ":");
    } while (token != NULL);

    /* Set default path if no valid path from CDPATH */
    if (strcmp(CURPATH, "") == 0)
        sprintf(CURPATH, "%s/%s", PWD, dir);
}

void update_env_vars(char *curpath, char *pwd, char *option)
{
    if (curpath != NULL || strlen(curpath) != 0)
    {
        char _oldpwd[FILENAME_MAX + 10];
        char _pwd[FILENAME_MAX + 10];
        char resolved_slink_path[FILENAME_MAX];
        char resolved_dotdot_path[FILENAME_MAX];
        char temp[FILENAME_MAX];

        /* Resolve symbolic link based on option flag */
        if (strcmp(option, "-L") == 0)
            strcpy(resolved_slink_path, curpath);
        else
            realpath(curpath, resolved_slink_path);
        handle_dotdot(resolved_slink_path, resolved_dotdot_path);
/* Update variables */
#ifdef DEBUG
        printf("Before updating env: \n");
        printf("PWD: %s, OLDPWD: %s\n", getenv("PWD"), getenv("OLDPWD"));
#endif
        if (strcmp(resolved_dotdot_path, pwd) != 0)
        {
            strcpy(temp, PWD);
            sprintf(_pwd, "PWD=%s", resolved_dotdot_path);
            putenv(_pwd);
            sprintf(_oldpwd, "OLDPWD=%s", temp);
            putenv(_oldpwd);
        }
#ifdef DEBUG
        printf("After updating env: \n");
        printf("PWD: %s, OLDPWD: %s\n", getenv("PWD"), getenv("OLDPWD"));
#endif
    }
}

/* Implement cd confirming to POSIX standard outlined in this document:
 * https://pubs.opengroup.org/onlinepubs/009695299/utilities/cd.html

 Args:
    char* _dir: target directory
    char* _option: either "-L" or "-P"

 */
int cd(char *_dir, char *_option)
{
    char dir[FILENAME_MAX];
    int status;

    /* Init Env Variables */
    HOME = getenv("HOME");
    CURPATH = (char *)malloc(sizeof(char) * FILENAME_MAX);
    CDPATH = getenv("CDPATH");
    PWD = getenv("PWD");
    OLDPWD = getenv("OLDPWD");

    /* If dir is null, set dir to HOME */
    if (_dir == NULL || strcmp(_dir, "~") == 0)
    {
        strcpy(dir, HOME);
        /* Set - to $OLDPWD */
    }
    else if (strcmp(_dir, "-") == 0)
    {
        strcpy(dir, OLDPWD);
    }
    else
    {
        strcpy(dir, _dir);
    }

    /* Set CURPATH based on dir and CDPATH */
    if (dir[0] == '/') // Handle cd /
        strcpy(CURPATH, dir);
    else if (dir[0] == '.' || (dir[0] == '.' && dir[1] == '.')) // Handle cd . or cd ..
        sprintf(CURPATH, "%s/%s", PWD, dir);
    else if (dir[0] == '~' && dir[1] == '/')
    { // Expand ~/directory
        sprintf(CURPATH, "%s%s", HOME, dir + 1);
    }
    else
        process_CDPATH(CDPATH, PWD, CURPATH, dir);

/* Perform a CD */
#ifdef DEBUG
    printf("Final CURPATH: %s\n", CURPATH);
#endif
    status = chdir(CURPATH);
    if (status == 0)
        update_env_vars(CURPATH, PWD, _option);
    else
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int execute_cd(char *file, char *argv[])
{
    char *token;
    char *dir = "";
    char *option = "-L";
    int index;
    int status;
    int dFlag = 0; /* Whether dir has been provide */
    index = 1;
    token = argv[1];
#ifdef DEBUG
    printf("Token: %s\n", token);
#endif

    /*Handle just cd */
    if (token == NULL)
    {
    }
    else
    {
        while (token != NULL)
        {
            if (dFlag == 1)
            {
                errno = EINVAL;
                fprintf(stderr, "cd: too many arguments\n");
                return EXIT_FAILURE;
            }
            if (token[0] == '-' && strcmp(token, "-") != 0)
            { /* Is a token */
                if (strcmp(token, "-P") == 0 || strcmp(token, "-L") == 0)
                    option = token;
                else if (strcmp(token, "-LP") == 0)
                    option = "-P";
                else if (strcmp(token, "-PL") == 0)
                    option = "-L";
                else
                {
                    errno = EINVAL;
                    fprintf(stderr, "cd: %s: invalid option\n", token);
                    fprintf(stderr, "cd: usage: cd [-L|-P] [dir]\n");
                    return EXIT_FAILURE;
                }
            }
            else
            { /* Is a dir */
                dFlag = 1;
                dir = token;
            }
            index = index + 1;
            token = argv[index];
        }
    }

    if (option == NULL)
    {
    }
    option = "-L";
#ifdef DEBUG
    printf("Executing 'cd(%s,%s)'\n", dir, option);
#endif
    status = cd(dir, option);
    if (status != EXIT_SUCCESS)
    {
        errno = ENOENT;
        fprintf(stderr, "cd: %s: No such file or directory\n", dir);
        return EXIT_FAILURE;
    }
    errno = 0;
    return EXIT_SUCCESS;
}

int get_command(char *args[], int size, char *command)
{
    int j;
    int bg = 0;
    /* Check for & */
    if (strcmp(args[size - 1], "&") == 0)
    {
        size = size - 1;
        args[size] = NULL;
        bg = 1;
    }
    /* Save command */
    strcpy(command, args[0]);
    for (j = 1; j < size; j++)
    {

        strcat(command, " ");
        strcat(command, args[j]);
    }
    return bg;
}

int main(int argk, char *argv[], char *envp[])
/* argk - number of arguments */
/* argv - argument vector from command line */
/* envp - environment pointer */

{
    int fork_status; /* value returned by fork sys call */
    int exec_status; /* value returned by execv call */
    // int wpid;          /* value returned by wait */
    char *args[NUMTOKENS]; /* array of pointers to command line tokens */
    char *sep = " \t\n";   /* command line token separators    */
    int size;              /* parse index */
    int bg = 0;
    char command[FILENAME_MAX];

    /* prompt for and process one command line at a time  */
    while (1)
    { /* do Forever */
        /* Tokenise input command */
        prompt();
        fgets(line, INSIZE, stdin);
        fflush(stdin);

        if (feof(stdin))
        { /* non-zero on EOF  */
            // fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(),
            // feof(stdin), ferror(stdin));
            exit(0);
        }
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
            continue; /* to prompt */

        /* Split line to tokens */
        args[0] = strtok(line, sep);
        for (size = 1; size < NUMTOKENS; size++)
        {
            args[size] = strtok(NULL, sep);
            if (args[size] == NULL)
                break;
        }
        bg = get_command(args, size, command);

        /* execute cd */
        if ((strcmp(args[0], "cd") == 0) && (bg == 0))
        {
            exec_status = execute_cd(args[0], args);
            continue;
        }

        /* fork a child process to exec the command in v[0] */
        switch (fork_status = fork())
        {
        case -1: /* fork returns error to parent process */
        {
            perror("Fork failed");
            break;
        }
        case 0: /* code executed only by child process */
        {
            if (strcmp(args[0], "cd") == 0)
                exec_status = execute_cd(args[0], args);
            else
                exec_status = execvp(args[0], args);

            if (exec_status != 0)
            {
                perror("Forked process status failed. Child process terminated");
                exit(0);
                ;
            }
        }
        default: /* code executed only by parent process */
        {
            waitpid(0, 0, 0);
        }
        } /* switch */
    }     /* while */
    return 0;
} /* main */