#pragma once
#include "DataTypes.h"
#include <math.h>

Token* getDeepestLParen(Token* start, Token* end) {
    Token* result = NULL;
    int depth = 0;
    int maxDepth = 0;
    for (Token* iter = start; iter <= end; iter++) {
        if (iter->type == LPAREN) {
            depth++;
            if (maxDepth < depth) {
                maxDepth = depth;
                result = iter;
            }
        } else if (iter->type == RPAREN) {
            depth--;
        }
    }
    return result;
}

Token* getDeepestRParen(Token* start, Token* end) {
    for (Token* iter = start; iter <= end; iter++) {
        if (iter->type == RPAREN) return iter;
    }
    return NULL;
}

Token* getBiggestPriority(Token* start, Token* end) {
    Token* result = NULL;
    int maxPriority = -1;
    for (Token* iter = start; iter <= end; iter++) {
        if (iter->type < ADD || iter->type > FACTOR) continue;
        if (iter->priority > maxPriority) {
            maxPriority = iter->priority;
            result = iter;
        }
    }
    return result;
}

void cleanup(Token** arr, int* len) {
    int writeIndex = 0;

    for (int i = 0; i < *len; i++) {
        if ((*arr)[i].type >= ADD && (*arr)[i].type <= NUMBER) {
            (*arr)[writeIndex++] = (*arr)[i];
        }
    }

    Token* newArr = realloc(*arr, writeIndex * sizeof(Token));
    if (!newArr) {
        fprintf(stderr, "Out of memory\n");
        free(*arr);
        exit(1);
    }
    *arr = newArr;
    *len = writeIndex;
}

double safePower(double base, double exponent) {
    if (base < 0 && fmod(exponent, 1) != 0) {
        fprintf(stderr, "Error: This interpreter doesn't support imaginary numbers.\n");
        exit(1);
    }
    return pow(base, exponent);
}

double nthRoot(double number, double n) {
    if (number < 0 && fmod(n, 2) != 1) {
        fprintf(stderr, "Error: This interpreter doesn't support imaginary numbers.\n");
        exit(1);
    }
    return (number < 0) ? -pow(-number, 1.0 / n) : pow(number, 1.0 / n);
}

void calculatePart(Token* arr, int* fullLen, Token* start, Token* end) {
    Token* biggestPriority = NULL;
    while ((biggestPriority = getBiggestPriority(start, end)) != NULL) {
        Token result = initToken(NUMBER);

        Token* val1 = biggestPriority - 1;
        Token* val2 = biggestPriority + 1;

        if (biggestPriority->type == ADD) result.val = val1->val + val2->val;
        else if (biggestPriority->type == SUBTRACT) result.val = val1->val - val2->val;
        else if (biggestPriority->type == MULTIPLY) result.val = val1->val * val2->val;
        else if (biggestPriority->type == DIVIDE) {
            if (val2->val == 0) {
                fprintf(stderr, "Division by zero\n");
                exit(1);
            }
            result.val = val1->val / val2->val;
        }
        else if (biggestPriority->type == MOD) {
            if (val2->val <= 0) {
                fprintf(stderr, "Modulus by zero or negative number\n");
                exit(1);
            }
            result.val = fmod(val1->val, val2->val);
        }
        else if (biggestPriority->type == FACTOR) result.val = safePower(val1->val, val2->val);
        else if (biggestPriority->type == ROOT) result.val = nthRoot(val2->val, val1->val);

        *biggestPriority = result;
        *val1 = initToken(0);
        *val2 = initToken(0);

        cleanup(&arr, fullLen);
        end -= 2;
    }
}

Token calculate(Token* arr, int* len) {
    Token* deepestLParen = NULL;
    while ((deepestLParen = getDeepestLParen(arr, arr+*len)) != NULL) {
        Token* deepestRParen = getDeepestRParen(deepestLParen, arr+*len);
        *deepestLParen = initToken(0);
        *deepestRParen = initToken(0);
        cleanup(&arr, len);
        calculatePart(arr, len, deepestLParen, deepestRParen-2);
        if (deepestLParen-1 >= arr && (deepestLParen-1)->type == SUBTRACT) {
            *(deepestLParen-1) = initToken(0);
            deepestLParen->val = -deepestLParen->val;
            if (deepestLParen-2 >= arr && ((deepestLParen-2)->type == NUMBER || (deepestLParen-2)->type == RPAREN)) *(deepestLParen-1) = initToken(ADD);
            else cleanup(&arr, len);
        }
    }
    while (*len > 1) {
        calculatePart(arr, len, arr, arr+*len);
    }
    Token result = *arr;
    free(arr);
    return result;
}
