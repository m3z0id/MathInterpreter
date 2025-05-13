#pragma once
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define true 1
#define false 0

enum TokenType {
    ADD = 1,
    SUBTRACT = 2,
    MULTIPLY = 3,
    DIVIDE = 4,
    MOD = 5,
    FACTOR = 6,
    LPAREN = 7,
    RPAREN = 8,
    NUMBER = 9
};

int isDelim(const char c) {
    return c == ' ';
}
int isDecimal(const char c) {
    return c == '.' || c == ',';
}
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
    if(strcmp("**", str) == 0) return FACTOR;
    if(strcmp("(", str) == 0) return LPAREN;
    if(strcmp(")", str) == 0) return RPAREN;
    if(strcmp("%", str) == 0) return MOD;
    if(isNumber(str)) return NUMBER;

    return -1;
}

int getPriority(const enum TokenType type) {
    if(type == NUMBER) return -1;
    if(type == ADD || type == SUBTRACT) return 1;
    if(type == MULTIPLY || type == DIVIDE || type == FACTOR || type == MOD) return 2;
    if(type == LPAREN || type == RPAREN) return 3;

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
    fprintf(stdout, "\n\n\n");
}

Token initToken(enum TokenType type) {
    Token token;
    token.type = type;
    token.priority = getPriority(type);
    token.val = 0;

    return token;
}

Token tokenFromString(char* str) {
    enum TokenType type = getType(str);
    Token token = initToken(type);

    if(type != NUMBER) return token;
    char** endptr;
    token.val = strtod(str, endptr);
    return token;
}

void insertAt(Token* arr, int* size, Token value, int index) {
    arr = realloc(arr, (*size + 1) * sizeof(Token));
    if (arr == NULL) {
        perror("Failed to reallocate memory");
        exit(1);
    }

    for (int j = *size; j > index; j--) {
        arr[j] = arr[j - 1];
    }

    arr[index] = value;
    (*size)++;
}

int getEntriesNumber(Token* arr, int size) {
    int count = 0;
    for (Token* iter = arr; iter <= arr + size; iter++) {
        if (iter->type != 0) count++;
    }
    return count;
}

int getStrArrLen(char** arr) {
    int count = 0;
    while (arr[count] != NULL) {
        count++;
    }
    return count;
}