#include <stddef.h>
#include <string.h>

#include <kernel/pmm.h>

enum { PAGE_SIZE = 4096 };

static uint8_t *frame_bitmap;
static uint64_t frame_count;
static uint64_t bitmap_base;
static uint64_t bitmap_end;
static const struct limine_memmap_response *physical_map;


/* helper to check if frame is unavailable  */
static bool frame_unavailable(uint64_t frame) {
	uint8_t mask = (uint8_t)(1u << (frame % 8));
	return (frame_bitmap[frame / 8] & mask) != 0;
}

/* helper to set a frame as unavailable */
static void set_frame_unavailable(uint64_t frame, bool unavailable) {
	uint8_t *byte = &frame_bitmap[frame / 8];
	uint8_t mask = (uint8_t)(1u << (frame % 8));

	if (unavailable) {
		*byte |= mask;
	} else {
		*byte &= (uint8_t)~mask;
	}
}

/* Initialize bitmap */
bool pmm_init(const struct limine_memmap_response *map, uint64_t hhdm_offset) {
	if (map == NULL || frame_bitmap != NULL) {
		return false;
	}

	uint64_t highest_end = 0;
	for (uint64_t i = 0; i < map->entry_count; ++i) {
		const struct limine_memmap_entry *entry = map->entries[i];
		if (entry->type != LIMINE_MEMMAP_USABLE || entry->length == 0) {
			continue;
		}

		if (entry->length > UINT64_MAX - entry->base) {
			return false;
		}
		uint64_t end = entry->base + entry->length;
		if (end > highest_end) {
			highest_end = end;
		}
	}

	uint64_t frames = highest_end / PAGE_SIZE;
	uint64_t bitmap_bytes = (frames + 7) / 8;
	if (bitmap_bytes == 0) {
		return false;
	}

	uint64_t bitmap_pages = (bitmap_bytes  + PAGE_SIZE - 1) / PAGE_SIZE;
	uint64_t storage_bytes = bitmap_pages * PAGE_SIZE;
	const struct limine_memmap_entry *storage = NULL;

	for (uint64_t i = 0; i < map->entry_count; ++i) {
	const struct limine_memmap_entry *entry = map->entries[i];
		if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= storage_bytes) {
			storage = entry;
			break;
		}
	}
	if (storage == NULL) {
		return false;
	}

	physical_map = map;
	frame_count = frames;
	bitmap_base = storage->base;
	bitmap_end = bitmap_base + storage_bytes;
	frame_bitmap = (uint8_t *)(uintptr_t)(hhdm_offset + bitmap_base);
	memset(frame_bitmap, 0xFF, (size_t)bitmap_bytes);

	for (uint64_t i = 0; i < map->entry_count; ++i) {
		const struct limine_memmap_entry *entry = map->entries[i];
		if (entry->type == LIMINE_MEMMAP_USABLE) {
			uint64_t first = entry->base / PAGE_SIZE;
			uint64_t count = entry->length / PAGE_SIZE;
			for (uint64_t frame = first; frame < first + count; ++frame) {
				set_frame_unavailable(frame, false);
			}
		}
	}

	for (uint64_t frame = bitmap_base / PAGE_SIZE; frame < bitmap_end / PAGE_SIZE; ++frame) {
		set_frame_unavailable(frame, true);
	}

	return true;

}

/* Allocate page */
bool pmm_alloc_page(uint64_t *physical_out) {
	if (frame_bitmap == NULL || physical_out == NULL) {
		return false;
	}

	for (uint64_t frame = 0; frame < frame_count; ++frame) {
		if (!frame_unavailable(frame)) {
			set_frame_unavailable(frame, true);
			*physical_out = frame * PAGE_SIZE;
			return true;
		}
	}

	return false;
}

/* Free page */
bool pmm_free_page(uint64_t physical) {
	uint64_t frame = physical / PAGE_SIZE;
	if (frame_bitmap == NULL || physical % PAGE_SIZE != 0 || frame >= frame_count || (physical >= bitmap_base && physical < bitmap_end) || !frame_unavailable(frame)) {
		return false;
	}

	for (uint64_t i = 0; i < physical_map->entry_count; ++i) {
		const struct limine_memmap_entry *entry = physical_map->entries[i];
		if (entry->type == LIMINE_MEMMAP_USABLE && physical >= entry->base && physical - entry->base < entry->length) {
			set_frame_unavailable(frame, false);
			return true;
		}
	}

	return false;
}

