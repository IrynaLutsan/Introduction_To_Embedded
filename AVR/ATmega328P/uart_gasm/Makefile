# --- Configuration Variables ---
TARGET = uart_demo
MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMER_TYPE = arduino
PORT = /dev/cu.usbmodem144301
BAUD_RATE = 115200
LINKER_SCRIPT = atmega328p.ld

# --- Toolchain Definitions ---
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
OBJDUMP = avr-objdump

# --- Compilation and Linking Flags ---
CFLAGS = -mmcu=$(MCU) -I. -D_GNU_SOURCE -DF_CPU=$(F_CPU) -Os -std=gnu99 -Wall -Wextra -gstabs

# -nostdlib: This is a "mega-flag" that combines -nodefaultlibs AND -nostartfiles
LDFLAGS = -mmcu=$(MCU) -nostdlib -Wl,-gc-sections -Wl,-Map=$(TARGET).map -T$(LINKER_SCRIPT)

# --- File Definitions ---
C_SRC = $(wildcard *.c)
AS_SRC = $(TARGET).S
OBJ = $(C_SRC:.c=.o) $(AS_SRC:.S=.o)

# --- Main Targets ---
all: $(TARGET).elf $(TARGET).hex $(TARGET).map

advanced:
	$(MAKE) TARGET=uart_advanced program

$(TARGET).elf: $(OBJ) $(LINKER_SCRIPT)
	@echo "Linking without standard startup files..."
	$(CC) $(LDFLAGS) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(TARGET).s: $(TARGET).elf
	$(OBJDUMP) -h -S $< > $@

program: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) -P $(PORT) -b $(BAUD_RATE) -U flash:w:$<:i

clean:
	rm -f *.o *.elf *.hex *.map *.lst *.s

.PHONY: all clean program advanced