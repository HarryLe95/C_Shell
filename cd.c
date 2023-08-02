#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef FILENAME_MAX
#define FILENAME_MAX 4096
#endif

#define HOME getenv("HOME")
#define UHOME getpwuid(getuid())->pw_dir
#define TRUE 1
#define FALSE 0
#define PATHNUM_MAX 100

int _dir_exists(char *);
void _process_CDPATH(char *, char *, char *);
void _process_path(char *, char *, char *);
void _execute_cd(char *, char *);
void cd(char *, char *);

int main(void) {
    // char curpath[FILENAME_MAX];
    // _process_CDPATH(
    //     ".:/mnt/c/Users/harry/Downloads/OS/T2:.:/mnt/c/Users/harry/Downloads/"
    //     "OS/"
    //     "T1",
    //     "E", curpath);
    // printf("%s\n", curpath);
    char path[100];
    _execute_cd("slink", NULL);
    getcwd(path, 100);
    printf("%s\n", path);
}

/* Check if a directory exist */
int _dir_exists(char *dir) {
    struct stat sb;
    if (dir == NULL) return FALSE;
    if (stat(dir, &sb) == 0 && S_ISDIR(sb.st_mode)) {
        return TRUE;
    }
    return FALSE;
}

void _process_CDPATH(char *CDPATH, char *dir, char *curpath) {
    char *sep = ":"; /* CDPATH separator */
    char *token;
    char path[FILENAME_MAX];

    /* Handle NULL CDPATH */
    if (CDPATH == NULL) {
        _process_path(NULL, dir, path);
        if (_dir_exists(path)) {
            strcpy(curpath, path);
            return;
        }
    }

    /* Non NULL CDPATH */
    char CDPATH_CPY[FILENAME_MAX];
    /* Copy to avoid modifying the original string */
    strcpy(CDPATH_CPY, CDPATH);
    /* Split to subpaths */
    token = strtok(CDPATH_CPY, sep);
    /* Split to subpaths */
    do {
        _process_path(token, dir, path);
        if (_dir_exists(path)) {
            strcpy(curpath, path);
            break;
        }
        token = strtok(NULL, sep);
    } while (token != NULL);
}

/*
    If the pathname is non-null, pathname is pathname + dir if pathname
    ends with '/' otherwise pathname + '/' + dir

    If the pathname is null, pathname is './' + dir
*/
void _process_path(char *path, char *dir, char *dest) {
    if (path == NULL) {
        strcpy(dest, "./");
    } else {
        strcpy(dest, path);
        int len = strlen(path);
        if (path[len - 1] != '/') {
            strcat(dest, "/");
        }
    }
    strcat(dest, dir);
}

void _execute_cd(char *curpath, char *option) {
    int status;
    status = chdir(curpath);

    if (status == 0) {
        char pwd[FILENAME_MAX];
        sprintf(pwd, "PWD=%s", curpath);
        putenv(pwd);
    } else {
        fprintf(stderr, "%s: No such file or directory\n", curpath);
    }
}

/* Manual link:
 * https://pubs.opengroup.org/onlinepubs/009695299/utilities/cd.html */
void cd(char *dir, char *option) {
    int cd_status;
    char *curpath;
    char *CDPATH = getenv("CDPATH");
    /* Step 1 and 2 in the manual */
    if (dir == NULL) {
        if (HOME == NULL) {
#ifdef DEBUG
            printf(
                "1. No directory operand is given and the HOME environment "
                "variable "
                "is empty or undefined, go to User Home Directory\n");
#endif
            curpath = UHOME;
        } else {
#ifdef DEBUG
            printf(
                "2. No directory operand is given and the HOME environment "
                "variable "
                "is defined, go to Home Directory\n");
#endif
            curpath = HOME;
        }
    }
    /* Step 3 in the manual*/
    if (dir[0] == '/') {
#ifdef DEBUG
        printf(
            "3. If the directory operand begins with a <slash> character, set "
            "curpath to the operand and proceed to step 7.");
#endif
        curpath = dir;
    } else if (dir[0] == '.' || (dir[0] == '.' && dir[1] == '.')) {
#ifdef DEBUG
        printf(
            "4. If the first component of the directory operand is dot or "
            "dot-dot, "
            "proceed to step 6.");
        printf("6. Set curpath to the directory operand.");
#endif
    } else {
    }
}