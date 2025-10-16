#include <mcb.h>

#include <stddef.h>
#include <mpx/mpxio.h>
#include <mpx/kernel_pcb.h>

#include <mpx/vm.h>

mcb_t* heap_ptr = NULL;

void initialize_heap(size_t size)
{
    mcb_t* mcb_ptr = (mcb_t*)kmalloc(size, 0, NULL);

    mcb_ptr->prev_ptr = NULL;
    mcb_ptr->next_ptr = NULL;
    mcb_ptr->allocated = MCB_FREE;
    mcb_ptr->start_ptr = (char*)mcb_ptr + sizeof(mcb_t);
    mcb_ptr->size = size - sizeof(mcb_t);

    heap_ptr = mcb_ptr;

    return;
}

void* allocate_memory(size_t size)
{
    // find first free, sufficient mcb
    mcb_t* mcb_ptr = heap_ptr;
    for (; mcb_ptr != NULL; mcb_ptr = mcb_ptr->next_ptr)
    {
        if (mcb_ptr->allocated == MCB_ALLOCATED)
        {
            continue;
        }
        if (mcb_ptr->size < size)
        {
            continue;
        }
        break;
    }
    if (mcb_ptr == NULL)
    {
        // out of mcbs!
        return NULL;
    }

    // make sure an additional mcb would have enough size to at least have a header
    // otherwise, allocates an mcb that is slightly larger than requested w/o making a new one after
    if (mcb_ptr->size >= size + sizeof(mcb_t) + MCB_MIN_SIZE)
    {
        // split mcb into two
        mcb_split(mcb_ptr, size);
    }
    mcb_ptr->allocated = MCB_ALLOCATED;
    return (void*)mcb_ptr->start_ptr;
}

int free_memory(void* memory_ptr)
{
    // get mcb ptr from memory ptr
    mcb_t* mcb_ptr = (mcb_t*)((char*)memory_ptr - sizeof(mcb_t));

    // check this mcb exists
    for (mcb_t* a_ptr = heap_ptr;; a_ptr = a_ptr->next_ptr)
    {
        if (a_ptr == NULL) {
            // wasn't an existant mcb
            return -1;
        }
        if (mcb_ptr == a_ptr)
        {
            // found our mcb
            if (!mcb_ptr->allocated)
            {
                // wasnt allocated
                return -2;
            }
            break;
        }
    }

    mcb_ptr->allocated = MCB_FREE;

    if (mcb_ptr->next_ptr != NULL && mcb_ptr->next_ptr->allocated == MCB_FREE)
    {
        mcb_merge(mcb_ptr);
    }
    if (mcb_ptr->prev_ptr != NULL && mcb_ptr->prev_ptr->allocated == MCB_FREE)
    {
        mcb_merge(mcb_ptr->prev_ptr);
    }
    return 0;
}

mcb_t* mcb_split(mcb_t* mcb_ptr, size_t size)
{
    // create new mcb 
    mcb_t* mcb_next_ptr = (mcb_t*)(mcb_ptr->start_ptr + size);

    // initialize new mcb
    mcb_next_ptr->next_ptr = mcb_ptr->next_ptr;
    mcb_next_ptr->prev_ptr = mcb_ptr;
    mcb_next_ptr->allocated = mcb_ptr->allocated;
    mcb_next_ptr->size = mcb_ptr->size - (size + sizeof(mcb_t));
    mcb_next_ptr->start_ptr = (char*)mcb_next_ptr + sizeof(mcb_t);

    // modify og mcb
    mcb_ptr->size = size;
    mcb_ptr->next_ptr = mcb_next_ptr;

    // let next next know we exist
    if (mcb_next_ptr->next_ptr != NULL)
    {
        mcb_next_ptr->next_ptr->prev_ptr = mcb_next_ptr;
    }

    return mcb_next_ptr;
}

mcb_t* mcb_merge(mcb_t* mcb_ptr)
{
    // reclaim size
    mcb_ptr->size = mcb_ptr->size + sizeof(mcb_t) + mcb_ptr->next_ptr->size;

    // let next next know we exist
    if (mcb_ptr->next_ptr->next_ptr != NULL)
    {
        mcb_ptr->next_ptr->next_ptr->prev_ptr = mcb_ptr;
    }

    // orphan old next mcb
    mcb_ptr->next_ptr = mcb_ptr->next_ptr->next_ptr;

    return mcb_ptr;
}

// eof
