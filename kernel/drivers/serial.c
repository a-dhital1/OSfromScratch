#include <kernel/serial.h>
#include <stdint.h>

enum {
	COM1 = 0x3F8,
	UART_DATA = COM1 + 0,
	UART_IER  = COM1 + 1,
	UART_FCR  = COM1 + 2,
	UART_LCR  = COM1 + 3,
	UART_MCR  = COM1 + 4,
	UART_LSR  = COM1 + 5,

	UART_RX_READY = 1 << 0,
	UART_TX_READY = 1 << 5,
	UART_POLL_LIMIT = 100000
};

static bool serial_ready;

static void outb(uint16_t port, uint8_t value) {
	__asm__ volatile ("outb %b0, %w1" : : "a"(value), "Nd"(port));
}

static uint8_t inb(uint16_t port) {
	uint8_t value;
	__asm__ volatile ("inb %w1, %b0" : "=a"(value) : "Nd"(port));
	return value;
}

static bool uart_wait(uint8_t status_bit) {
	for (unsigned i = 0; i < UART_POLL_LIMIT; ++i) {
		if (inb(UART_LSR) & status_bit) {
			return true;
		}
	}
	return false;
}

bool serial_init(void) {
	serial_ready = false;

	outb(UART_LCR, 0x03);   // Clear DLAB so port +1 selects the interrupt register.
	outb(UART_IER, 0x00);   // Disable UART interrupts. This driver polls.
	outb(UART_LCR, 0x80);   // Set DLAB to access the baud-rate divisor.
	outb(UART_DATA, 0x03);  // Divisor low byte. Divisor 3 gives 38,400 baud.
	outb(UART_IER, 0x00);   // Divisor high byte while DLAB is set.
	outb(UART_LCR, 0x03);   // Clear DLAB and select 8 data bits, no parity, 1 stop bit.
	outb(UART_FCR, 0x07);   // Enable and clear both FIFOs.

	outb(UART_MCR, 0x13);   // Enable internal loopback, DTR and RTS.
	outb(UART_DATA, 0xAE);  // Send a test byte back to the same UART.
	serial_ready = uart_wait(UART_RX_READY) && inb(UART_DATA) == 0xAE;
	outb(UART_MCR, 0x03);   // Exit loopback, leaving DTR and RTS enabled.

	return serial_ready;
}

void serial_write_char(char c) {
	if (serial_ready && uart_wait(UART_TX_READY)) {
		outb(UART_DATA, (uint8_t)c);
	}
}

void serial_write(const char *str) {
	while (*str != '\0') {
		if (*str == '\n') {
			serial_write_char('\r');
		}
		serial_write_char(*str++);
	}
}

void serial_write_hex(uint64_t value) {
	const char *digits = "0123456789ABCDEF";

	serial_write("0x");

	for (int shift = 60; shift >= 0; shift -= 4) {
		unsigned nibble = (value >> shift) & 0xF;
		serial_write_char(digits[nibble]);
	}
}
