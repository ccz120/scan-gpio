#ifndef __MANAGEMENT_STRING
#define __MANAGEMENT_STRING

#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <pwd.h>
#include    "apiSO.c"
#include    "assert.h"
#include    "debug.c"

#ifndef TRUE
#pragma message "problema di incompatibilita', risolto con una pezza"
#define     TRUE    1
#define     FALSE   0
#endif


#ifndef PRECONDITION
#define PRECONDITION int i=0; i<
#define POSTCONDITION ;i++
#endif


#define FFLUSH while(getchar()!='\n') ;

int isNumber(char *text) {
    for (int i=0; i<strlen(text); i++) {
        if (!(text[i] >= 48 && text[i]<=57)) return FALSE;
    }
    return TRUE;
}

char *ltrim(char *s){
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s){
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s){
    return rtrim(ltrim(s)); 
}

void readString(char s[], int dim){
    int i = 0;
    for (i = 0; i<dim - 1; i++)
        if ((s[i] = getchar()) == '\n') break;
    if (i == dim - 1) FFLUSH;
    s[i] = '\0';
}

void initArray_str(char *array, int dim) {
    for (int i=0; i<dim; i++) {
        array[i] = '\0';
    }
}

int subString(char *str, int start, int end) {
    int len = strlen(str);
    if (end<0) end=len;
    if (start<0) start=0;
    if (end>len) {
        if (debug()){
            printf("[%s->subString] Nothing to do. String %s have dim:%d, so the index %d (end index) doesn't exist\n", __FILE__, str, len, end, end);
        }
        return -1;
    }
    if (start > len || start > end) {
        if (debug()) printf("[%s->subString] Error substring (size). str:%s--start:%d--end:%d--lenstr:%d\n", __FILE__, str, start, end, len);
        return -1;
    }
    char *str_temp = (char*)malloc(len);
    initArray_str(str_temp, len);
    int new_index=0;
    for (int i=0; i<len; i++ ){
        if(i>=start && i<end){
            str_temp[new_index++]=str[i];
        }
    }
    str_temp[new_index]='\0';
    strcpy(str, str_temp);
    free(str_temp);
    return new_index;
}



void lowerCase(char *str, int dim) {
    for (PRECONDITION dim POSTCONDITION) {
        if (str[i] >= 65 && str[i] <= 90)
            str[i] += 32;
    }
}

void upperCase(char *str, int dim) {
    for (PRECONDITION dim POSTCONDITION) {
        if (str[i] >= 97 && str[i] <= 122)
            str[i] -= 32;
    }
}

void upperCase_onlyfirstChar(char *str, int dim) {
    lowerCase(str, dim);
    if (str[0] >= 97 && str[0] <= 122)
        str[0] -= 32;
}

char** str_split(char* a_str, const char a_delim) {
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    while (*tmp) {
        if (a_delim == *tmp) {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    // Add space for trailing token
    count += last_comma < (a_str + strlen(a_str) - 1);

    // Add space for terminating null string so caller knows where the list of returned strings ends.
    count++;

    result = (char**)malloc(sizeof(char*) * count);

    if (result) {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    return result;
}


#endif
