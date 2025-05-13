#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "DataTypes.h"
#include "CustomMath.h"

int BUF_LEN = 1024;

char* getInput() {
    char* buf = malloc(sizeof(char) * (BUF_LEN + 1));
    fprintf(stdout, "Enter a math expression: ");
    fgets(buf, sizeof(char) * BUF_LEN, stdin);
    buf[BUF_LEN] = 0;
    buf[strcspn(buf, "\n")] = 0;

    buf = realloc(buf, sizeof(char) * (strlen(buf) + 1));
    return buf;
}

void printError(char* err, char* input) {
    free(input);
    fprintf(stdout, "%s", err);
    exit(1);
}

char** splitUp(char* str, int* len) {
    char** rawTokens = malloc(sizeof(char*) * BUF_LEN);

    *len = 0;
    char *token = strtok(str, " ");
    while (token != NULL) {
        rawTokens[*len] = strdup(token); 
        (*len)++;
        token = strtok(NULL, " ");
    }

    return rawTokens;
}

Token* tokenize(char* input, int* len) {
    *len = 0;
    char** rawTokens = splitUp(input, len);
    Token* tokenArr = malloc(*len * sizeof(Token));

    for(int i = 0; i < *len; i++) {
        Token token = tokenFromString(rawTokens[i]);
        tokenArr[i] = token;
    }

    free(rawTokens);

    return tokenArr;
}

void validate(Token* tokenArr, int* len) {
    int danglingParenCount = 0;
    if (!(tokenArr[0].type == NUMBER || tokenArr[0].type == LPAREN)) {
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
                insertAt(tokenArr, len, initToken(MULTIPLY), i);
                i++;
            } else if (tokenArr[i].type == RPAREN && tokenArr[i-1].type == LPAREN) {
                insertAt(tokenArr, len, initToken(MULTIPLY), i);
                i++;
            } else if (tokenArr[i].type == NUMBER && tokenArr[i-1].type == RPAREN) {
                insertAt(tokenArr, len, initToken(MULTIPLY), i);
                i++;
            }
        }
    }
    if (danglingParenCount != 0) {
        fprintf(stderr, "Unmatched parentheses\n");
        exit(1);
    }
}

int main() {
    char* buf = getInput();
    int len = 0;
    Token* tokens = tokenize(buf, &len);

    validate(tokens, &len);

    //printTokenArray(tokens, len);

    fprintf(stdout, "The result is probably %g", calculate(tokens, &len).val);

    free(buf);
}