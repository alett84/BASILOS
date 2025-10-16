#include <mpx/kernel_pcb.h>
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

pcb_t* idle_pcb = NULL; 

pcb_t* pcb_ptrs[] = {NULL, NULL, NULL};

pcb_t* pcb_allocate(void) 
{
    pcb_t *pcb_ptr = sys_alloc_mem(sizeof(pcb_t));
    if (pcb_ptr == NULL)
    {
        return NULL;
    }
    pcb_ptr->stack_ptr = pcb_ptr->stack + sizeof(pcb_ptr->stack) - 2;
    return pcb_ptr;
}

int pcb_free(pcb_t* pcb_ptr)
{
    return sys_free_mem(pcb_ptr);
}

pcb_t* pcb_setup(const char *name, int class_type, int priority)
{
    if (strlen(name) > 8)
    {
        return NULL;
    }
    pcb_t* pcb_ptr = pcb_allocate();
    if (pcb_ptr == NULL)
    {
        return NULL;
    }
    memcpy(pcb_ptr->name, name, strlen(name));
    pcb_ptr->class_type = (unsigned int)class_type;
    pcb_ptr->priority = (unsigned int)priority;
    pcb_ptr->eax_override = 0;
    pcb_ptr->state = PCB_READY;
    return pcb_ptr;
}

pcb_t* pcb_find(const char *name)
{
    for (unsigned int i = 0; i < sizeof(pcb_ptrs) / sizeof(pcb_t*); i++)
    {  
        if (pcb_ptrs[i] != NULL)
        {
            for (pcb_t* pcb_a_ptr = pcb_ptrs[i]; pcb_a_ptr != NULL; pcb_a_ptr = pcb_a_ptr->next_ptr)
            {
                if (strcmp(pcb_a_ptr->name, name) == 0)
                {
                    return pcb_a_ptr;
                }
            }
        }
    }
    return NULL;
}

void pcb_insert(pcb_t* pcb_ptr)
{
    if (pcb_ptr->state == PCB_READY)
    {
        if (pcb_ptr->suspended == PCB_SUSPENDED)
        {
            pcb_insert_queue(pcb_ptr, &PCB_READY_PTR);
        }
        else
        {
            pcb_insert_queue_prioritized(pcb_ptr, &PCB_READY_PTR);
        }
    }
    else if (pcb_ptr->state == PCB_BLOCKED)
    {
        pcb_insert_queue(pcb_ptr, &PCB_BLOCKED_PTR);
    }
}

void pcb_insert_queue_prioritized(pcb_t* pcb_ptr, pcb_t** pcb_queue_ptr)
{
    if (*pcb_queue_ptr == NULL)
    {
        *pcb_queue_ptr = pcb_ptr;
        pcb_ptr->prev_ptr = NULL;
        pcb_ptr->next_ptr = NULL;
        return;
    }
    if (pcb_ptr->priority < (*pcb_queue_ptr)->priority || ((*pcb_queue_ptr)->suspended == PCB_SUSPENDED && pcb_ptr->suspended != PCB_SUSPENDED))
    {
        pcb_ptr->prev_ptr = NULL;
        pcb_ptr->next_ptr = *pcb_queue_ptr;
        (*pcb_queue_ptr)->prev_ptr = pcb_ptr;
        *pcb_queue_ptr = pcb_ptr;
        return;
    }
    pcb_t* pcb_a_ptr = *pcb_queue_ptr;
    for (; pcb_a_ptr->next_ptr != NULL; pcb_a_ptr = pcb_a_ptr->next_ptr)
    {
        if (pcb_ptr->priority < pcb_a_ptr->next_ptr->priority || (pcb_a_ptr->next_ptr->suspended == PCB_SUSPENDED && pcb_ptr->suspended != PCB_SUSPENDED))
        {
            pcb_insert_spot(pcb_ptr, pcb_a_ptr);
            return;
        }
    }
    pcb_insert_spot(pcb_ptr, pcb_a_ptr);
}

void pcb_insert_queue(pcb_t* pcb_ptr, pcb_t** pcb_queue_ptr)
{
    if (*pcb_queue_ptr == NULL)
    {
        *pcb_queue_ptr = pcb_ptr;
        pcb_ptr->prev_ptr = NULL;
        pcb_ptr->next_ptr = NULL;
        return;
    }
    pcb_t* pcb_a_ptr = *pcb_queue_ptr;
    for (; pcb_a_ptr->next_ptr != NULL; pcb_a_ptr = pcb_a_ptr->next_ptr);
    pcb_insert_spot(pcb_ptr, pcb_a_ptr);  
}

void pcb_insert_spot(pcb_t* pcb_ptr, pcb_t* pcb_prev_ptr)
{
    pcb_ptr->next_ptr = pcb_prev_ptr->next_ptr;
    pcb_ptr->prev_ptr = pcb_prev_ptr;
    pcb_prev_ptr->next_ptr = pcb_ptr;
    if (pcb_ptr->next_ptr != NULL) 
    {
        pcb_ptr->next_ptr->prev_ptr = pcb_ptr;
    }
}

int pcb_remove(pcb_t* pcb_ptr)
{
    for (unsigned int i = 0; i < 2; i++) // doesnt do to running pointer because causes issues
    { 
        if (pcb_ptr == pcb_ptrs[i])
        {
            pcb_ptrs[i] = pcb_ptr->next_ptr;
            continue;
        }
    }
    if (pcb_ptr->prev_ptr != NULL)
    {
        pcb_ptr->prev_ptr->next_ptr = pcb_ptr->next_ptr;
    }
    if (pcb_ptr->next_ptr != NULL)
    {
        pcb_ptr->next_ptr->prev_ptr = pcb_ptr->prev_ptr;
    }
    pcb_ptr->prev_ptr = NULL;
    pcb_ptr->next_ptr = NULL;
    return 0;
}

pcb_t* pcb_find_addr(void* addr)
{
    for (unsigned int i = 0; i < sizeof(pcb_ptrs) / sizeof(pcb_t*); i++)
    {  
        if (pcb_ptrs[i] != NULL)
        {
            for (pcb_t* pcb_a_ptr = pcb_ptrs[i]; pcb_a_ptr != NULL; pcb_a_ptr = pcb_a_ptr->next_ptr)
            {
                if (addr == pcb_a_ptr)
                {
                    return pcb_a_ptr;
                }
            }
        }
    }
    return NULL;
}

// EOF
