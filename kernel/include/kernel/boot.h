
#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>
#include <stdbool.h>
#include <limine.h>

bool boot_protocol_supported(void);

uint64_t boot_framebuffer_count(void);

struct limine_framebuffer *boot_framebuffer(
        uint64_t index
);

#endif

