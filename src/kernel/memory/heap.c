/*
 * File:		heap.c
 * *****************************************************************************
 * Copyright 2020 Scott Maday
 * You should have received a copy of the GNU General Public License along with this program. 
 * If not, see https://www.gnu.org/licenses/gpl-2.0
 */

#include "heap.h"

// Start of heap
struct PageTableEntry* start;

// TODO pretty much redo all this when virtual memory implemented
// TODO WAY LATER mutex locks on table entries
void heap_init(struct MemoryRegion* base, size_t offset){
	start = (struct PageTableEntry*)(base->base + offset);
	start->size = base->size - sizeof(struct PageTableEntry);
	start->next = NULL;
	start->properties = MEMORY_HEAP_PROPERTY_FREE;
}

void* malloc(size_t size) {
	size_t overhead = sizeof(struct PageTableEntry);
	size_t mod = size % MEMORY_HEAP_PAGE_SIZE;
	size -= mod;
	if(mod != 0){
		size += MEMORY_HEAP_PAGE_SIZE;
	}
	// Get current page
	struct PageTableEntry* current = start;
	while((current->properties & MEMORY_HEAP_PROPERTY_FREE) != MEMORY_HEAP_PROPERTY_FREE || current->size < size){
		if(current->next == NULL) { // Uh oh
			return 0;
		}
		current = current->next;
	}
	// Setup next
	if(current->size > size + overhead){
		struct PageTableEntry* next = (struct PageTableEntry*)((void*)current + size + overhead);
		next->next = current->next;
		next->size = current->size - overhead + size;
		next->properties = MEMORY_HEAP_PROPERTY_FREE;

		current->next = next;
		current->size = size;
		current->properties &= ~MEMORY_HEAP_PROPERTY_FREE;
	}
	if(current == start){
		start = current->next;
	}
	return current + 1;
}

void free(void* address) {
	
}