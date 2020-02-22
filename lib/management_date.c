#ifndef __MANAGEMENT_DATE
#define __MANAGEMENT_DATE

#include    <stdio.h>
#include    <stdlib.h>
#include    <time.h>
#include    <management_string.c>


char* getTime() { 
    time_t mytime;
    mytime = time(NULL);
    return trim(ctime(&mytime));
}

char* getHours() {
    char *hours = (char*)malloc(6);
    char *dump = getTime();
    subString(dump, 11, 16);
    strcpy(hours, dump);
    return hours;
}

int* calculatesDifference(char *hourStart, char *hourEnd) {
    if (strlen(hourStart)<5 || strlen(hourEnd)<5){
        printf("[%s->calculatesDifference] il parametro passato deve avere lunghezza fissa di 6 caratteri. Adesso avrai una violazione in memoria\n", __FILE__);
        printf("Start: >%s< strlen=%d\n", hourStart, (int)strlen(hourStart));
        printf("End: >%s< strlen=%d\n", hourEnd, (int)strlen(hourEnd));
        return NULL;
    }

    int hStart, mStart;
    int hEnd, mEnd;
    char *dump = (char*)malloc(6);

    strcpy(dump, hourStart);
    subString(dump, 0, 2);
    hStart = atoi(dump);

    strcpy(dump, hourStart);
    subString(dump, 3, 5);
    mStart=atoi(dump);

    strcpy(dump, hourEnd);
    subString(dump, 0, 2);
    hEnd=atoi(dump);

    strcpy(dump, hourEnd);
    subString(dump, 3, 5);
    mEnd=atoi(dump);

    free(dump);

    if (debug()) {
        printf("Ora inizio: %d:%d\n", hStart, mStart);
        printf("Ora fine: %d:%d\n", hEnd, mEnd);
    }

    if (hStart > 24 || mStart > 60 || hEnd > 24 || mEnd > 60) {
        printf("[%s->calculatesDifference] Ore passate errate. ", __FILE__);
        printf("Adesso avrai una violazione in memoria\n");
        return NULL;
    }
    int countH = 0, countM = 0, flag=0;

reCompute:
    if (hStart != hEnd) { 
        while (1) {
            countH++;
            hStart++;
            if (hStart == 24) {
                hStart = 0;
            }

            if (hStart == hEnd){
                break;
            }
        }
    } else {
        if (mStart > mEnd) { 
            //printf("Tick\n");
            hStart += 1;
            flag = 1;
            goto reCompute;
        }
    }

    if (flag) countH++;

    if (mStart != mEnd) {
        while (1) {
            countM++;
            mStart++;
            if (mStart == 60) {
                countH--;
                mStart = 0;
            }

            if (mStart == mEnd) {
                break;
            }
        }
    }
    //printf("Res:   %d:%d\n", countH, countM);
    int *ret = (int*)malloc(sizeof(int) * 2);
    ret[0] = countH;
    ret[1] = countM;
    return ret;
}
#endif
