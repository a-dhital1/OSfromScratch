
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <limine.h>

#include <kernel/boot.h>

/* Limine base revision. */
__attribute__((used, section(".limine_requests"))) static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

/* Request framebuffer information. */
__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {
        .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
        .revision = 0
};

/* Print Physical Memory Map. */
__attribute__((used, section(".limine_requests"))) static volatile struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST_ID,
	.revision = 0,
};

/* Start of Limine request area. */
__attribute__((used, section(".limine_requests_start"))) static volatile uint64_t requests_start[] = LIMINE_REQUESTS_START_MARKER;

/* End of Limine request area. */
__attribute__((used, section(".limine_requests_end"))) static volatile uint64_t requests_end[] = LIMINE_REQUESTS_END_MARKER;

bool boot_protocol_supported(void) {
        return LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision);
}

uint64_t boot_framebuffer_count(void) {
        if (framebuffer_request.response == NULL) {
                return 0;
        }

        return framebuffer_request.response->framebuffer_count;
}

struct limine_framebuffer *boot_framebuffer(uint64_t index) {
        if (framebuffer_request.response == NULL) {
                return NULL;
        }

        if (index >= boot_framebuffer_count()) {
                return NULL;
        }

        return framebuffer_request.response->framebuffers[index];
}

const struct limine_memmap_response *boot_memory_map(void) {
	return memmap_request.response;
}
