
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <kernel/boot.h>
#include <kernel/framebuffer.h>
#include <kernel/serial.h>

static void halt(void) {
        for (;;) {
                __asm__ volatile ("hlt");
        }
}

static void print_memory_map(void) {
	const struct limine_memmap_response *map = boot_memory_map();

	if (map == NULL) {
		serial_write("[MEM] Limine did not provide a memory map\n");
		return;
	}

	uint64_t usable_bytes = 0;

	serial_write("[MEM] Physical memory map\n");
	for (uint64_t i = 0; i < map->entry_count; i++) {
		const struct limine_memmap_entry *entry = map->entries[i];

		serial_write(" base=");
		serial_write_hex(entry->base);
		serial_write(" length=");
		serial_write_hex(entry->length);
		serial_write(" type=");
		serial_write_hex(entry->type);

		if (entry->type == LIMINE_MEMMAP_USABLE) {
			usable_bytes += entry->length;
			serial_write(" (usable)");
		}

		serial_write("\n");
	}

	serial_write("[MEM] Total usable RAM: ");
	serial_write_hex(usable_bytes);
	serial_write(" bytes \n");

	serial_write("[MEM] Usable 4 KiB pages: ");
	serial_write_hex(usable_bytes / 4096);
	serial_write("\n");
}

void kmain(void) {
        /* Verify the boot protocol. */
        if (!boot_protocol_supported()) {
                halt();
        }
	
	/* Memory Map */
	if (serial_init()) {
		serial_write("[RemSSOS] Entered kmain\n");
		print_memory_map();
		
		const struct limine_hhdm_response *hhdm = boot_hhdm();

		if (hhdm == NULL) {
			serial_write("[MEM] Limine did not provide an HHDM response\n");
		} else {
			serial_write("[MEM] HHDM offset: ");
			serial_write_hex(hhdm->offset);
			serial_write("\n");
		}
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

