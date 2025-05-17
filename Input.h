#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

long goToNextNlOrDelim(FILE *file) {
    char c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n' || c == ';') {
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
    long nlLocation = goToNextNlOrDelim(file);

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