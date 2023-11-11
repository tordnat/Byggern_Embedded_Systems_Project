#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "can.h"
#include "mcp2515.h"

int main(void) {
  mcp2515_init();
  static can_message_t msg;
  msg.id = 0x69;
  msg.data_length = 1;
  msg.data[0] = 'k';
    // Set pin 13 (PB5 on ATmega328P) as an output
    DDRB |= (1 << DDB5);

    while (1) {
        // Turn the LED on
        PORTB |= (1 << PORTB5);
        _delay_ms(200); // Delay for 1000 milliseconds

        // Turn the LED off
        PORTB &= ~(1 << PORTB5);
        can_message_transmit(&msg);
        _delay_ms(200); // Delay for 1000 milliseconds
    }

    return 0;
}
