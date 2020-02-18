#ifndef COLORED_MESSAGE_C
#define COLORED_MESSAGE_C

#include <stdio.h>
#include "printColor.c"


void errorMessage(const char *mex) {
    printf("[");
    setColor(PRINT_RED);
    printf("Error");
    resetColor();
    printf("] %s", mex);
}

void abortMessage(const char *mex) {
    printf("[");
    setColor(PRINT_RED);
    printf("Abort");
    resetColor();
    printf("] %s", mex);
}

void successMessage(const char *mex) {
    printf("[");
    setColor(PRINT_GREEN);
    printf("Success");
    resetColor();
    printf("] %s", mex);
}

void warningMessage(const char *mex) {
    printf("[");
    setColor(PRINT_YELLOW);
    printf("Warning");
    resetColor();
    printf("] %s", mex);
}
#endif
