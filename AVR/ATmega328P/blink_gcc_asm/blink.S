#include <avr/io.h>

/* Register Definitions using C Preprocessor */
#define mask    r16
#define ledR    r17
#define oLoopR  r18
#define iLoopRl r24
#define iLoopRh r25

/* Constants */
#define oVal    71
#define iVal    28168

.section .vectors
.global __vectors
__vectors:
    rjmp _start           ; The Reset Vector now points to our standard entry point

.section .text
.global _start            ; Standard entry point name

_start:
    clr     ledR            ; clear led register
    ldi     mask, (1 << PB5)
    out     _SFR_IO_ADDR(DDRB), mask  ; Set PB5 to output

start_loop:
    eor     ledR, mask      ; toggle PB5 in led register
    out     _SFR_IO_ADDR(PORTB), ledR ; write led register to PORTB

    ldi     oLoopR, oVal    ; initialize outer loop count

oLoop:
    ldi     iLoopRl, lo8(iVal) ; initialize inner loop low (GAS uses lo8)
    ldi     iLoopRh, hi8(iVal) ; initialize inner loop high (GAS uses hi8)

iLoop:
    sbiw    iLoopRl, 1      ; decrement inner loop pair (r25:r24)
    brne    iLoop           ; branch if not zero

    dec     oLoopR          ; decrement outer loop register
    brne    oLoop           ; branch if not zero

    rjmp    start_loop      ; jump back to start
