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
    Token* result = NULL;
    int depth = 0;
    int maxDepth = 0;
    for (Token* iter = end; iter >= start; iter--) {
        if (iter->type == RPAREN) {
            depth++;
            if (maxDepth < depth) {
                maxDepth = depth;
                result = iter;
            }
        } else if (iter->type == LPAREN) {
            depth--;
        }
    }
    return result;
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
    int reduceBy = 0;
    for (int i = 0; i <= *len; i++) {
        if ((*arr)[i].type == 0) {
            reduceBy++;
            for (int j = i; j < *len - 1; j++) {
                (*arr)[j] = (*arr)[j + 1];
            }
        }
    }
    printTokenArray(*arr, (*len - reduceBy));
    *arr = realloc(*arr, (*len - reduceBy) * sizeof(Token));
}

void calculatePart(Token* start, Token* end) {
    if (start->type == LPAREN && end->type == RPAREN) {
        *start = initToken(0);
        *end = initToken(0);
    }
    Token* biggestPriority = NULL;
    while ((biggestPriority = getBiggestPriority(start, end)) != NULL) {
        Token result = initToken(NUMBER);

        Token* val1 = NULL;
        Token* val2 = NULL;

        for (Token* iter = biggestPriority; iter >= start; iter--) {
            if (iter->type == NUMBER) {
                val1 = iter;
                break;
            }
        }

        for (Token* iter = biggestPriority; iter <= end; iter++) {
            if (iter->type == NUMBER) {
                val2 = iter;
                break;
            }
        }

        if (val1 == NULL || val2 == NULL) {
            fprintf(stderr, "Idk what happened\n");
            exit(1);
        }

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
    }
}

Token calculate(Token* arr, int* len) {
    Token* deepestLParen = NULL;
    while ((deepestLParen = getDeepestLParen(arr, arr+*len)) != NULL) {
        Token* deepestRParen = getDeepestRParen(arr, arr+*len);
        calculatePart(deepestLParen, deepestRParen);
    }
    while (getEntriesNumber(arr, *len) > 1) {
        calculatePart(arr, arr+*len);
    }
    for (Token* iter = arr; iter < arr + *len; iter++) {
        if (iter->type == NUMBER) {
            Token result = *iter;
            free(arr);
            return result;
        }
    }
    return *arr;
}
