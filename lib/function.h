#ifndef FUNCTION_H
#define FUNCTION_H

#define     _GNU_SOURCE	//for stdio -> fcloseall()
#include    <stdio.h>
#include    <stdlib.h>
#include    <ctype.h>
#include    <stdbool.h>
#include    <signal.h>

#define     PRECONDITION    int i=0; i<
#define     POSTCONDITION   ;i++

#include    <printColor.c>
#include    <apiSO.c>
#include    <debug.c>
#include    <management_date.c>
#include    <management_string.c>
#include    <coloredMessage.c>

#define COMMAND     1
#define PARAM       0

#define CHAR        0
#define INT         1
#define FLOAT       2
#define DOUBLE      3
#define STRING      4
#define VOID_GP     5

/**
 *Struttura per passare ad una funzione una parametro generico. Esempio la funzione sotto
 */
typedef struct{
    int type;

    union {
        char c;
        int i;
        float f;
        double d;
    };
} variant;

void print_number_v(variant v){
    switch(v.type){
        case CHAR:
            printf("%c\n", v.c);
            break;
        case INT:
            printf("%d\n", v.i);
            break;
        case FLOAT:
            printf("%.2f\n", v.f);
            break;
        case DOUBLE:
            printf("%f\n", v.d);
            break;
    }
}

void initArray_int(int *array, int dim, int init_value) {
    for (int i=0; i<dim; i++)
        array[i] = init_value;
}

void managementParam(int i);
void managementCommand(int i);

//#define MAX_NUMBER_COMMAND      24
//#define MAX_NUMBER_PARAM        24
//#define MAX_DIM_P_C             64

#define MAX_NUMBER_COMMAND      2048
#define MAX_NUMBER_PARAM        2048
#define MAX_DIM_P_C             512

/**
 * Questa funzione analizza i parametri inseritu su argv e li divide tra comandi e effettivi parametri
 * Esempio comando:
 *      compile -d file.c
 *  -d          è un comando
 *  file.c      è un parametro
 *
 *  La funzione sotto salverà nella variabile   'param' tutti i parametri riconosciuti
 *                                              'command' tutti i comandi riconosciuti
 * 
 */
char command[MAX_NUMBER_COMMAND][MAX_DIM_P_C];
int countCommand;
char param[MAX_NUMBER_PARAM][MAX_DIM_P_C];
int countParam;
char **__p;
char name_exe[32];
void analyzeArg(int argc, char **argv) { 
    countCommand = countParam = 0;
    if (argc > MAX_NUMBER_COMMAND + MAX_NUMBER_PARAM) {
        setColor(PRINT_RED);
        printf("[FATAL ERROR] (%s->analyzeArg()) Numero di argomenti troppo grande, bisogna modificare la dichiarazione delle variabili della funzione in questo errore mostrata. ", __FILE__);
        resetColor();
        exit(EXIT_FAILURE);
    }

    for (int i=1; i<argc; i++) {
        if (argv[i][0] == '-') {
            if (strlen(argv[i]) > MAX_DIM_P_C) 
                subString(argv[i], 0, MAX_DIM_P_C);
            
            strcpy(command[countCommand++], argv[i]);
        } else {
            if (strlen(argv[i]) > MAX_DIM_P_C) 
                subString(argv[i], 0, MAX_DIM_P_C);

            strcpy(param[countParam++], argv[i]);
        }
    }
    __p = argv;
    initArray_str(name_exe, 32);
    subString(name_exe, 0, 32);
    strcat(name_exe, argv[0]);
#ifdef _DEBUG_
    __viewAllCommand();
    __viewAllParam();
#endif
}

void __viewAllCommand() {
    printf("########### Comandi ##############\n");
    for (int i=0; i< countCommand; i++)
        printf("#\t%s\n", command[i]);
    printf("##################################\n\n");
}

void __viewAllParam() {
    printf("########## Parametri ############\n");
    for (int i=0; i< countParam; i++)
        printf("#\t%s\n", param[i]);
    printf("##################################\n\n");
}

bool _isInitializeRandomf = false;
//Ritorna un numero compreso tra 0 e 1
float randomf() {
    if (_isInitializeRandomf == false) {
        srand((unsigned int)time(NULL));
        _isInitializeRandomf = true;
    }
    //      Love shitcode
    return ((float)((rand()) % 10)) / (float)((rand() % 10) + 10.0);
}


void printWaiting(char *text, int timeToWait) {
//text like:        "Waiting %d seconds, give me SIGINT if you want stop me.."
        printf("\b");
        for (int i=timeToWait; i>=0; i--) {
            printf(text, i);
            fflush(stdout);
            sleep(1);
            for (int j=0; j<strlen(text); j++) printf("\b");
        }
        printf("\n");
}

#endif
