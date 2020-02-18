#include <stdio.h>

#ifndef PRINTF_COLOR
#define PRINTF_COLOR

#define PRINT_RESET       0
#define PRINT_BRIGHT      1
#define PRINT_DIM         2
#define PRINT_UNDERLINE   3
#define PRINT_BLINK       4
#define PRINT_REVERSE     7
#define PRINT_HIDDEN      8

#define PRINT_BLACK       0
#define PRINT_RED         1
#define PRINT_GREEN       2
#define PRINT_YELLOW      3
#define PRINT_BLUE        4
#define PRINT_MAGENTA     5
#define PRINT_CYAN        6
#define PRINT_WHITE       7

#define P_BLACK          PRINT_BLACK
#define P_RED            PRINT_RED
#define P_GREEN          PRINT_GREEN
#define P_YELLOW         PRINT_YELLOW
#define P_BLUE           PRINT_BLUE
#define P_MAGENTA        PRINT_MAGENTA
#define P_CYAN           PRINT_CYAN
#define P_WHITE          PRINT_WHITE


#define BEGIN_PRINT_RED     setColor(PRINT_RED); printf(
#define BEGIN_PRINT_GREEN   setColor(PRINT_GREEN); printf(
#define BEGIN_PRINT_BLUE    setColor(PRINT_BLUE); printf(
#define END_PRINT           ); resetColor();


void setColor(int fg) {
    char    command[13];
    sprintf(command, "%c[%d;%d;%dm", 0x1B, PRINT_BRIGHT, fg + 30, PRINT_BLACK + 40);      //BLACK è lo sfondo, BRIGHT è per modificare il colore
    printf("%s", command);
}

void resetColor() {
    char    command[13];
    sprintf(command, "%c[%d;%d;%dm", 0x1B, PRINT_RESET, PRINT_WHITE + 30, PRINT_BLACK + 40);
    printf("%s", command);
}


/* Esempio di utilizzo:
int main(void){   
    setColor(PRINT_RED);  
    printf("Questo testo sara' colorato di rosso\n");
    resetColor(); 
    return 0;
}
*/


#endif
