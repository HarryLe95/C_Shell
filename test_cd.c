#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "cd.h"

struct stat st = {0};

void force_mkdir(char* dir) {
    if (stat(dir, &st) == -1) {
        mkdir(dir, 0700);
    }
}

void dotdot_handleTest(char* input, char* output) {
    char dest[FILENAME_MAX];
    strcpy(dest, "");
    handle_dotdot(input, dest);

#ifdef DEBUG
    printf("Call to handle_dotdot is successful\n");
#endif

    char fail_msg[10000], success_msg[10000];
    sprintf(fail_msg, "Test failed: output: %s, expected: %s\n", dest, output);
    sprintf(success_msg, "Test passed for input: %s\n", input);

    if (strcmp(dest, "")==0) {
        if (output != NULL) {
            fprintf(stderr, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET, fail_msg);
        }
        printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, success_msg);
        return;
    }

    if (output == NULL) {
        if (dest != NULL && strlen(dest) != 0) {
            fprintf(stderr, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET, fail_msg);
            return;
        }
        printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, success_msg);
        return;
    }
    if (strcmp(dest, output) != 0) {
        fprintf(stderr, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET, fail_msg);
        return;
    }
    printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, success_msg);
}

void test_dotdot() {
    printf(ANSI_COLOR_GREEN
           "Testing function handle_dotdot\n" ANSI_COLOR_RESET);
    dotdot_handleTest("/A/B/C", "/A/B/C");
    dotdot_handleTest("A/B/C/..", "A/B");
    dotdot_handleTest("A/..", NULL);
    dotdot_handleTest("./A", "A");
    dotdot_handleTest("..", "..");
    dotdot_handleTest("/..", "/..");
    dotdot_handleTest("../..", "../..");
    dotdot_handleTest("/.././A", "/../A");
    dotdot_handleTest("A/./B/./..", "A");
    dotdot_handleTest("A///././//B/./..", "A");
    dotdot_handleTest("/mnt/c/Users/harry/Downloads/OS/..",
                      "/mnt/c/Users/harry/Downloads");

    printf("\n\n\n");
}

void CDPATH_handleTest(char* dir, char* CDPATH, char* PWD, char* output) {
    char dest[FILENAME_MAX];
    char real[FILENAME_MAX];
    strcpy(dest, "");
    process_CDPATH(CDPATH, PWD, dest, dir);

#ifdef DEBUG
    printf("Call to process_CDPATH is successful\n");
#endif
    char fail_msg[10000], success_msg[10000];
    sprintf(fail_msg, "Test failed: output: %s, expected: %s, CDPATH: %s\n",
            dest, output, CDPATH);
    sprintf(success_msg, "Test passed for dir: %s, CDPATH: %s, PWD: %s\n", dir,
            CDPATH, PWD);

    if (dest == NULL) {
        if (output != NULL) {
            fprintf(stderr, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET, fail_msg);
        }
        printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, success_msg);
        return;
    }

    if (output == NULL) {
        if (dest != NULL && strlen(dest) != 0) {
            fprintf(stderr, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET, fail_msg);
            return;
        }
        printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, success_msg);
        return;
    }
    realpath(dest, real);
    if (strcmp(real, output) != 0) {
        fprintf(stderr, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET, fail_msg);
        return;
    }
    printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, success_msg);
}

void test_CDPATH() {
    printf(ANSI_COLOR_GREEN
           "Testing function process_CDPATH\n" ANSI_COLOR_RESET);
    char* PWD = getenv("PWD");
    /* Initialise test directory */
    char pT1[FILENAME_MAX];
    char pT2[FILENAME_MAX];
    char pA[FILENAME_MAX];
    char pB[FILENAME_MAX];
    char pC[FILENAME_MAX];
    char pT1A[FILENAME_MAX];
    char pT2A[FILENAME_MAX];
    char pT2B[FILENAME_MAX];
    char cdpathT1[FILENAME_MAX * 2];
    char cdpathT2[FILENAME_MAX * 2];
    char cdpathT12[FILENAME_MAX * 2];
    char cdpathT21[FILENAME_MAX * 2];
    force_mkdir("T1");
    force_mkdir("T1/A");
    force_mkdir("T2");
    force_mkdir("T2/A");
    force_mkdir("T2/B");
    force_mkdir("A");
    force_mkdir("B");
    force_mkdir("C");
    sprintf(pT1, "%s/T1", PWD);
    sprintf(pT2, "%s/T2", PWD);
    sprintf(pA, "%s/A", PWD);
    sprintf(pB, "%s/B", PWD);
    sprintf(pC, "%s/C", PWD);
    sprintf(pT1A, "%s/T1/A", PWD);
    sprintf(pT2A, "%s/T2/A", PWD);
    sprintf(pT2B, "%s/T2/B", PWD);
    sprintf(cdpathT1, "%s", pT1);
    sprintf(cdpathT2, "%s", pT2);
    sprintf(cdpathT12, "%s:%s", pT1, pT2);
    sprintf(cdpathT21, "%s:%s", pT2, pT1);

    /* Run test cases */
    CDPATH_handleTest("A", "", PWD, pA);
    CDPATH_handleTest("A", NULL, PWD, pA);
    CDPATH_handleTest("A", ":", PWD, pA);
    CDPATH_handleTest("A", cdpathT1, PWD, pT1A);
    CDPATH_handleTest("A", cdpathT12, PWD, pT1A);
    CDPATH_handleTest("A", cdpathT2, PWD, pT2A);
    CDPATH_handleTest("A", cdpathT21, PWD, pT2A);

    CDPATH_handleTest("B", cdpathT12, PWD, pT2B);
    CDPATH_handleTest("B", cdpathT21, PWD, pT2B);
    CDPATH_handleTest("B", cdpathT2, PWD, pT2B);
    CDPATH_handleTest("B", cdpathT1, PWD, pB);

    CDPATH_handleTest("C", cdpathT12, PWD, pC);
    CDPATH_handleTest("C", cdpathT21, PWD, pC);
    CDPATH_handleTest("C", cdpathT2, PWD, pC);
    CDPATH_handleTest("C", cdpathT1, PWD, pC);

    /* Remove test directories */
    rmdir(pT1A);
    rmdir(pT1);
    rmdir(pT2A);
    rmdir(pT2B);
    rmdir(pT2);
    rmdir(pA);
    rmdir(pB);
    rmdir(pC);

    printf("\n\n\n");
}

void cd_handleTest(char* dir, char* option, char* exp_old, char* exp_new) {
    cd(dir, option);
    char* pwd = getenv("PWD");
    char* oldpwd = getenv("OLDPWD");

    char fail_old[10000], success_old[10000], fail_new[10000],
        success_new[10000];
    sprintf(
        fail_old,
        "Test failed OLDPWD: output: %s, expected: %s, dir: %s, option: %s\n",
        oldpwd, exp_old, dir, option);
    sprintf(fail_new,
            "Test failed PWD: output: %s, expected: %s, dir: %s, option: %s\n",
            pwd, exp_new, dir, option);
    sprintf(success_old, "Test passed OLDPWD: dir: %s, option: %s\n", dir,
            option);
    sprintf(success_new, "Test passed PWD: dir: %s, option: %s\n", dir, option);

    if (strcmp(pwd, exp_new) == 0)
        printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, success_new);
    else
        printf(ANSI_COLOR_RED "%s" ANSI_COLOR_RESET, fail_new);

    if (strcmp(oldpwd, exp_old) == 0)
        printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, success_old);
    else
        printf(ANSI_COLOR_RED "%s" ANSI_COLOR_RESET, fail_old);
}

void test_cd() {
    force_mkdir("A");
    symlink(getenv("HOME"), "slink");
    char* pwd = getenv("PWD");
    char* home = getenv("HOME");
    char prev[FILENAME_MAX];
    char pA[FILENAME_MAX];
    char pS[FILENAME_MAX];

    realpath("../", prev);
    realpath("A", pA);
    sprintf(pS, "%s/slink", pwd);

    cd_handleTest("..", "-L", pwd, prev);
    cd_handleTest("-", "-L", prev, pwd);
    cd_handleTest("~", "-L", pwd, home);
    cd_handleTest("-", "-L", home, pwd);
    cd_handleTest("./A", "-L", pwd, pA);
    cd_handleTest("..", "-L", pA, pwd);
    cd_handleTest("././A", "-L", pwd, pA);
    cd_handleTest("..", "-L", pA, pwd);
    cd_handleTest("-", "-L", pwd, pA);
    cd_handleTest("..", "-L", pA, pwd);
    cd_handleTest("A", "-L", pwd, pA);
    cd_handleTest("..", "-L", pA, pwd);
    cd_handleTest("slink", "-L", pwd, pS);
    cd_handleTest(getenv("OLDPWD"), "-L", pS, pwd);
    cd_handleTest("slink", "-P", pwd, home);
    cd_handleTest(getenv("OLDPWD"), "-L", home, pwd);

    /*Clean up*/
    unlink("slink");
    rmdir("A");
}

void execute_cd_handleTest(char* args){
    char option[10000];
    int status;
    char* sep = " \t\n";
    strcpy(option, args);
    char *argv[1000];
    int index = 0; 
    
    argv[index] = strtok(option, sep);

    while(argv[index]!=NULL){
        index = index + 1;
        argv[index] = strtok(NULL, sep);
    }
    
    status = execute_cd("cd", argv);
    if (status != 0)
        perror("");
}

int main(void) {
    test_dotdot();
    test_CDPATH();
    test_cd();
    // execute_cd_handleTest("cd -PL ~/Desktop");
}