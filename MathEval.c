#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include "Helpers/DataTypes.h"
#include "Helpers/CustomMath.h"
#include "Lexer.h"
#include "Input.h"

void printError(char* err, char* input) {
    free(input);
    fprintf(stdout, "%s", err);
    exit(1);
}

void freeStrArr(char **arr, int len) {
    if (!arr) return;
    for (int i = 0; i < len; i++) {
        free(arr[i]);
    }
    free(arr);
}

void handleExit(int sig) {
    fprintf(stderr, "\nExiting...\n");
    exit(0);
}

void doCalc(char* buf, int* len, int* counter) {
    Token* tokenArr = tokenize(buf, len);

    validate(tokenArr, len);
    fprintf(stdout, "%d. The result is %g\n", ++*counter, calculate(tokenArr, len).val);
}

int main(int argc, char* argv[]) {
    if (!(argc == 1 || argc == 3)) {
        fprintf(stderr, "Usage: %s -i <filename>\n", argv[0]);
        return 1;
    }
    char* buf;
    int isStdin = false;
    signal(SIGINT, handleExit);
    
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
        int arrLen = 0;

        if (isStdin) {
            char** multiple = splitUp(buf, &arrLen, ";");
            for (int i = 0; i < arrLen; i++) {
                int len = 0;
                char* current = multiple[i];
                doCalc(current, &len, &counter);
                free(current);
            }
            free(multiple);
            multiple = NULL;
        } else {
            doCalc(buf, &arrLen, &counter);
            free(buf);
        }

        if (isStdin) {
            buf = getInput();
            counter = 0;
        } else {
            buf = readNextLine(file);
        }
    }

    if (!isStdin) fclose(file);

    return 0;
}
