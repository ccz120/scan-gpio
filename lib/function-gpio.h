#include "function.h"

#ifndef FUNCTION_GPIO
#define FUNCTION_GPIO

#ifdef DEBUG_GPIO
#define GPIO_MAX 28


struct {
    int direction[GPIO_MAX];  //in or out
    int gpio[GPIO_MAX];           //gpio number, eg: 25
    int isInit;
} Gpio;


int pinIsInitialized(int pin) {
    printf("[pinIsInitialized] Called on %d\n", pin);
    return Gpio.isInit == 0; 
}

int initializePin(int pin, int direction) {
    warningMessage("[initializePin] Called on %d\n", pin);
    if (Gpio.isInit == 1) {
        errorMessage("[initializePin] YOU CALL ME GREATER THEN ONE TIME\n");
        return 1;
    }
    Gpio.isInit = 1;
    return 0;
}

void write_pin(int pin, int value) {
    if (Gpio.direction[pin % GPIO_MAX] == 0) {
        errorMessage("[write_pin] ");
    }
    printf("[write_pin] Called on %d. New value is: %d\n", pin, value);
}

int read_direction(int pin) {
}

int is_input(int pin) {
}

int read_pin(int pin) {
}





#else









#define NO_ROOT     TRUE
#define PATH_LOG    "/home/%s/log/log_gpio/"
#define RISING      "HIGH"
#define FALLING     "LOW"


/*	
 *	control path: /sys/class/gpio/gpioN/value. if not exist return 0, else 1
*/
int pinIsInitialized(int pin) {
	char path[48];
    initArray_str(path, 48);
	sprintf(path, "/sys/class/gpio/gpio%d/value", pin);
	FILE *f=fopen(path, "r");
	if(f == NULL) return 0;
	fclose(f);
	return 1;
}

/*	write: 	pin into /sys/class/gpio/export 
 *		direction into /sys/class/gpio/gpiopin/direction
 *	
 *	At the end the pin will be initialized and it will be possible to use it correctly
 *
 *	Eg: initializePin(4, "out");
 * */
int initializePin(int pin, int direction) {	//direction = 1 ==> "out"; direction = 0 ==> "in"
	char path[48];
    initArray_str(path, 48);
	if (!pinIsInitialized(pin)){
		FILE *f=fopen("/sys/class/gpio/export", "w");
        if (f == NULL) {
            printf("ERROR -> file system '/sys/class/gpio/export not found. Are you in a raspberry?\n");
            printf("###################\n\n"
                    "UNABLE TO USE ANY GPIO\n\n"
                    "###################\n"
                    );
            printf("Abort\n");
            exit(EXIT_FAILURE);
        }
		fprintf(f, "%d", pin);
		fclose(f);
		sleep(1);
		sprintf(path, "/sys/class/gpio/gpio%d/direction", pin);
		f=fopen(path, "w");
		direction == 0 ? fprintf(f, "in") : fprintf(f, "out");
		fclose(f);
		return 0;
	}
	return 1;
}

/*	turn on/off pin		and, if is necessary, initialize automatically the pin
 *
 * Eg: 	write_pin(12, 1);	turn on gpio 12
 * 	write_pin(12,0);	turn off gpio 12
 * */
void write_pin(int pin, int value) {
	initializePin(pin, 1);
	char path[64];
    initArray_str(path, 64);
	sprintf(path, "/sys/class/gpio/gpio%d/value", pin);
	FILE *f=fopen(path, "w");
	fprintf(f, "%d", value);
	fclose(f);
}

/*
 *  store the action and turn on/off pin
 *
*/
void write_pin_s(int pin, int value, char *file) {
    char log[128];
    char pathLog[64];
    initArray_str(pathLog, 32);
    initArray_str(log, 128);
    initArray_str(pathLog, 64);

    getUserId(user, TRUE);
    sprintf(pathLog, PATH_LOG"%d", user, pin);
    
    if (value == 0 || value == 1) {
        sprintf(log, "[%s] new state is %s \tupdate by: '%s'\n", getTime(), value == 1 ? RISING : FALLING, file);
        if (strcmp(user, "stefano"))
            write_pin(pin, value);
        else 
            printf("[non siamo sul raspberry, avremmo modificato il pin] %d newvalue -> %d\n", pin, value);
    } else {
        sprintf(log, "[%s]->write_pin_s error value. Its need to be '1' or '0' only. You gived me: %d\n\n", getTime(), value);
    }
    if (debug()) printf("%s", log);
    
    if (strcmp(user, "stefano"))
        writeFile(pathLog, log, "a+");
    else  
        printf("[non siamo sul raspberry, avremmo scritto il log] %s\n", log);
}


/*
 * @return	0 if gpio is "in"
 * 		1 if gpio is "out"
 * 		-1 if gpio is not initialize
 *
 * 	Eg:	if (read_direction(12) == 0)
 * 			printf("Pin 12 is set like INPUT, the value is %d\n", read_pin(12));
 *
 * 	OUTPUT:	Pin 12 is set like INPUT, the value is 0
 * */
int read_direction(int pin) {
	if (pinIsInitialized(pin)) {
        char path[48];
        initArray_str(path, 48);
		char direction[8];
		sprintf(path, "/sys/class/gpio/gpio%d/direction", pin);
		FILE *f=fopen(path, "r"); //se e' inizializzato questo file deve esistere
		fscanf(f, "%s", direction);
		fclose(f);
		if (!strcmp(direction, "in")) return 0;
		if (!strcmp(direction, "out")) return 1;
	}
	//read direction without initilization is impossible
	//printf("[%s->read_direction] Pin (%d) Error 498\n", __FILE__, pin);
	printf("Read direction without initilization is not possible (pin: %d)\n", pin);
	return -1;
}

/**
 * The direction of pin is "in"?
 * */
int is_input(int pin) {
	char path_gpio[64];
	char state[3];
    initArray_str(path_gpio, 64);
    initArray_str(state, 3);
	sprintf(path_gpio, "/sys/class/gpio/gpio%d/direction", pin);
	FILE *f =fopen(path_gpio, "r");
	if (f == NULL) { printf("Error, pin [%d] not found\n", pin); return -1; }
	fscanf(f, "%s", state);
	fclose(f);
	if (!strcasecmp(state, "in")) return TRUE;
	return FALSE;
}

/*	read_pin(pin)
 *	read the value of the gpio (0/1) -1 if pin is not initialized
 *
 * @return value of the gpio
 * */
int read_pin(int pin) {
	int value;
	char path[32];
    initArray_str(path, 32);
	sprintf(path, "/sys/class/gpio/gpio%d/value", pin);

//	print_direction(pin);

	FILE *f=fopen(path, "r");
	if (f==NULL) return -1;
	fscanf(f, "%d", &value);
	fclose(f);
	return value;
}

/*
 *	to example see down: f_pwm_gpio()
 *
 *  This function create a pwm, be careful. Please, use f_pwm_gpio()
 * */
void _create_pwm(int f, int pinPwm) {
	for (int i=0; i<100; i++) {
		if (i<f){
			write_pin(pinPwm, 1);
		} else {
			write_pin(pinPwm, 0);
		}
		usleep(5);
	}
}

#endif

void print_direction(int pin) {
	if (pinIsInitialized(pin)) {
		printf("The direction is: "); is_input(pin) == TRUE ? printf("IN\n") : printf("OUT\n");
	} else {
		printf("Pin %d is not inizilized\n", pin);
	}
}


void change_priority() {
	nice(-19);  //-20 MAX priority, 19 MIN, 0 DEFAULT	
}

void set_priority(int priority) {
	if (priority>19 || priority<-20) {
		PRINT_IF_DEBUG_ON("[function-gpio.h->set_priority] Value of priority is only: [-20, +19]\n");
	} else {
		nice(priority);  //-20 MAX, 19 MIN, 0 DEFAULT	
	} 
}

void remove_file_pid(int pin) {
	if (user==NULL)
		getUserId(user, NO_ROOT);
	char cmd[32];
    initArray_str(cmd, 32);
	sprintf(cmd, "/home/%s/pid_pwm%d.txt", user, pin);
	remove(cmd);
}

void sig_handler(int sig) {
	printf("lose\n");
	//fcloseall();
	exit(EXIT_SUCCESS);
}

void write_pid(int pid, int pin, float frequency) {
	if (user==NULL)
		getUserId(user, NO_ROOT);
	char path[48];
    initArray_str(path, 48);
	sprintf(path, "/home/%s/pid_pwm%d.txt", user, pin);	
	FILE *f=fopen(path, "w");
	fprintf(f, "%d\n", pid);
	fprintf(f, "%.4f\n", frequency);
	fclose(f);
}


//Only for function_pwm
void clear_other_istance(int pin) {
	if (user==NULL)
		getUserId(user, NO_ROOT);
	char path[48];
    initArray_str(path, 48);
	sprintf(path, "/home/%s/pid_pwm%d.txt", user, pin);
	FILE *f=fopen(path, "r");
	if (f!=NULL){
		int pid;
		fscanf(f, "%d", &pid);
		fclose(f);
		if (pid==0){
			printf("[%s->clear_other_istance] ERROR: file [%s] have a bad value\n", __FILE__, path);
			return;
		}
		kill(pid, 9);
	}
}

//Only for function_pwm
void verify_input(char **argv) {
	float f=(float)atof(argv[1]);
	if (f<0 || f>1) {
		printf("Frequency error [%.2f]. Value: 0<f<1\n", f); 
		exit(EXIT_FAILURE);
	}

	if (atoi(argv[2])==0){
		printf("Pin value is a number >0. [%s] in not a number >0\n", argv[2]);
		exit(EXIT_FAILURE);
	}
}


//The follow function are usable to control, thoughtless the gpio



/*	f(unction)_print_value_gpio(pin)
 *	print value and direction
 * */
void f_print_value_gpio(int pin) {
	if (pin<=0 || pin >= 27){
		printf("Bad argument. [%s->f_print_value_gpio] read/-r pin 	where pin is a number [1,26]\n", __FILE__);
		exit(EXIT_FAILURE);
	}
	if (pinIsInitialized(pin)) { 
        printf("Value: %d\n", read_pin(pin));
        printf("Direction: "); read_direction(pin) == 0 ? printf("in\n") : printf("out\n");
    } else {
        printf("Pin %d is not initialize then have not any direction and any value\n", pin);
    }
}

/*	f(unction)_write_gpio(pin)
 * */
void f_write_gpio(int pin, int value) {
	if (pin<=0 || pin >= 27){
		printf("Bad argument. [%s->f_write_gpio] write/-w pin 	where pin is a number [1,26]\n", __FILE__);
		exit(EXIT_FAILURE);
	}
	write_pin(pin, value);
}

/**
 * f(unction)_init(ilized)_gpio(pin, dir(ection))
 * @return 1 if the direction is out, 0 if the direction is in, -1 if an error is detected
 * eg: f_init_gpio(12, "out")		to initialize gpio 12 like output
 * */
int f_init_gpio(int pin, char *dir) {
	if (pin<=0 || pin >= 27){
		printf("Bad argument. [%s] init/-i pin direction	where pin is a number [1,26] and direction is 0 (in) or 1 (out)\n", __FILE__);
		return -1;
	}
	int direction;
	if (!strcmp(dir, "in")){ 
		direction=0;
	} else if (!strcmp(dir, "out")){
        direction=1;
	} else {
		printf("Bad argument. [%s] init/-i pin direction	where pin is a number [1,26] and direction is 0 (in) or 1 (out)\n", __FILE__);
		return -1;
	}
	initializePin(pin, direction);
	return direction;
}

/*	f(unction)_pwm_gpio(frequency, pin)
 * @param f	is a float (kHz)    value: [0:0.001:1]
 * @param pin	is a gpio
 *
 * Eg: 	f_pwm_gpio(0.001, 12);	//ok
 * 	f_pwm_gpio(0.84, 12);	//ok
 *	f_pwm_gpio(2, 12);	//NO! It's an error
 * */
int f_pwm_gpio(float f, int pin) {
	if (pin<=0 || pin >= 27 || f>1 || f<0){
		printf("Bad argument. [%s->f_pwm_gpio] f_pwm_gpio(f, pin)		where f is a float [0,1] and pin is a int [1,26]\n", __FILE__);
		exit(EXIT_FAILURE);
	}
	//signal(SIGINT, sig_handler);
	//getUserId(user, NO_ROOT);
	if (f==0.0){ clear_other_istance(pin); remove_file_pid(pin); write_pin(pin, 0); exit(EXIT_SUCCESS); }
	if (f==1.0){ clear_other_istance(pin); remove_file_pid(pin); write_pin(pin, 1); exit(EXIT_SUCCESS); }
	initializePin(pin, 1);
	//int pid = getpid();
	//clear_other_istance(pin);
	//change_priority();
	//write_pid(pid, pin, f); //and frequency
	//_pinPwm=pin;	//per routine chiusura
	//_flag=1;
    int normalizedDutyCycle = f * 100;
	printf("Frequency: %d pin: %d\n", normalizedDutyCycle, pin); //or %.4f
	while(1){
		_create_pwm(normalizedDutyCycle, pin);
	}
}


/**
 * switch the value.
 * Eg:	tooggles_pin(6);
 * 	if pin 6 is on, it will be turned off
 *
 * @return the new value
 * */
int toggles_pin(int pin, char *instanceCaller) {
	if (pinIsInitialized(pin)) {
		if (read_pin(pin)==0){
			write_pin_s(pin, 1, instanceCaller);
			printf("Pin %d now is HIGH\n", pin);
			return 1;
		} else {
            write_pin_s(pin, 0, instanceCaller);
			printf("Pin %d now is LOW\n", pin);
			return 0;
		}
	} else {
		write_pin_s(pin, 1, instanceCaller);
		printf("Pin %d has been initialized like output and now is HIGH\n", pin);
		return 1;
	}
}

int open_door() {
	return read_pin(6) == 1;
}

int isOpenDoor() {
	return read_pin(6) == 1;
}

#endif
