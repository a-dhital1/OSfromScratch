
#include <stdint.h>
#include <stdbool.h>

#include <kernel/boot.h>
#include <kernel/framebuffer.h>

static void halt(void) {
        for (;;) {
                __asm__ volatile ("hlt");
        }
}

void kmain(void) {
        /* Verify the boot protocol. */
        if (!boot_protocol_supported()) {
                halt();
        }

        /* Ensure a framebuffer is available. */
        uint64_t count = boot_framebuffer_count();

        if (count == 0) {
                halt();
        }

        /* Draw to each available framebuffer. */
        for (uint64_t i = 0; i < count; i++) {
                struct limine_framebuffer *fb = boot_framebuffer(i);

                if (!framebuffer_supported(fb)) {
                        halt();
                }

                framebuffer_pattern(fb);
        }

        halt();
}

