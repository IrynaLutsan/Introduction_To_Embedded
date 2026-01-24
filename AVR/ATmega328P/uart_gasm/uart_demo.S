#include <avr/io.h>

/* Register aliases */
#define temp    r16
#define temp2   r17
#define data    r18
#define zero    r1

/* Ring Buffer Constants */
#define BUFFER_SIZE 64    /* Must be power of 2 (16, 32, 64, 128...) */
#define BUFFER_MASK (BUFFER_SIZE - 1)

/* UART Constants for 115200 baud @ 16MHz (Double Speed Mode) */
/* Formula: (F_CPU / (8 * Baud)) - 1 = (16000000 / 921600) - 1 = 16.36 -> 16 */
#define UBRR_VAL 16

.section .bss
/* TX Ring Buffer */
tx_buffer:      .space BUFFER_SIZE
tx_head:        .space 1
tx_tail:        .space 1

/* RX Ring Buffer */
rx_buffer:      .space BUFFER_SIZE
rx_head:        .space 1
rx_tail:        .space 1

.section .vectors
.global _start_vectors

_start_vectors:
    rjmp _start         ; 0x0000 RESET
    nop
    reti                ; 0x0002 INT0
    nop
    reti                ; 0x0004 INT1
    nop
    reti                ; 0x0006 PCINT0
    nop
    reti                ; 0x0008 PCINT1
    nop
    reti                ; 0x000A PCINT2
    nop
    reti                ; 0x000C WDT
    nop
    reti                ; 0x000E TIMER2_COMPA
    nop
    reti                ; 0x0010 TIMER2_COMPB
    nop
    reti                ; 0x0012 TIMER2_OVF
    nop
    reti                ; 0x0014 TIMER1_CAPT
    nop
    reti                ; 0x0016 TIMER1_COMPA
    nop
    reti                ; 0x0018 TIMER1_COMPB
    nop
    reti                ; 0x001A TIMER1_OVF
    nop
    reti                ; 0x001C TIMER0_COMPA
    nop
    reti                ; 0x001E TIMER0_COMPB
    nop
    reti                ; 0x0020 TIMER0_OVF
    nop
    reti                ; 0x0022 SPI_STC
    nop
    rjmp usart_rx_isr   ; 0x0024 USART_RX
    nop
    rjmp usart_udre_isr ; 0x0026 USART_UDRE
    nop
    reti                ; 0x0028 USART_TX
    nop
    reti                ; 0x002A ADC
    nop
    reti                ; 0x002C EE_READY
    nop
    reti                ; 0x002E ANALOG_COMP
    nop
    reti                ; 0x0030 TWI
    nop
    reti                ; 0x0032 SPM_READY
    nop

.section .text
.global _start
_start:
    /* Initialize stack pointer */
    ldi temp, lo8(RAMEND)
    out _SFR_IO_ADDR(SPL), temp
    ldi temp, hi8(RAMEND)
    out _SFR_IO_ADDR(SPH), temp

    /* Initialize Zero Register (r1) */
    /* The code uses 'adc r31, r1', assuming r1 is 0. */
    clr zero

    /* Initialize UART */
    rcall uart_init
    
    /* Enable global interrupts */
    sei

    /* Send test message */
    ldi r30, lo8(hello_msg)
    ldi r31, hi8(hello_msg)
    rcall uart_send_string

main_loop:

    /* Check if data received */
    rcall uart_rx_available
    tst temp
    breq main_loop
    
    /* Echo received character */
    rcall uart_receive
    rcall uart_transmit
    
    rjmp main_loop

/* Initialize UART */
uart_init:
    /* Set baud rate */
    ldi temp, lo8(UBRR_VAL)
    sts UBRR0L, temp
    ldi temp, hi8(UBRR_VAL)
    sts UBRR0H, temp

    /* Enable Double Speed Mode (U2X0) for accurate 115200 baud */
    /* UCSR0A is register 0xC0. Bit 1 is U2X0. */
    ldi temp, (1<<U2X0)
    sts UCSR0A, temp

    /* Enable RX, TX and RX interrupt */
    ldi temp, (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)
    sts UCSR0B, temp
    
    /* Set frame format: 8 data bits, 1 stop bit */
    ldi temp, (1<<UCSZ01)|(1<<UCSZ00)
    sts UCSR0C, temp

    clr temp
    sts tx_head, temp
    sts tx_tail, temp
    sts rx_head, temp
    sts rx_tail, temp

    ret

/* Transmit character in data register */
uart_transmit:
    push temp
    push temp2
    push r30            ; <--- SAVE Z-Low
    push r31            ; <--- SAVE Z-High

    /* Calculate next head position */
    lds temp, tx_head
    inc temp
    andi temp, BUFFER_MASK
    
    /* We must wait with interrupts ENABLED. 
       If we CLI before waiting, the ISR can never run to clear space. */
tx_wait:
    lds temp2, tx_tail
    cp temp, temp2
    breq tx_wait
    
    /* NOW disable interrupts for the atomic update */
    cli
    
    /* Store data in buffer */
    lds temp2, tx_head
    ldi r30, lo8(tx_buffer)
    ldi r31, hi8(tx_buffer)
    add r30, temp2
    adc r31, zero
    st Z, data
    
    /* Update head */
    sts tx_head, temp
    
    /* Enable UDRE interrupt */
    lds temp, UCSR0B
    ori temp, (1<<UDRIE0)
    sts UCSR0B, temp
    
    sei
    pop r31             ; <--- RESTORE Z-High
    pop r30             ; <--- RESTORE Z-Low    
    pop temp2
    pop temp
    ret

/* Receive character into data register */
uart_receive:
    push temp
    push temp2
    push r30            ; <--- SAVE Z
    push r31            ; <--- SAVE Z

    /* Wait for data */
rx_wait:
    rcall uart_rx_available
    tst temp
    breq rx_wait
    
    /* Disable interrupts temporarily */
    cli
    
    /* Get data from buffer */
    lds temp, rx_tail
    ldi r30, lo8(rx_buffer)
    ldi r31, hi8(rx_buffer)
    add r30, temp
    adc r31, zero
    ld data, Z
    
    /* Update tail */
    inc temp
    andi temp, BUFFER_MASK
    sts rx_tail, temp
    
    sei
    pop r31             ; <--- RESTORE Z
    pop r30             ; <--- RESTORE Z    
    pop temp2
    pop temp
    ret

/* Check if RX data available - returns result in temp */
uart_rx_available:
    lds temp, rx_head
    lds temp2, rx_tail
    cp temp, temp2
    breq rx_empty
    ldi temp, 1
    ret
rx_empty:
    ldi temp, 0
    ret

/* Send string pointed to by Z register (r31:r30) */
uart_send_string:
    push data
    push r30
    push r31
    
send_loop:
    lpm data, Z+
    tst data
    breq send_done
    rcall uart_transmit
    rjmp send_loop
    
send_done:
    pop r31
    pop r30
    pop data
    ret

/* USART RX Complete ISR */
usart_rx_isr:
    push temp
    push temp2
    push r30
    push r31
    in temp, _SFR_IO_ADDR(SREG)
    push temp
    
    /* Read received data */
    lds data, UDR0
    
    /* Calculate next head position */
    lds temp, rx_head
    inc temp
    andi temp, BUFFER_MASK
    
    /* Check if buffer full */
    lds temp2, rx_tail
    cp temp, temp2
    breq rx_isr_done  ; Buffer full, drop data
    
    /* Store data in buffer */
    lds temp2, rx_head
    ldi r30, lo8(rx_buffer)
    ldi r31, hi8(rx_buffer)
    add r30, temp2
    adc r31, zero
    st Z, data
    
    /* Update head */
    sts rx_head, temp
    
rx_isr_done:
    pop temp
    out _SFR_IO_ADDR(SREG), temp
    pop r31
    pop r30
    pop temp2
    pop temp
    reti

/* USART Data Register Empty ISR */
usart_udre_isr:
    push temp
    push temp2
    push r30
    push r31
    in temp, _SFR_IO_ADDR(SREG)
    push temp
    
    /* Check if buffer empty */
    lds temp, tx_head
    lds temp2, tx_tail
    cp temp, temp2
    breq tx_buffer_empty
    
    /* Get data from buffer */
    ldi r30, lo8(tx_buffer)
    ldi r31, hi8(tx_buffer)
    add r30, temp2
    adc r31, zero
    ld temp, Z
    
    /* Send data */
    sts UDR0, temp
    
    /* Update tail */
    inc temp2
    andi temp2, BUFFER_MASK
    sts tx_tail, temp2
    
    rjmp udre_isr_done
    
tx_buffer_empty:
    /* Disable UDRE interrupt */
    lds temp, UCSR0B
    andi temp, ~(1<<UDRIE0)
    sts UCSR0B, temp
    
udre_isr_done:
    pop temp
    out _SFR_IO_ADDR(SREG), temp
    pop r31
    pop r30
    pop temp2
    pop temp
    reti

.section .progmem
hello_msg:
    .asciz "UART Ready - Echo Mode\r\n"
