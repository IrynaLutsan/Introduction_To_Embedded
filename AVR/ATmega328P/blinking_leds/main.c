#include <avr/io.h>
#include <util/delay.h>

#define BUTTON_GPIO 2

static void setup(void);
static void wait_for_button_press(void);
static void blink_leds(void);


int main(void)
{
    setup();
    for (;;){
        wait_for_button_press();
        blink_leds();
    }
    return 0;
}

static void setup(void){       
    DDRD = 0xFF;        //configure all gpios in port D as outputs for leds
    DDRB &= ~(1 << BUTTON_GPIO);     //copfigure gpio 2 in port B as input for button
    PORTB |= (1 << BUTTON_GPIO);     //enable pul-up resistor on gpio 2 in port B for button
}

static void wait_for_button_press(void){
    while(PINB & (1 << BUTTON_GPIO)){
        ;
    }
}

static void blink_leds(void){
    unsigned char i = 0;
    for (i = 0; i < 8; i++){
        PORTD |= (1 << i);
        _delay_ms(500);
        PORTD &= ~(1 << i);
    }
}