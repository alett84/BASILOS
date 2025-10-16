#ifndef MPX_MCB_H
#define MPX_MCB_H

#include <stddef.h>

// R5 MANUAL THIS

#define MCB_FREE 0u
#define MCB_ALLOCATED 1u

// you can malloc(0) in standard C so idk, this is fine
#define MCB_MIN_SIZE 0

struct mcb {
    struct mcb* prev_ptr;
    struct mcb* next_ptr;
    unsigned int allocated : 1;
    char* start_ptr;
    size_t size;

};
typedef struct mcb mcb_t;

extern mcb_t* heap_ptr;

/**
 initializes the heap, and sets the global heap head pointer to said heap
 @param size size of heap
 */
void initialize_heap(size_t size);

/**
 allocates some memory
 @param size size of memory block to allocate
 @return pointer to newly allocated memory, or NULL on error
 */
void* allocate_memory(size_t size);

/**
 frees some memory no longer in use
 @param memory_ptr pointer to memory to free
 @return 0 on success, otherwise on error
 */
int free_memory(void* memory_ptr);

/**
 splits a given mcb in twain
 @param mcb_ptr mcb to split in twain, becomes 1st mcb of pair. mem size will be size param
 @param size mem size of 1st mcb of pair
 @return 2nd mcb of pair. mem size will be OG mcb's mem size - sizeof(mcb_t) + size
 */
mcb_t* mcb_split(mcb_t* mcb_ptr, size_t size);

/**
 merges an mcb with its next mcb. inherits all header data from 1st mcb.
 @param mcb_ptr 1st mcb to merge. 2nd mcb will be its next mcb
 @return mcbs merged 
 */
mcb_t* mcb_merge(mcb_t* mcb_ptr);

#endif
