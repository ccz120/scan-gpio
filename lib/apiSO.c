#ifndef __APISO
#define __APISO

#include    <stdio.h>
#include    <stdlib.h>
#include    <dirent.h>
#include    <pwd.h>
#include    <unistd.h>
#include    <errno.h>
#include    <stdint.h>

#define     TRUE      1
#define     FALSE     0

#include    "management_date.c"
#include    "coloredMessage.c"
#include    "printColor.c"
#include    "debug.c"


void msleep(int ms) {
    usleep(ms * 1000);
}

/* 
 * If 'force == TRUE', if you run within sudo this function return uid 1000 by default
 * */
char user[32];
int getUserId(char *user, int disableRoot) {
    int uid = getuid();
    initArray_str(user, 32);
    if (uid == 0 && disableRoot == TRUE) {       //trattiamo l'user come semplice utente
        uid = 1000;
        //printf_d("[apiSO.c]->getUserId sei l'utente root ma sei stato disattivatto e sei stato trattato come user 1000\n");
        printf_d("[apiSO.c]->getUserId user root is disabled, now you are user 1000\n");
    }

    if (getpwuid(uid)->pw_name == NULL) {
        printf("getpwuid failed, what is your user id? [userID@namePC:~$]\n>");
        readString(user, 32);
    } else {
        strcpy(user, getpwuid(uid)->pw_name);
    }
    return uid;
}

void getCurrentDirectory(char *dir) {
    initArray_str(dir, strlen((dir)));
    if (getcwd(dir, sizeof(dir)) == NULL) {
        perror("getCurrentDirectory() failed\n");
    }
}

/**
 * Dato un file GIÀ APERTO legge riga per riga fino ad un certo discriminante salvando il contenuto su una variabile
 * 
 * Example:
 * readSingleRow(*f, output, 64, '#');
 * printf("Ho salvato la riga: [%s]\n", output);
 * 
 * Il file puntato da f contiene:
 * voglio leggere questa riga fino al carattere: #questo testo non viene letto ne' salvato
 *
 * La printf stamperà:
 *  Ho salvato la riga [voglio leggere questa riga fino al carattere: ]
 *
 * */
int readSingleRow(FILE *f, char *toSave, int dim, char discerning) {
    if (f == NULL) {
        printf("[ERROR] %s->readSingleRow(). First parameter is NULL\n", __FILE__);
        exit(EXIT_FAILURE);
    }
    
    initArray_str(toSave, dim);
    char *temp = (char*)malloc(dim + 1);
    //printf("Ho una stringa di dim %ld che vale: [%s] dim: %d\n", sizeof(temp), temp, dim);
    initArray_str(temp, dim);

    fgets(temp, dim, f);
    for (int i=0; i<dim - 1; i++) {
        if (temp[i] >= 33 && temp[i] < 127) {   //se è un carattere leggibile
            if (temp[i] != discerning) {
                sprintf(toSave, "%s%c", toSave, temp[i]);
            } else {
                int c=0;
                while (fscanf(f, "%c", &temp[i]) > 0 && c++<10)
                    if (temp[i] == '\n') break;
                if (c > 9) printf("uscita 1\n");
                toSave[i]='\0';
                break;
            }
        } else if (temp[i] == 32) { //se è spazio
            toSave[i]='\0';
            break;
        }
    }
    free(temp);
    return strlen(toSave);
}

/*
 * write text in the path 
 * @return -1 if fail, else the written bytes 
 */
int writeFile(char *path, const char *text, const char *mode) {
    FILE *f=fopen(path, mode);
    if (f == NULL) { printf("[%s->writeFile] %s not found\n", __FILE__, path); return -1; }
    int success=fprintf(f, "%s", text);
    fclose(f);
    if (success==-1) return -1;
    return success;
}


int existDirectory(char *file) {
    DIR* dir = opendir(file);
    if (dir) {
        /* Directory exists. */
        closedir(dir);
        return TRUE;
    } else if (ENOENT == errno) {
        /* Directory does not exist. */
        return FALSE;
    } else {
        /* opendir() failed for some other reason. */
        return FALSE;
    }
}
#define SIZE_NAME_FILE  256
#define NUMBER_FILE     256

struct {
    char name[NUMBER_FILE][SIZE_NAME_FILE];
    int n_file;
} Directory;

void init_read_directory() {
    for (int i=0; i<NUMBER_FILE; i++) {
        initArray_str(Directory.name[i], SIZE_NAME_FILE);
    }
    Directory.n_file = 0;
}

int read_directory(const char *path) {
    DIR *dir;
    struct dirent *ent;
    init_read_directory();
    if ((dir = opendir(path)) != NULL) {
        int i=0;
        while ((ent = readdir(dir)) != NULL) {
            if (i > SIZE_NAME_FILE) {
                char errMess[128];
                initArray_str(errMess, 128);
                sprintf(errMess, "[%s]->read_directory, too many files (greater then %d). You can modify 'SIZE_NAME_FILE' macro\n", __FILE__, SIZE_NAME_FILE);
                errorMessage(errMess);
                if (debug()) {
                    printf("\n\n\n%s\n\n\n\n", errMess);
                }
                break;
            }
            if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
                continue;
            subString(ent->d_name, 0, SIZE_NAME_FILE);
            strcpy(Directory.name[i++], ent->d_name);
        }
        closedir(dir);
        Directory.n_file = i;
        return TRUE;
    } else {
        printf("Directory %s not found\n", path);
        return FALSE;
    }
}

/**
 * Read directory and store only interessed kind of file
 * If you have:
 * > ls
 * myFile.c     yourFile.cpp    hisFile.txt     herFile.pdf     itsFile
 *
 * With this function, if you write:
 * store_file_from_directory("your path printed above", "cpp")
 *
 * Inside Directory you have only: "yourFile.cpp"
 * */
int store_file_from_directory(const char *path, const char *saveFor) {
    DIR *dir;
    struct dirent *ent;
    init_read_directory();
    if ((dir = opendir(path)) != NULL) {
        int i=0;
        while ((ent = readdir(dir)) != NULL) {
            if (i > SIZE_NAME_FILE) {
                if (debug()) printf("[%s]->read_directory, too many files (greater then %d). You can modify 'SIZE_NAME_FILE' macro\n", __FILE__, SIZE_NAME_FILE);
                break;
            }
            if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
                continue;

            char *dumpType = (char*)memchr(ent->d_name, '.', strlen(ent->d_name));            //substring al '.'
            if (dumpType != NULL) {
                if (!strcmp(dumpType, saveFor)) {
                    subString(ent->d_name, 0, SIZE_NAME_FILE);
                    strcpy(Directory.name[i++], ent->d_name);
                }
                
            }
        }
        closedir(dir);
        Directory.n_file = i;
        return TRUE;
    } else {
        printf("Directory %s not found\n", path);
        return FALSE;
    }
}


int removeFile(char *name) {
    FILE *f=fopen(name, "r");
    if (f==NULL) {
        if (debug()) printf("[%s->removeFile]File %s not found\n", __FILE__, name);
        return FALSE;
    }
    fclose(f);
    remove(name);
    return TRUE;
}

int existFile(char *file) {
    FILE *f = fopen(file, "r");
    if (f==NULL) return FALSE;
    fclose(f);
    return TRUE;
}


/*
 * Ritorna il livello della batteria, se la batteria è in carica il valore sarà positivo, se la batteria non è in carica il valore sarà negativo
 * Bug: Error 11 scaturisce quando ????
 */
int getLevelBattery(){
    char output[64];
    FILE *f;
init_func:
    system("acpitool -b > /home/stefano/.battery.txt");

    usleep(10000);
    initArray_str(output, 64);
    f = fopen("/home/stefano/.battery.txt", "r");
    if (f == NULL) {
        writeFile("/home/stefano/log/crash_batteria.txt", "Error 11. File '.battery.txt' not found. Why?\n", "a+");
        setColor(PRINT_RED);
        printf("Error 11. File '.battery.txt' not found. Why?\n"); 
        resetColor();
        sleep(1); 
        goto init_func;
    } //Permessi o acpitool non esistente
    fgets(output, 64, f);
    fclose(f);
    removeFile("/home/stefano/.battery.txt");
    trim(output);
    int offset;
    int level;
    int oldLevel=50;	//a caso
    for (int i=0; i<64; i++){
        if (output[i] == 58){
            char _level[7];
            initArray_str(_level, 7);
            if (debug()) printf("<%s>\n", output);
            if (output[i+2] == 68){ //Batteria NON in carica 
                offset=15;
                for (int j=0; j<5;j++)
                    _level[j]=output[i+j+offset];
                //printf("Level: <%s>\n", _level);
                level=atoi(_level) *-1;
                break;
            } else if (output[i+2] == 67){ //Batteria in Carica
                //printf("In carica\n");
                offset=12;
                for (int j=0; j<5;j++)
                    _level[j]=output[i+j+offset];
                level=atoi(_level);
                break;
            } else if (output[i+2] == 70){ //batteria carica al 100%                                                                                                                
                level=100;
                break;
            } else if (output[i+2] == 85){
                //errore interno del comando di lettura dello stato della batteria
                setColor(PRINT_RED);
                printf("Errore di acpitool, ha ritornato l'output: <%s> che non e' noto. E' stato tornato il vecchio valore letto\n", output);
                resetColor();
                return oldLevel;
            } else {        //Carattere sconosciuto. Potrebbe essere un errore scaturito dal comando acpitool. Un output non atteso?
                char report[128];
                initArray_str(report, 128);
                sprintf(report, 
                        "[errore 33]Output: <%s> E' un output noto?\nTra questi caratteri ho guardato il carattere centrale: <%c%c%c> Data: %s\n",
                        output, output[i+1], output[i+2], output[i+3], getTime()
                       );
                writeFile("/home/stefano/log/crash_batteria.txt", report, "a+");
                printf("Errore 33\n");
                printf_d(report);
                return oldLevel;
            }
        }
    }
    if (level==0) { //se vero c'è un problema con l'offset del comando. L'output è cambiato?
        printf("Errore 44\n");
        char report[128];
        initArray_str(report, 128);
        sprintf(report, "[errore 44]Data: %s. Output: <%s> E' un output noto?\n", getTime(), output);
        writeFile("/home/stefano/log/crash_batteria.txt", report, "a+");
        printf_d(report);
        return oldLevel;
    }
    oldLevel = level;
    return level;
}
#endif
