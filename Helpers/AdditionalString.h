#pragma once
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int isDelim(const char c) {
    return c == ' ';
}
int isDecimal(const char c) {
    return c == '.' || c == ',';
}

int getSpaceCount(char* str, int len) {
    int counter = 0;
    for (int i = 0; i < len; i++) {
        if (isDelim(str[i])) {
            counter++;
        }
    }
    return counter;
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


int getStrArrLen(char** arr) {
    int count = 0;
    while (arr[count] != NULL) {
        count++;
    }
    return count;
}