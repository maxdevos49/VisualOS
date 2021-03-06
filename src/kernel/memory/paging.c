/*
 * File:		paging.c
 * *****************************************************************************
 * Copyright 2020 Scott Maday
 * You should have received a copy of the GNU General Public License along with this program. 
 * If not, see https://www.gnu.org/licenses/gpl-2.0
 */

#include "paging.h"
#include "bitmap.h"


size_t g_memory_total_size = 0;
size_t g_memory_used_size = 0;
size_t g_memory_reserved_size = 0;


void init_paging(struct MemoryDescriptor* mem_map, size_t mem_map_size, size_t mem_map_descriptor_size) {
	g_memory_total_size = 0;
	g_memory_used_size = 0;
	g_memory_reserved_size = 0;
	uint num_enteries = mem_map_size / mem_map_descriptor_size;
	struct MemoryDescriptor* largest_primary = NULL;
	// Find largest primary memory region
	for(uint i = 0; i < num_enteries; i++){
		struct MemoryDescriptor* descriptor = (struct MemoryDescriptor*)((void*)mem_map + (i * mem_map_descriptor_size));
		if(descriptor->type == MEMORY_PAGEABLE_TYPE && (largest_primary == NULL || descriptor->num_pages > largest_primary->num_pages)){
			largest_primary = descriptor;
		}
		g_memory_total_size += descriptor->num_pages * BOOTLOADER_MEMORY_PAGE_SIZE;
	}
	// Assign bitmap
	bitmap_initialize(&g_pagemap, largest_primary->phys_address, g_memory_total_size / MEMORY_PAGE_SIZE / BITMAP_SCALE);
	// Reserve the g_pagemap itself
	size_t g_pagemap_pages = g_pagemap.size % MEMORY_PAGE_SIZE > 0 ? g_pagemap.size / MEMORY_PAGE_SIZE + 1 : g_pagemap.size / MEMORY_PAGE_SIZE;
	page_reserve(g_pagemap.buffer, g_pagemap_pages);
	// Reserve kernel space
	size_t kernel_size = (size_t)&_kernel_end - (size_t)&_kernel_start;
	size_t kernel_pages = kernel_size % MEMORY_PAGE_SIZE > 0 ? kernel_size / MEMORY_PAGE_SIZE + 1 : kernel_size / MEMORY_PAGE_SIZE;
	page_reserve(&_kernel_start, kernel_pages);
	// Reserve unusable memory segments
	for(uint i = 0; i < num_enteries; i++){
		struct MemoryDescriptor* descriptor = (struct MemoryDescriptor*)((void*)mem_map + (i * mem_map_descriptor_size));
		if(descriptor->type == MEMORY_PAGEABLE_TYPE){
			continue;
		}
		size_t reserved_size = descriptor->num_pages * BOOTLOADER_MEMORY_PAGE_SIZE;
		page_reserve(descriptor->phys_address, reserved_size / MEMORY_PAGE_SIZE);
	}
}

size_t paging_get_free() {
	return g_memory_total_size - g_memory_used_size - g_memory_reserved_size;
}

bool page_manipulate(ulong index, bool state) {
	if(bitmap_get(&g_pagemap, index) == state){
		return true;	// already in state
	}
	bitmap_set(&g_pagemap, index, state);
	return false;
}

void* page_request() {
	for(ulong i = 0; i < bitmap_adjusted_size(&g_pagemap); i++){
		if(bitmap_get(&g_pagemap, i) == true){
			continue;	// not free
		}
		void* page = (void*)(i * MEMORY_PAGE_SIZE);	// transform the index into the page address
		page_lock(page, 1);
		return page;
	}
	// TODO Page frame swap to file
	return NULL;
}

void page_free(void* address, size_t pages) {
	ulong start = (ulong)address / MEMORY_PAGE_SIZE;
	for(ulong i = start; i < start + pages; i++){
		if(page_manipulate(i, false)){
			if(g_memory_used_size >= MEMORY_PAGE_SIZE) {		// prevent overflow
				g_memory_used_size -= MEMORY_PAGE_SIZE;
			}
		}
	}
}

void page_lock(void* address, size_t pages) {
	ulong start = (ulong)address / MEMORY_PAGE_SIZE;
	for(ulong i = start; i < start + pages; i++){
		if(page_manipulate(i, true)){
			g_memory_used_size += MEMORY_PAGE_SIZE;
		}
	}
}

void page_unreserve(void* address, size_t pages) {
	ulong start = (ulong)address / MEMORY_PAGE_SIZE;
	for(ulong i = start; i < start + pages; i++){
		if(page_manipulate(i, false)){
			if(g_memory_reserved_size >= MEMORY_PAGE_SIZE) {	// prevent overflow
				g_memory_reserved_size -= MEMORY_PAGE_SIZE;
			}
		}
	}
}

void page_reserve(void* address, size_t pages) {
	ulong start = (ulong)address / MEMORY_PAGE_SIZE;
	for(ulong i = start; i < start + pages; i++){
		if(page_manipulate(i, true)){
			g_memory_reserved_size += MEMORY_PAGE_SIZE;
		}
	}
}