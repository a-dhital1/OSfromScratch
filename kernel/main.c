
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <kernel/boot.h>
#include <kernel/framebuffer.h>
#include <kernel/serial.h>
#include <kernel/pmm.h>

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
	
	/* Test Page Allocation */
	const struct limine_hhdm_response *hhdm = boot_hhdm();
	
	if (hhdm == NULL || !pmm_init(boot_memory_map(), hhdm->offset)) {
		serial_write("[PMM] Initialization failed\n");
		halt();
	}

	uint64_t physical;
	if (!pmm_alloc_page(&physical)) {
		serial_write("[PMM] ALLOCATION failed\n");
		halt();
	}

	serial_write("[PMM] Allocated physical frame ");
	serial_write_hex(physical);
	serial_write("\n");
	
	/* Test by allocating one frame, write to frame, read from frame, and free frame */
	volatile uint64_t *word = (volatile uint64_t *)(uintptr_t)(hhdm->offset + physical);

	const uint64_t pattern = UINT64_C(0x1122334455667788);
	*word = pattern;
	uint64_t readback = *word;

	if (!pmm_free_page(physical) || readback != pattern) {
		serial_write("[PMM] Page test failed\n");
		halt();
	}
	serial_write("[PMM] Page write, read, and free passed\n");

	/* Test by allocating two frames, write to both frame, read from both frames,
	   free one frame, reallocated freed frame, read new write, free both frames. */
	uint64_t first_physical;
	uint64_t second_physical;
	if (!pmm_alloc_page(&first_physical) || !pmm_alloc_page(&second_physical) || first_physical == second_physical) {
		serial_write("[PMM] Could not allocate two distinct frames\n");
	}
	
	volatile uint64_t *first_word = (volatile uint64_t *)(uintptr_t)(hhdm->offset + first_physical);
	volatile uint64_t *second_word = (volatile uint64_t *)(uintptr_t)(hhdm->offset + second_physical);

	const uint64_t first_pattern = UINT64_C(0x1122334455667788);
	const uint64_t second_pattern = UINT64_C(0x2233445566778899);

	*first_word = first_pattern;
	*second_word = second_pattern;

	if (*first_word != first_pattern || *second_word != second_pattern) {
		serial_write("[PMM] Two-frame readback failed\n");
		halt();
	}

	uint64_t reused_physical;
	if (!pmm_free_page(first_physical) || !pmm_alloc_page(&reused_physical) || reused_physical != first_physical) {
		serial_write("[PMM] Frame reuse test failed\n");
		halt();
	}

	const uint64_t reused_pattern = UINT64_C(0xAABBCCDDEEFF0011);
	*first_word = reused_pattern;

	if (*first_word != reused_pattern || *second_word != second_pattern) {
		serial_write("[PMM] Readback after frame reuse failed\n");
		halt();
	}

	serial_write("[PMM] Two-frame allocation, readback, and reuse passed\n");

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

