#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "uart.h"

#define RING_BUFFER_SIZE 32
#define RING_BUFFER_MASK (RING_BUFFER_SIZE - 1)

#if (RING_BUFFER_SIZE & RING_BUFFER_MASK)
#error Ring buffer size is not a power of 2
#endif

/* ATmega with one USART */
 #define UART0_RECEIVE_INTERRUPT   USART_RX_vect
 #define UART0_TRANSMIT_INTERRUPT  USART_UDRE_vect
 #define UART0_STATUS      UCSR0A
 #define UART0_CONTROL     UCSR0B
 #define UART0_CONTROLC    UCSR0C
 #define UART0_DATA        UDR0
 #define UART0_UDRIE       UDRIE0
 #define UART0_UBRRL       UBRR0L
 #define UART0_UBRRH       UBRR0H
 #define UART0_BIT_U2X     U2X0
 #define UART0_BIT_RXCIE   RXCIE0
 #define UART0_BIT_RXEN    RXEN0
 #define UART0_BIT_TXEN    TXEN0
 #define UART0_BIT_UCSZ0   UCSZ00
 #define UART0_BIT_UCSZ1   UCSZ01

typedef struct RingBuffer{
    volatile unsigned char buf[RING_BUFFER_SIZE];
    volatile unsigned char head;
    volatile unsigned char tail;
} RingBuffer_t;

inline void RingBufInit(RingBuffer_t *b){
    b->head = b->tail;
}

inline int IsRingBufEmpty(RingBuffer_t *b){
    return b->head == b->tail;
}

static RingBuffer_t UART_Rx_Buf;
static RingBuffer_t UART_Tx_Buf;

static volatile unsigned char UART_LastRxError;

ISR (UART0_RECEIVE_INTERRUPT)	
/*************************************************************************
Function: UART Receive Complete interrupt
Purpose:  called when the UART has received a character
**************************************************************************/
{
    unsigned char data;
    unsigned char usr;
    unsigned char lastRxError;
    unsigned char tmphead;
 
    /* read UART status register and UART data register */
    usr  = UART0_STATUS;
    data = UART0_DATA;
    
    /* get FEn (Frame Error) DORn (Data OverRun) UPEn (USART Parity Error) bits */
#if defined(FE) && defined(DOR) && defined(UPE)
    lastRxError = usr & (_BV(FE)|_BV(DOR)|_BV(UPE) );
#elif defined(FE0) && defined(DOR0) && defined(UPE0)
    lastRxError = usr & (_BV(FE0)|_BV(DOR0)|_BV(UPE0) );
#elif defined(FE1) && defined(DOR1) && defined(UPE1)
    lastRxError = usr & (_BV(FE1)|_BV(DOR1)|_BV(UPE1) );
#elif defined(FE) && defined(DOR)
    lastRxError = usr & (_BV(FE)|_BV(DOR) );
#endif

    tmphead = (UART_Rx_Buf.head + 1) & RING_BUFFER_MASK;

    if (tmphead == UART_Rx_Buf.tail) {
        /* error: receive buffer overflow */
        lastRxError = UART_BUFFER_OVERFLOW >> 8;
    }else{
        /* store new index */
        UART_Rx_Buf.head = tmphead;
        UART_Rx_Buf.buf[tmphead] = data;
    }
    UART_LastRxError |= lastRxError;   
}

ISR (UART0_TRANSMIT_INTERRUPT)
/*************************************************************************
Function: UART Data Register Empty interrupt
Purpose:  called when the UART is ready to transmit the next byte
**************************************************************************/
{
    if (IsRingBufEmpty(&UART_Tx_Buf)) {
        /* tx buffer empty, disable UDRE interrupt */
        UART0_CONTROL &= ~_BV(UART0_UDRIE);
    }else{
        UART_Tx_Buf.tail = (UART_Tx_Buf.tail + 1) & RING_BUFFER_MASK;
        UART0_DATA = UART_Tx_Buf.buf[UART_Tx_Buf.tail];
    }
}

void uart_init(unsigned int baud)
/*************************************************************************
Function:    uart_init()
Purpose:     initialize UART and set baudrate
Input:       baudrate using macro UART_BAUD_SELECT()
Returns:     none
**************************************************************************/
{
    RingBufInit(&UART_Rx_Buf);
    RingBufInit(&UART_Tx_Buf);

    /* Set baud rate */
    if ( baud & 0x8000 ) {
        UART0_STATUS = (1<<UART0_BIT_U2X);  //Enable 2x speed
        baud = baud & 0x7FFF;
    }
    UART0_UBRRL = baud; //set baud rate
    UART0_UBRRH = (baud>>8);

    /* Enable USART receiver and transmitter and receive complete interrupt */
    UART0_CONTROL = _BV(UART0_BIT_RXCIE) | _BV(UART0_BIT_RXEN) | _BV(UART0_BIT_TXEN);

    /* Set frame format: asynchronous, 8data, no parity, 1stop bit */
    UART0_CONTROLC = _BV(UART0_BIT_UCSZ1) | _BV(UART0_BIT_UCSZ0);
}

/*************************************************************************
Function: uart_getc()
Purpose:  return byte from ringbuffer  
Returns:  lower byte:  received byte from ringbuffer
          higher byte: last receive error
**************************************************************************/
unsigned int uart_getc(void)
{
    unsigned char tmptail;
    unsigned char data;

    if (IsRingBufEmpty(&UART_Rx_Buf)){
        return UART_NO_DATA;   /* no data available */
    }

    /* calculate /store buffer index */
    tmptail = (UART_Rx_Buf.tail + 1) & RING_BUFFER_MASK;
    UART_Rx_Buf.tail = tmptail;

    /* get data from receive buffer */
    data =  UART_Rx_Buf.buf[tmptail];
    return (UART_LastRxError << 8) + data;
}

/*************************************************************************
Function: uart_putc()
Purpose:  write byte to ringbuffer for transmitting via UART
Input:    byte to be transmitted
Returns:  none
**************************************************************************/
void uart_putc(unsigned char data)
{
    unsigned char tmphead;


    tmphead = (UART_Tx_Buf.head + 1) & RING_BUFFER_MASK;

    while ( tmphead == UART_Tx_Buf.tail ){ /* wait for free space in buffer */
        ;
    }

    UART_Tx_Buf.buf[tmphead] = data;
    UART_Tx_Buf.head = tmphead;

    /* enable UDRE interrupt */
    UART0_CONTROL    |= _BV(UART0_UDRIE);
}

/*************************************************************************
Function: uart_puts()
Purpose:  transmit string to UART
Input:    string to be transmitted
Returns:  none
**************************************************************************/
void uart_puts(const char *s)
{
    while (*s){
        uart_putc(*s++);
    }
}

/*************************************************************************
Function: uart_puts_p()
Purpose:  transmit string from program memory to UART
Input:    program memory string to be transmitted
Returns:  none
**************************************************************************/
void uart_puts_p(const char *progmem_s)
{
    unsigned char c;

    while ( (c = pgm_read_byte(progmem_s++)) ){
        uart_putc(c);
    }
}






