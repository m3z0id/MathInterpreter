#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include "DataTypes.h"
#include "CustomMath.h"

int BUF_LEN = 1024;

void printError(char* err, char* input) {
    free(input);
    fprintf(stdout, "%s", err);
    exit(1);
}

char** splitUp(char* str, int* len, char* delim) {
    int words = getSpaceCount(str, strlen(str)) + 1;
    char** rawTokens = calloc(words, sizeof(char*));

    *len = 0;
    char *token = strtok(str, delim);
    while (token != NULL) {
        rawTokens[*len] = strdup(token); 
        (*len)++;
        token = strtok(NULL, delim);
    }

    return rawTokens;
}

void freeStrArr(char **arr, int len) {
    if (!arr) return;
    for (int i = 0; i < len; i++) {
        free(arr[i]);
    }
    free(arr);
}

char** getInput() {
    char* buf = calloc(BUF_LEN + 1, sizeof(char));
    fprintf(stdout, "Enter a math expression: ");
    fgets(buf, sizeof(char) * BUF_LEN, stdin);
    buf[BUF_LEN] = 0;
    buf[strcspn(buf, "\n")] = 0;

    char* temp = realloc(buf, sizeof(char) * (strlen(buf) + 1));
    if (!temp) {
        fprintf(stderr, "Out of memory\n");
        free(buf);
        exit(1);
    }
    buf = temp;
    char** wrapper = malloc(sizeof(char*));
    wrapper[0] = buf;
    return wrapper;
}

char** getFileInput(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Could not open file %s\n", filename);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    BUF_LEN = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buf = calloc(BUF_LEN + 1, sizeof(char));
    if (!buf) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        exit(1);
    }

    fread(buf, sizeof(char), BUF_LEN, file);
    buf[BUF_LEN] = 0;
    fclose(file);

    int lines = 0;
    char** result = splitUp(buf, &lines, "\n");
    free(buf);
    return result;
}

Token* tokenize(char* input, int* len) {
    *len = 0;
    char** rawTokens = splitUp(input, len, " ");
    Token* tokenArr = calloc(*len, sizeof(Token));

    for(int i = 0; i < *len; i++) {
        Token token = tokenFromString(rawTokens[i]);
        tokenArr[i] = token;
    }

    for (int i = 0; i < *len; i++) {
        free(rawTokens[i]);
    }
    free(rawTokens);

    return tokenArr;
}

void validate(Token* tokenArr, int* len) {
    if (tokenArr == NULL) {
        free(tokenArr);
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    int danglingParenCount = 0;
    if ((tokenArr[0].type == SUBTRACT || tokenArr[0].type == ADD) && tokenArr[1].type == LPAREN) {
        insertAt(&tokenArr, len, initToken(NUMBER), 0);
    } else if (!(tokenArr[0].type == NUMBER || tokenArr[0].type == LPAREN)) {
        fprintf(stderr, "Expression can't start with an operator!\n");
        exit(1);
    }

    if (!(tokenArr[*len-1].type == NUMBER || tokenArr[*len-1].type == RPAREN)) {
        fprintf(stderr, "Expression can't end with an operator!\n");
        exit(1);
    }

    for (int i = 0; i < *len; i++) {
        if (tokenArr[i].type == -1) {
            fprintf(stderr, "Invalid token at %d\n", i);
            exit(1);
        }
        if (tokenArr[i].type == LPAREN) {
            danglingParenCount++;
        } else if (tokenArr[i].type == RPAREN) {
            danglingParenCount--;
        }

        if (i > 0) {
            if (tokenArr[i].type == NUMBER && tokenArr[i-1].type == NUMBER) {
                fprintf(stderr, "Invalid token at %d, must not be a number.\n", i);
                exit(1);
            }
            if (tokenArr[i].type == RPAREN && !(tokenArr[i-1].type == NUMBER || tokenArr[i-1].type == RPAREN)) {
                fprintf(stderr, "Invalid token at %d, must not be a right parenthesis.\n", i);
                exit(1);
            }
            if (tokenArr[i].type >= ADD && tokenArr[i].type <= FACTOR) {
                if (tokenArr[i-1].type >= ADD && tokenArr[i-1].type <= LPAREN) {
                    fprintf(stderr, "Invalid token at %d, there can't be double operands.\n", i);
                    exit(1);
                }
            }

            if (tokenArr[i].type == LPAREN && (tokenArr[i-1].type == RPAREN || tokenArr[i-1].type == NUMBER)) {
                insertAt(&tokenArr, len, initToken(MULTIPLY), i);
                i++;
            } else if (tokenArr[i].type == RPAREN && tokenArr[i-1].type == LPAREN) {
                insertAt(&tokenArr, len, initToken(MULTIPLY), i);
                i++;
            } else if (tokenArr[i].type == NUMBER && tokenArr[i-1].type == RPAREN) {
                insertAt(&tokenArr, len, initToken(MULTIPLY), i);
                i++;
            }
        }
    }
    if (danglingParenCount != 0) {
        fprintf(stderr, "Unmatched parentheses\n");
        exit(1);
    }
}

void handleExit(int sig) {
    fprintf(stderr, "\nExiting...\n");
    exit(0);
}

int main(int argc, char* argv[]) {
    if (!(argc == 1 || argc == 3)) {
        fprintf(stderr, "Usage: %s -i <filename>\n", argv[0]);
        return 1;
    }
    char** buf;
    int isStdin = false;
    signal(SIGINT, handleExit);

input:
    if (argc == 3 && strcmp(argv[1], "-i") == 0) {
        buf = getFileInput(argv[2]);
    } else if (argc == 1) {
        isStdin = true;
        buf = getInput();
    } else {
        fprintf(stderr, "Usage: %s -i <filename>\n", argv[0]);
        return 1;
    }

    int bufLen = getStrArrLen(buf);
    for (int i = 0; i < bufLen; i++) {
        int len = 0;
        Token *tokens = tokenize(buf[i], &len);

        validate(tokens, &len);
        fprintf(stdout, "%d. The result is probably %g\n", i+1, calculate(tokens, &len).val);

        if (isStdin) {
            free(buf);
            goto input;
        }

        free(tokens);
    }

    if (!isStdin) {
        freeStrArr(buf, bufLen);
    } else {
        free(buf);
    }
    return 0;
}
