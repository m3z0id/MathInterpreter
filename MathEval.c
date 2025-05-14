#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include "DataTypes.h"
#include "CustomMath.h"

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

char* getInput() {
    fprintf(stdout, "Enter a math expression: ");

    char* buf = NULL;
    int len = 0;

    int c;
    while ((c = getchar()) != '\n' && c != '\r' && c != EOF) {
        char* temp = realloc(buf, len + 2);
        if (!temp) {
            fprintf(stderr, "Out of memory\n");
            free(buf);
            exit(1);
        }
        buf = temp;
        buf[len++] = (char)c;
        buf[len] = 0;
    }

    buf[strcspn(buf, "\r")] = 0;
    buf[strcspn(buf, "\n")] = 0;

    return buf;
}

long goToNextNl(FILE *file) {
    char c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            return ftell(file);
        }
    }
    fseek(file, 0, SEEK_END);
    return ftell(file);
}

char* readNextLine(FILE* file) {
    if (!file) {
        fprintf(stderr, "This file doesn't exist\n");
        fclose(file);
        exit(1);
    }

    long origLocation = ftell(file);
    long nlLocation = goToNextNl(file);

    fseek(file, 0, SEEK_END);
    if (origLocation == ftell(file)) return NULL;
    fseek(file, origLocation, SEEK_SET);

    long lineLen = nlLocation - origLocation;
    char* buf = calloc(lineLen + 1, sizeof(char));
    if (!buf) {
        fprintf(stderr, "Out of memory\n");
        fclose(file);
        exit(1);
    }

    fread(buf, sizeof(char), lineLen, file);
    buf[lineLen] = 0;
    buf[strcspn(buf, "\n")] = 0;

    return buf;
}

FILE* openFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "File not found\n");
        exit(1);
    }
    return file;
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
    if ((tokenArr[0].type == SUBTRACT || tokenArr[0].type == ADD) && (tokenArr[1].type == LPAREN || tokenArr[1].type == NUMBER)) {
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
                if (tokenArr[i-1].type >= ADD && tokenArr[i-1].type <= LPAREN && tokenArr[i].type != SUBTRACT) {
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
    char* buf;
    int isStdin = false;
    signal(SIGINT, handleExit);

input:
    FILE* file = NULL;
    if (argc == 3 && strcmp(argv[1], "-i") == 0) {
        file = openFile(argv[2]);
        buf = readNextLine(file);

    } else if (argc == 1) {
        isStdin = true;
        buf = getInput();
    } else {
        fprintf(stderr, "Usage: %s -i <filename>\n", argv[0]);
        return 1;
    }

    int counter = 0;
    while (buf != NULL) {
        counter++;
        int len = 0;
        Token* tokenArr = tokenize(buf, &len);
        free(buf);

        validate(tokenArr, &len);
        fprintf(stdout, "%d: The result is %g\n", counter, calculate(tokenArr, &len).val);

        if (isStdin) {
            buf = getInput();
        } else {
            buf = readNextLine(file);
        }
    }

    fclose(file);

    return 0;
}
