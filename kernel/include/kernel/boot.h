
#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>
#include <stdbool.h>
#include <limine.h>

bool boot_protocol_supported(void);

uint64_t boot_framebuffer_count(void);

struct limine_framebuffer *boot_framebuffer(uint64_t index);

const struct limine_memmap_response *boot_memory_map(void);

const struct limine_hhdm_response *boot_hhdm(void);

#endif

