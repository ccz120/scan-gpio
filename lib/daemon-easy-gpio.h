#ifndef DAEMON_EASY_GPIO_H
#define DAEMON_EASY_GPIO_H

#include <sys/msg.h>
#define _REENTRANT
#include <pthread.h>

#define KEY_MESSAGE_QUEUE 1234
#define VERSION "V5.0_STABLE"

#define CLOSE 1

int action;

//-------------------------------------------------------------------------------------------------------------------------------------------------------
//The follow structure and function are needed to use a message queue to let the comunication with dameon-easey-gpio

struct MessageData {
    long int type;
    int pin;
    float value;
};

void send_message/*at daemon-easy-gpio ONLY*/(int pin, float f){
    key_t key = (key_t)KEY_MESSAGE_QUEUE;
    int msgid = msgget(key, 0666 | IPC_CREAT);

    PRINT_IF_DEBUG_ON("[send_message] Pin: %d\tfrequency: %.2f\n", pin, f);

    struct MessageData my_data;
    my_data.type = 1;
    my_data.pin=pin;
    my_data.value=f;
    msgsnd(msgid, (void *)&my_data, sizeof(struct MessageData) - sizeof(long int), 0);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
//The follow function are usable to control, thoughtless the gpio

/*
 * If you wan't switch the value using the daemon-easy-gpio
 * Eg:  toggles_pin(6);
 * 
 */
int toggles_pin_msg(int pin){
    if (pinIsInitialized(pin)){
        if (read_pin(pin)==0){
            send_message(pin, 1.0);
            f_print_value_gpio(pin);
            return 0;
        } else {
            send_message(pin, 0.0);
            f_print_value_gpio(pin);
            return 0;
        }
    } else {
        write_pin(pin, 1); //exception
        printf("Pin %d has been initialized like output and now is HIGH\n", pin);
        return 1;
    }
}

#endif
