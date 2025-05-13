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
        if ((*arr)[i].type != 0) {
            (*arr)[writeIndex++] = (*arr)[i];
        }
    }

    Token* newArr = realloc(*arr, writeIndex * sizeof(Token));
    if (!newArr) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    *len = writeIndex;
    *arr = newArr;
}

void calculatePart(Token* arr, int* fulllen, Token* start, Token* end) {
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
        else if (biggestPriority->type == FACTOR) result.val = pow(val1->val, val2->val);
        else if (biggestPriority->type == MOD) result.val = fmod(val1->val, val2->val);

        *biggestPriority = result;
        *val1 = initToken(0);
        *val2 = initToken(0);

        cleanup(&arr, fulllen);
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
    }
    while (*len > 1) {
        calculatePart(arr, len, arr, arr+*len);
    }
    return *arr;
}
