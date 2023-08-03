#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    printf("PWD=%s; OLDPWD=%s\n", getenv("PWD"), getenv("OLDPWD"));
    char old[100];
    char new[100];
    char* PWD = "A";
    char* OLDPWD = "B";

    sprintf(old, "OLDPWD=%s", OLDPWD);
    putenv(old);
    sprintf(new, "PWD=%s", PWD);
    putenv(new);
    printf("PWD=%s; OLDPWD=%s\n", getenv("PWD"), getenv("OLDPWD"));
}