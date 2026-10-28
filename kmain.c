#include <stdint.h>

static inline void outb(uint16_t port, uint8_t value) {
	__asm__ volatile ("outb %0, %1": : "a"(value), "Nd"(port));
}

static void serial_init(void) {
	outb(0x3F8 + 1, 0x00); // Disable interrupts
	outb(0x3F8 + 3, 0x80); // Enable divisor latch
	outb(0x3F8 + 0, 0x03); // Divisor low: 38400 baud
	outb(0x3F8 + 1, 0x00); // Divisor high
	outb(0x3F8 + 3, 0x03); // 8 bits, no parity, one stop bit
	outb(0x3F8 + 2, 0xC7); // Enable and clear FIFO
	outb(0x3F8 + 4, 0x0B); // Enable IRQs, RTS/DSR
}

static void serial_write(const char *s) {
	while (*s) {
		outb(0x3F8, (uint8_t)*s++);
	}
}

void kmain(void) {
	serial_init();
	serial_write("Hello from Little OS!\r\n");

	for (;;) {
		__asm__ volatile ("hlt");
	}
}
