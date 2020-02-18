#include <function.h>
#include <function-gpio.h>

#define MAX_GPIO                    28
#define MAX_LENGTH_PATH_GPIO        64
#define MAX_LENGTH_DIRECTION_GPIO   4
#define PATH_LOG_GPIO               "/home/%s/log_status_variation_GPIO.txt"
#define DISABLE_SCAN_FILE           "/home/%s/disable_scan_gpio.txt"
#define DISABLE_SCAN_FILE_TEMPORARY "/home/%s/.disable_scan_gpio_temp.txt"

typedef struct {
    int value;               //value pin
    int pwm;                 //if is pwm => disable scan
} GPIO;        


//simplify code
GPIO gpio[MAX_GPIO];
int gpio_newValue[MAX_GPIO];
int gpio_disabled[MAX_GPIO];
int n_gpio=0;

int isEnable(int index) {
    return gpio_disabled[index] == FALSE;
}

int read_directory_ad(char *directory) {  //particolare implementazione della read_directory
    DIR *d;
    struct dirent *dir;
    int pin;
    d = opendir(directory);  
    if (d!= NULL) {
        while ((dir = readdir(d)) != NULL) {
            if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..") || !strcmp(dir->d_name, "export") || strlen(dir->d_name) > 6) 
                continue;
            
            //get number of pin
            subString(dir->d_name, 4, strlen(dir->d_name));
            pin = atoi(dir->d_name);
            
            //validate pin
            if (!isEnable(pin)) {
                gpio[pin].pwm = TRUE;
                continue;
            } else gpio[pin].pwm = FALSE;

            //store value
            gpio[pin].value = read_pin(pin);
            gpio_newValue[pin] = gpio[pin].value;

            //check 
            n_gpio++;
            if (n_gpio >= MAX_GPIO) {
                warningMessage("Much gpio counted");
                printf(" (%d). It's ok?\n", n_gpio);
                break;
            }
        }
        closedir(d);
        return TRUE;
    } else {
        printf("Directory [%s] not found\n", directory);
        exit(EXIT_FAILURE);
    }
}

void updateFileLog(int pin, int value){
    char path[128];
    initArray_str(path, 128);
    getUserId(user, NO_ROOT);
    sprintf(path, PATH_LOG_GPIO, user);
    FILE *f=fopen(path, "a+");
    fprintf(f,"###################################################\n"
            "pin %d new value is %d. %s\n"
            , pin, value, getTime());
    fclose(f);
}

void enable_all_gpio() {
    initArray_int(gpio_disabled, MAX_GPIO, FALSE);  //all gpio are enable
}

void init_new_value_structure() {
    initArray_int(gpio_newValue, MAX_GPIO, -1);
}

unsigned int mutex_warning_print = true;
void disable_gpio(char *path) {
    FILE *f = fopen(path, "r");
    int pin_to_disable;
    enable_all_gpio();
    if (f != NULL) {
        while (fscanf(f, "%d\n", &pin_to_disable) > 0) {
            if (pin_to_disable >= 0 && pin_to_disable < MAX_GPIO) {
                gpio_disabled[pin_to_disable] = TRUE;
            } else {
                if (mutex_warning_print) {
                    mutex_warning_print = false;
                    warningMessage("Strange gpio.");
                    printf(" %d is not a existed gpio. It will be ignored then it's not a problem\n", pin_to_disable);
                    printf("But if you use [-d] parameter to check which gpio you entered is wrong it would be better\n");
                }
            }
        }
        fclose(f);
    }
}

void trigger_gpio(int pin) {
    printf("\n----------------------------------- [%s]\n", getTime());
    printf("GPIO %d --> ", pin);
    gpio[pin].value == 1 ? printf("is on") : printf("is off");
    printf("\n");
    printf("Waiting a new state varaition...\n");

}

void update_structure(char *directory) {
    DIR *d;
    struct dirent *dir;
    d = opendir(directory);  
    if (d!= NULL) {
        while ((dir = readdir(d)) != NULL) {
            if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..") || !strcmp(dir->d_name, "export") || strlen(dir->d_name) > 6) 
                continue;

            //store pin
            subString(dir->d_name, 4, strlen(dir->d_name));
            int pin = atoi(dir->d_name);

            if (!isEnable(pin)) continue;

            int newValue = read_pin(pin);
            //printf("    %d vs %d\n", gpio[pin].value, newValue);
            if (gpio[pin].value != newValue) {
                gpio[pin].value = newValue; 
                trigger_gpio(pin);
            }
        }
        closedir(d);
    }
}

void check_existence_file() {
    char buf[256];
    initArray_str(buf, 256);
    sprintf(buf, DISABLE_SCAN_FILE, user);
    if (!existFile(buf)) 
        writeFile(buf, "", "w");
    initArray_str(buf, 256);
    sprintf(buf, DISABLE_SCAN_FILE_TEMPORARY, user);
    if (!existFile(buf)) 
        writeFile(buf, "", "w");
}

void open_file_disable_gpio() {
    char cmd[256];
    initArray_str(cmd, 256);
    check_existence_file();
    sprintf(cmd, "cp " DISABLE_SCAN_FILE " " DISABLE_SCAN_FILE_TEMPORARY " && editor " DISABLE_SCAN_FILE_TEMPORARY,  user, user, user);
    printf("Press return to continue\n");
    getchar();
    system(cmd);
}

int validate_disable_file() {
    char path[128];
    char cmd[256];
    char paux;
    int error = FALSE;
    initArray_str(path, 128);
    initArray_str(cmd, 256);
    sprintf(path, DISABLE_SCAN_FILE_TEMPORARY, user);
    FILE *f = fopen(path, "r");
    if (f != NULL) {
        while (fscanf(f, "%c", &paux) > 0) {
            if (paux == '\n') 
                continue;
            if (paux < 48 || paux > 57) {
                error = TRUE;
                printf("Ah! hai inserito (%c)\n", paux);
                break;
            }
        }
        fclose(f);
        if (error == TRUE) {
            errorMessage("Format file is wrong! Your new file will be destroyed\n"); 
            printf("Correct format is like bottom\n");
            printf("12\n5\n9\nif you don't want to check them (12, 5, 9)\n");
            removeFile(path);
        } else {
            successMessage("Format file correct! Well done\n"); 
            sprintf(cmd, "cp " DISABLE_SCAN_FILE_TEMPORARY " " DISABLE_SCAN_FILE, user, user);
            system(cmd);
        }
        return error == FALSE;
    }
    return FALSE;
}

void print_syntax(char **argv) {
    printf("This is %s software. This software allow you to see all state variation of the gpio. Don't care the direction of the pin, it show you all variation.\n ", argv[0]);
    printf("It's possibile don't see one, or more, gpio (if there is are pwm or if you don'r care them).\n");
    printf("All varation well be store in"PATH_LOG_GPIO"\n", user);
    printf("Syntax:     $ %s            show all state change\n", argv[0]);
    printf("            $ %s -h         print this text\n", argv[0]);
    printf("            $ %s -d         guide you to choose one (or more) pin to leave out during the scan\n", argv[0]);
}

int main(int argc, char **argv) {
    getUserId(user, NO_ROOT);
    if (argc > 1) {
        if (!strcasecmp(argv[1], "-h") || !strcasecmp(argv[1], "--help")) {
            print_syntax(argv);
        } else if (!strcasecmp(argv[1], "-d") || !strcasecmp(argv[1], "--disable")) {
            char dump[4];
            initArray_str(dump, 4);
            printf("Would you leave out some gpio? (s/n)\n> ");
            readString(dump, 4);
            if (!strcmp(dump, "s")) {
                printf("In the file that will be opened, insert, one for each line, the gpio you want to exclude\n");
                warningMessage("one for each line, no text!\n");
                open_file_disable_gpio();
                exit(!validate_disable_file());
            } else {
                printf("Nothing to do\n");
            }
        } else {
            printf("Unknow command given [%s]. Use: %s --help\n", argv[1], argv[0]);
        }
    } else {
        char path_disabled_file[128];
        initArray_str(path_disabled_file, 128);
        sprintf(path_disabled_file, DISABLE_SCAN_FILE, user);
        printf("All variation will be store in: "PATH_LOG_GPIO"\n", user);

        read_directory_ad("/sys/class/gpio/");
        printf("Waiting a new state varaition...\n");
        init_new_value_structure();

        while(TRUE) {
            usleep(1000);
            disable_gpio(path_disabled_file);
            update_structure("/sys/class/gpio/");
        }
    }
    exit(EXIT_SUCCESS);
}
