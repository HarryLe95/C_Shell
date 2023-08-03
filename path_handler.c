#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef FILENAME_MAX
#define FILENAME_MAX 4096
#endif

#define SEP "/"

typedef struct Token {
    char* value;
    struct Token* prev;
    struct Token* next;
} Token;

Token* _createToken(char* value) {
    if (value == NULL) {
        return NULL;
    }
    Token* new_token = (Token*)malloc(sizeof(Token));
    new_token->value = value;
    new_token->prev = NULL;
    new_token->next = NULL;
    return new_token;
}

void _deleteToken(Token* token) {
    if (token == NULL) return;
    _deleteToken(token->next);
#ifdef DEBUG
    printf("Free %s\n", token->value);
#endif
    free(token);
}

void _joinToken(Token* node, char* dest) {
    if (node == NULL) return;
    if (dest == NULL || strlen(dest) == 0) {
        sprintf(dest, "%s", node->value);
    } else {
        sprintf(dest, "%s/%s", dest, node->value);
    }
    _joinToken(node->next, dest);
}

void handle_dotdot(char* path, char* dest) {
    /* Handle NULL input */
    if (path == NULL) {
        dest = NULL;
        return;
    }
    /* Check if path has a leading / */
    int leading_slash = 0;
    if (path[0] == '/') leading_slash = 1;

    /* Tokenise using separator*/
    char cp_path[FILENAME_MAX];
    strcpy(cp_path, path);

    Token* root = _createToken(strtok(cp_path, SEP));
    Token* current_token = root;
    Token* previous_token = NULL;
    Token* next_token = NULL;

    while (current_token != NULL) {
        next_token = _createToken(strtok(NULL, SEP));
        previous_token = current_token;
        current_token = next_token;
        previous_token->next = next_token;
        if (next_token != NULL) next_token->prev = previous_token;
    }

#ifdef DEBUG
    printf("Pass tokenisation\n");
#endif

    /* Remove . and .. */
    current_token = root;
    previous_token = NULL;
    next_token = NULL;
    while (current_token != NULL) {
        next_token = current_token->next;
        /* Handle . */
        if (strcmp(current_token->value, ".") == 0) {
#ifdef DEBUG
            printf("Processing %s\n", current_token->value);
#endif
            if (next_token != NULL) next_token->prev = previous_token;
            if (previous_token != NULL) previous_token->next = next_token;
            current_token->next = NULL;
            if (current_token == root) root = next_token;
            _deleteToken(current_token);
            current_token = NULL;
        }

        /* Handle .. */
        if (current_token != NULL) {
            if (strcmp(current_token->value, "..") == 0) {
                /* There is previous token that is not .. */
#ifdef DEBUG
                printf("Processing %s\n", current_token->value);
#endif
                if (previous_token != NULL &&
                    strcmp(previous_token->value, "..") != 0) {
                    Token* temp = previous_token->prev;
                    if (next_token != NULL) next_token->prev = temp;
                    if (temp != NULL) temp->next = next_token;
                    current_token->next = NULL;
                    if (previous_token == root) root = next_token;
                    _deleteToken(previous_token);
                    current_token = NULL;
                    previous_token = temp;
                }
            }
        }

        /* Reset pointers */
        if (current_token != NULL) previous_token = current_token;
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

#endif
    /* Join tokens */
    if (root == NULL) {
        strcpy(dest, "");
    } else {
        _joinToken(root, dest);
    }
    if (leading_slash) { /* Give back leading slash if there is*/
        memmove(dest + 1, dest, strlen(dest) + 1);
        memcpy(dest, "/", 1);
    }

    /* Free memory */
    _deleteToken(root);

#ifdef DEBUG
    printf("Final string: %s\n", dest);
#endif
}

void _handleTest(char* input, char* output) {
    char dest[FILENAME_MAX];
    handle_dotdot(input, dest);

#ifdef DEBUG
    printf("Call to handle_dotdot is successful\n");
#endif

    char msg[10000];
    sprintf(msg, "Test failed: output: %s, expected: %s\n", dest, output);

    if (dest == NULL) {
        if (output != NULL) {
            fprintf(stderr, msg);
        }
        printf("Test passed\n");
        return;
    }

    if (output == NULL) {
        if (dest != NULL && strlen(dest) != 0) {
            fprintf(stderr, msg);
            return;
        }
        printf("Test passed\n");
        return;
    }
    if (strcmp(dest, output) != 0) {
        fprintf(stderr, msg);
        return;
    }
    printf("Test passed\n");
}

void test_dotdot() {
    _handleTest("/A/B/C", "/A/B/C");
    _handleTest("A/B/C/..", "A/B");
    _handleTest("A/..", NULL);
}

int main() { test_dotdot(); }
