
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <limine.h>
#include <stdint.h>
#include <stdbool.h>

bool framebuffer_supported(
        const struct limine_framebuffer *fb
);

void framebuffer_putpixel(
        struct limine_framebuffer *fb,
        uint64_t x,
        uint64_t y,
        uint8_t red,
        uint8_t green,
        uint8_t blue
);

void framebuffer_pattern(
        struct limine_framebuffer *fb
);

#endif

