#include <stdint.h>
#include "uart_and_printf/printf-stdarg.h"
#include <stdlib.h>
#include <pwm.h>
#include "servo.h"
#include "utils.h"

#define PWM_SERVO_DUTY_MIN 82
#define PWM_SERVO_DUTY_MAX 164

static int8_t prev_pos = 0;

void servo_init() {
    pwm_channel5_init();
}

void servo_set_pos(int8_t pos) {
    pos = -pos;
    //Invalid position
    if((pos < -100) || (pos > 100)) {
        printf("Pos outside valid range %d\n\r", pos);
        return; //add error code
    }
    //TODO: add smoothing
    int diff = pos - prev_pos;
    diff = abs(diff);
    if(diff < 3) {
        return;
    }
    prev_pos = pos;
    //int duty = (((pos - (0)) * (PWM_SERVO_DUTY_MAX - PWM_SERVO_DUTY_MIN))/(100 - (0))) + PWM_SERVO_DUTY_MIN;
    int duty = map(pos, -100, 0, PWM_SERVO_DUTY_MIN, PWM_SERVO_DUTY_MAX);
    //printf("Pos: %d \n\rDuty Cycle value: %d\n\r", pos, duty);
    if((duty <= PWM_SERVO_DUTY_MAX) && (duty >= PWM_SERVO_DUTY_MIN)) {
        pwm_set_duty_channel5(duty);
        return; //add error code
    }
    else {
        printf("Calc error\n\r");
    }
}