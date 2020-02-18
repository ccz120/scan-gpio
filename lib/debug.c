#ifndef __DEBUG_FILE_STE
#define __DEBUG_FILE_STE

#include    <stdio.h>
#include    <apiSO.c>
#include    "printColor.c"

#define PRINT_IF_DEBUG_ON if(debug()) printf


int __debug=FALSE;
int debug() { 
    return __debug;
}

void printf_d(char *s) {
#ifdef _DEBUG_
    printf("%s\n", s);
#else
    if (debug())
        printf("%s\n", s);
#endif
}

void debug_on() {
#ifdef _DEBUG_
    setColor(PRINT_CYAN);
    printf("[MODALITA' DEBUG: FORCED ON]");
    resetColor();
    printf("\n");
    __debug=TRUE;
#else
    setColor(PRINT_YELLOW);
    printf("[MODALITA' DEBUG: ON]");
    resetColor();
    printf("\n");
    __debug=TRUE;
#endif
}

void debug_off() {
#ifdef _DEBUG_
    setColor(PRINT_CYAN);
    printf("[MODALITA' DEBUG: IS FORCED ON] recompile without _DEBUG_ macro");
    resetColor();
    printf("\n");
#else
    setColor(PRINT_YELLOW);
    printf("[MODALITA' DEBUG: OFF]");
    resetColor();
    printf("\n");
    __debug=FALSE;
#endif
}

void active_debug_FOR_EACH() {
    if (getuid() == 0) {
        //to do
    }
}

#endif
