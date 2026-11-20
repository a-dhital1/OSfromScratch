#ifndef PMM_H
#define PMM_H

#include <stdbool.h>
#include <stdint.h>
#include <limine.h>


bool pmm_init(const struct limine_memmap_response *map, uint64_t hhdm_offset);
bool pmm_alloc_page(uint64_t *physical_out);
bool pmm_free_page(uint64_t physical);

#endif
