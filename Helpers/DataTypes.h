#pragma once
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Helpers/AdditionalString.h"
#define true 1
#define false 0

enum TokenType {
    ADD = 1,
    SUBTRACT = 2,
    MULTIPLY = 3,
    DIVIDE = 4,
    MOD = 5,
    ROOT = 6,
    FACTOR = 7,
    LPAREN = 8,
    RPAREN = 9,
    NUMBER = 10
};

int isNumber(const char* str) {
    char* iter = str;
    int seenDelim = false;
    while(*iter != 0) {
        if(isDecimal(*iter) && !seenDelim) {
            seenDelim = true;
            *iter = '.';
            iter++;
            continue;
        }
        if (isDecimal(*iter)) return false;
        if(!isdigit(*iter)) return false;
        iter++;
    }
    return true;
}

enum TokenType getType(const char* str) {
    if(strcmp("+", str) == 0) return ADD;
    if(strcmp("-", str) == 0) return SUBTRACT;
    if(strcmp("*", str) == 0) return MULTIPLY;
    if(strcmp("/", str) == 0) return DIVIDE;
    if(strcmp("^", str) == 0) return FACTOR;
    if(strcmp("(", str) == 0) return LPAREN;
    if(strcmp(")", str) == 0) return RPAREN;
    if(strcmp("%", str) == 0) return MOD;
    if(strcmp("root", str) == 0) return ROOT;
    if(isNumber(str)) return NUMBER;

    return -1;
}

int getPriority(const enum TokenType type) {
    if(type == NUMBER) return -1;
    if(type == ADD || type == SUBTRACT) return 1;
    if(type == MULTIPLY || type == DIVIDE || type == MOD) return 2;
    if(type == ROOT || type == FACTOR) return 3;
    if(type == LPAREN || type == RPAREN) return 4;

    return -2;
}

typedef struct Token {
    enum TokenType type;
    int priority;
    double val;
} Token;

void printTokenArray(Token* arr, int len) {
    for(int i = 0; i < len; i++) {
        if (arr[i].type == NUMBER) {
            fprintf(stdout, "Token Type: %d; Token Value: %g\n", arr[i].type, arr[i].val);
        } else {
            fprintf(stdout, "Token Type: %d; Token Priority: %d\n", arr[i].type, arr[i].priority);
        }
    }
    fprintf(stdout, "\n");
}

Token initToken(enum TokenType type) {
    Token token;
    token.type = type;
    token.priority = getPriority(type);
    token.val = 0;

    return token;
}

void insertAt(Token** arr, int* size, Token value, int index) {
    *arr = realloc(*arr, (*size + 1) * sizeof(Token));
    if (!*arr) {
        fprintf(stderr, "Out of memory\n");
        free(*arr);
        free(arr);
        exit(1);
    }
    (*arr)[*size] = initToken(0);

    for (int j = *size; j > index; j--) {
        (*arr)[j] = (*arr)[j - 1];
    }

    (*arr)[index] = value;
    (*size)++;
}

int getEntriesNumber(Token* arr, int size) {
    int count = 0;
    for (Token* iter = arr; iter <= arr + size; iter++) {
        if (iter->type >= ADD && iter->type <= NUMBER) count++;
    }
    return count;
}