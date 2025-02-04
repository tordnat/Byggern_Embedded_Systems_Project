#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "uart_and_printf/uart.h"
#include "uart_and_printf/printf-stdarg.h"
#include "can_controller.h"
#include "can_interrupt.h"
#include "sam/sam3x/include/sam.h"
#include "pwm.h"
#include "adc.h"
#include "solenoid.h"
#include "dac.h"
#include "timer.h"
#include "motor.h"
#include "utils.h"
#include "servo.h"
#include "regulator.h"
#include "encoder.h"
#include "game.h"

#define LED1 23
#define LED2 19
#define MCK 84000000

uint8_t goal_scored() {
    uint16_t adc_val = adc_read();
    const uint16_t threshold = 1000;
    if(adc_val < threshold) {
        return 1;
    } else {
        return 0;
    }
}

CAN_MESSAGE can_msg;


int32_t ref_pos = 0; //Input from node 1
int32_t prev_encoder_pos = 0; //Prev encoder value
int32_t current_encoder_pos = 0; //Current encoder value

int main() {
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer
    configure_uart();
    servo_init();
    dac_channel1_init();
    adc_channel0_init();
    solenoid_init();
    motor_init();
    encoder_init();
    timer_init();
    uint32_t can_br = ((42-1) << CAN_BR_BRP_Pos) | ((4-1) << CAN_BR_SJW_Pos) | ((7-1) << CAN_BR_PROPAG_Pos) | ((4-1) << CAN_BR_PHASE1_Pos) | ((4-1) << CAN_BR_PHASE2_Pos);
    if(can_init_def_tx_rx_mb(can_br)) {
        printf("Can failed init\n\r");
    }

    printf("Everything inited\n\rCalibrating...\n\r");
    encoder_calibrate();
    printf("Finished calibration\n\r\n\r");
    int32_t ref_pos = (get_node1_msg().joystickX+100)/2; 
    int32_t servo_pos = get_node1_msg().slider; //0-100
    uint8_t solenoid_pos = get_node1_msg().btn;
    while (1) {
       if(get_reg_tick()) {
            set_reg_tick();
            //Game state machine
            switch (get_state()) {
                case NODE1_PLAYING:
                    ref_pos = (get_node1_msg().joystickX+100)/2;
                    servo_pos = get_node1_msg().slider; //0-100
                    solenoid_pos = get_node1_msg().btn;
                    game_loop(servo_pos, solenoid_pos);
                    break;
                case NODE3_PLAYING:
                    if((get_node3_msg().x_pos <= 0) || (get_node3_msg().y_pos <= 0)) { //invalid states
                        break;
                    }
                    ref_pos = map(get_node3_msg().x_pos, 765, 1600, 99, 1);
                    printf("Node3 %d %d %d\n\r", get_node3_msg().x_pos, get_node3_msg().y_pos, ref_pos);
                    solenoid_pos = (get_node3_msg().y_pos > 305);
                    
                    game_loop(servo_pos, solenoid_pos);
                    break;
                case STOPPED:
                    ref_pos = 50;
                    game_stop();
                    delay_us(10000);
                    break;
            }
            regulator_pos(ref_pos, &prev_encoder_pos);
       }
    }
}


