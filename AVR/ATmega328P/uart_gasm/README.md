# ATmega328P UART Driver in GNU Assembler

This project implements an interrupt-driven UART driver with ring buffers in pure GNU assembler for the ATmega328P microcontroller.

## Features

- **Interrupt-driven UART**: Uses RX Complete and Data Register Empty interrupts
- **Ring buffers**: 64-byte circular buffers for both TX and RX
- **Two demo programs**: Basic echo mode and advanced command interface
- **9600 baud**: Configured for 115200 baud rate at 16MHz clock
- **Pure assembler**: Written entirely in GNU assembler

## Hardware Setup

- ATmega328P microcontroller
- 16MHz crystal oscillator
- UART connection on pins PD0 (RX) and PD1 (TX)

## Building

```bash
make all        # Build basic echo demo (uart_demo)
make advanced   # Build advanced command demo (uart_advanced)
make program    # Flash basic demo to microcontroller
make clean      # Clean build files
```

## Programs

### Basic Demo (uart_demo.S)
- Simple echo mode
- Receives characters and echoes them back
- Sends welcome message on startup

### Advanced Demo (uart_advanced.S)
- Interactive command interface
- Commands:
  - `1`: Send hello message
  - `2`: Send message counter (in hex)
  - `3`: Show menu
  - Other keys: Echo mode
- Demonstrates hex output formatting

## UART Configuration

- Baud rate: 115200
- Data bits: 8
- Stop bits: 1
- Parity: None
- Flow control: None

## Ring Buffer Implementation

- Buffer size: 64bytes (power of 2 for efficient masking)
- Separate TX and RX buffers
- Head/tail pointers for circular operation
- Interrupt-safe operations
- Automatic overflow protection

## Functions

- `uart_init`: Initialize UART hardware
- `uart_transmit`: Queue character for transmission
- `uart_receive`: Get received character from buffer
- `uart_rx_available`: Check if data is available
- `uart_send_string`: Send null-terminated string
- `send_hex_byte`: Convert byte to hex ASCII (advanced demo)

## Interrupts

- **USART_RX_vect**: Handles received characters, stores in RX ring buffer
- **USART_UDRE_vect**: Handles transmission, sends from TX ring buffer

## Memory Usage

- TX Buffer: 64 bytes
- RX Buffer: 64 bytes
- Buffer pointers: 4 bytes
- Total RAM: ~132 bytes

## Technical Details

- Uses GNU assembler syntax
- Interrupt vectors properly aligned
- Stack pointer initialized to RAMEND
- Global interrupts enabled after initialization
- Efficient bit masking for circular buffer indexing