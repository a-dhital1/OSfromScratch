#ifndef SERIAL_H
#define SERIAL_H

#include <stdbool.h>
#include <stdint.h>

bool serial_init(void);
void serial_write_char(char c);
void serial_write(const char *str);
void serial_write_hex(uint64_t value);

#endif
