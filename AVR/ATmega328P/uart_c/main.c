
#include "uart.h"
#include <avr/interrupt.h>

int main(void)
{
    uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(115200,F_CPU));
    sei();
    uart_puts("Hello World!\n\r");
    uart_puts("This is UART echo program.\n\r");

    for (;;) {
        unsigned int c = uart_getc();
        if ((c & 0xFF00) == 0) {
            uart_putc((unsigned char)c);
        }
    }
    
    return 0; // Should never be reached
}


