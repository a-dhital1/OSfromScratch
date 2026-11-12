
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <kernel/framebuffer.h>

bool framebuffer_supported(const struct limine_framebuffer *fb) {
        return fb != NULL &&
               fb->memory_model == LIMINE_FRAMEBUFFER_RGB &&
               fb->bpp == 32;
}

static uint32_t channel(uint8_t value, uint8_t bits, uint8_t shift) {
        uint64_t max = ((uint64_t)1 << bits) - 1;
        return (uint32_t)((value * max / 255) << shift);
}

static uint32_t rgb(struct limine_framebuffer *fb, uint8_t red, uint8_t green, uint8_t blue) {
        return channel(red, fb->red_mask_size, fb->red_mask_shift) 
             | channel(green, fb->green_mask_size, fb->green_mask_shift) 
             | channel(blue, fb->blue_mask_size, fb->blue_mask_shift);
}

void framebuffer_putpixel(struct limine_framebuffer *fb, uint64_t x, uint64_t y, uint8_t red, uint8_t green, uint8_t blue) {
        if (!framebuffer_supported(fb)) return;
        if (x >= fb->width || y >= fb->height) return;

        volatile uint32_t *pixels = fb->address;
        size_t stride = fb->pitch / sizeof(uint32_t);

        pixels[y * stride + x] = rgb(fb, red, green, blue);
}

void framebuffer_pattern(struct limine_framebuffer *fb) {
        if (!framebuffer_supported(fb)) return;

        for (uint64_t y = 0; y < fb->height; y++) {
                for (uint64_t x = 0; x < fb->width; x++) {
                        uint8_t blue = (uint8_t)(x * 255 / fb->width);

                        uint8_t green = (uint8_t)(y * 255 / fb->height);

                        framebuffer_putpixel(fb, x, y, 0, green, blue);
                }
        }
}

