#pragma once
#include "Helpers/DataTypes.h"
#include "Helpers/AdditionalString.h"
#include <stdio.h>
#include <stdlib.h>

Token tokenFromString(char* str) {
    enum TokenType type = getType(str);
    Token token = initToken(type);

    if(type != NUMBER) return token;
    char* endptr;
    token.val = strtod(str, &endptr);
    return token;
}

Token* oneCharToken(Token* arr, int* len, char c) {
    char token[2] = {c, 0};
    arr = realloc(arr, ++*len * sizeof(Token));
    arr[*len - 1] = tokenFromString(token);
    return arr;
}

Token* tokenize(char* input, int* tokArrLen) {
    *tokArrLen = 0;
    Token* arr = calloc(0, sizeof(Token));

    char currentChar;
    int pos = 0;
    while ((currentChar = *input++) != 0) {
        if (currentChar == ' ') continue;
        if (currentChar == '(' || currentChar == ')' || currentChar == '+' || currentChar == '-' || currentChar == '*' || currentChar == '/' || currentChar == '%' || currentChar == '^') {
            arr = oneCharToken(arr, tokArrLen, currentChar);
        } else if (isdigit(currentChar)) {
            int len = 1;
            char* number = calloc(len, sizeof(char));
            number[0] = currentChar;

            int foundDecimal = false;
            while (isdigit(currentChar = *input++) || isDecimal(currentChar)) {
                if (isDecimal(currentChar) && !foundDecimal) {
                    foundDecimal = true;
                    currentChar = '.';
                } else if (isDecimal(currentChar) && foundDecimal) {
                    fprintf(stderr, "Invalid token at %d, too many decimal points.\n", pos);
                    exit(1);
                }
                number = realloc(number, ++len * sizeof(char));
                number[len-1] = currentChar;
            }
            *input--;
            number = realloc(number, ++len * sizeof(char));
            number[len-1] = 0;

            arr = realloc(arr, ++*tokArrLen * sizeof(Token));
            arr[*tokArrLen - 1] = tokenFromString(number);
        } else if (currentChar == 'r') {
            if ((currentChar = *input++) == 'o' && (currentChar = *input++) == 'o' && (currentChar = *input++) == 't') {
                Token token = tokenFromString("root");
                arr = realloc(arr, ++*tokArrLen * sizeof(Token));
                arr[*tokArrLen - 1] = token;
            }
        }
        pos++;
    }
    return arr;
}

/*Token* tokenize(char* input, int* len) {
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
}*/

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