#include <avr/io.h>
#include <util/delay.h>

// Define the clock frequency (important for delay functions)
#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz clock speed
#endif

int main(void)
{
    // Set PB5 (Pin 13 on Arduino Uno) as an output
    // DDRB is the Data Direction Register for Port B
    // (1 << PB5) sets the 5th bit of DDRB to 1 (Output)
    DDRB |= (1 << PB5);

    while (1)
    {
        // Toggle the LED ON
        // PORTB is the Port Register, controls the output state
        // The XOR assignment (^) flips the current state of PB5
        PORTB ^= (1 << PB5);

        // Wait for 500 milliseconds
        _delay_ms(500);
    }

    return 0; // Should never be reached
}

