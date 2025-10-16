#include <stdlib.h>
#include <string.h>
#include <mpx/isr.h>
#include <mpx/kernel_pcb.h>
#include <sys_req.h>
#include <mpx/dcb.h>
#include <mpx/serial.h>
#include <memory.h>

context_t * default_context_ptr = NULL;

char private_interrupt_stack[1024] = {0};

context_t *sys_call(context_t * current_context_ptr)
{
    int op = current_context_ptr->eax;
    if (op == READ || op == WRITE)
    {
        // get variables from context
        device dev = current_context_ptr->ebx;
        char* buffer = (char*)current_context_ptr->ecx;
        size_t len = current_context_ptr->edx;

        // create new iocb for thing
        iocb_t* iocb_ptr = sys_alloc_mem(sizeof(iocb_t));
        iocb_ptr->pcb_ptr = PCB_RUNNING_PTR;
        
        int dno = serial_devno(dev);
        iocb_ptr->dcb_ptr = &dcb[dno];

        iocb_ptr->operation = (unsigned int)op;
        iocb_ptr->event_flag = 0;

        iocb_ptr->buffer = buffer;
        iocb_ptr->buffer_size = len;
        iocb_ptr->buffer_pos = 0;

        dcb_enqueue(iocb_ptr->dcb_ptr, iocb_ptr);

        PCB_RUNNING_PTR->state = PCB_BLOCKED;

        if (op == READ)
        {
            serial_read(dev);
        }
        else // op is WRITE
        {
            serial_write(dev);
        } 
    }
    else if (op == LOCK || op == UNLOCK)
    {
        // get variables from context
        device dev = current_context_ptr->ebx;

        // create new iocb for thing
        iocb_t* iocb_ptr = sys_alloc_mem(sizeof(iocb_t));
        iocb_ptr->pcb_ptr = PCB_RUNNING_PTR;
        
        int dno = serial_devno(dev);
        iocb_ptr->dcb_ptr = &dcb[dno];

        if (op == LOCK)
        {
            iocb_ptr->operation = DCB_LOCKING;
        }
        else // op is UNLOCK
        {
            iocb_ptr->operation = DCB_UNLOCKING;
        }
        iocb_ptr->event_flag = 0;

        dcb_enqueue(iocb_ptr->dcb_ptr, iocb_ptr);

        PCB_RUNNING_PTR->state = PCB_BLOCKED;

        int ret = 0;
        if (op == LOCK)
        {
            ret = dcb_lock(dev);
        }
        else // op is UNLOCK
        {
            ret = dcb_unlock(dev);
        } 

        if (ret == 0)
        {
            // was immidiately able to service request to (un)lock, so shouldn't IDLE
            return current_context_ptr;
        }
    }

    for (unsigned int i = 0; i < sizeof(dcb) / sizeof(dcb_t); i++)
    {
        if (dcb[i].open != DCB_OPEN || dcb[i].event_flag != 1) {
            continue;
        }
        dcb[i].event_flag = 0;
        if (dcb[i].iocb_ptr != NULL && dcb_is_open_to_iocb(&dcb[i], dcb[i].iocb_ptr))
        {
            if (dcb[i].iocb_ptr->operation == DCB_READING)
            {
                serial_read(dcb[i].dev);
            }
            else if (dcb[i].iocb_ptr->operation == DCB_WRITING) 
            {
                serial_write(dcb[i].dev);
            }
            else if (dcb[i].iocb_ptr->operation == DCB_LOCKING) 
            {
                dcb_lock(dcb[i].dev);
            }
            else // is UNLOCKING
            {
                dcb_unlock(dcb[i].dev);
            }
        }
    }

    context_t * next_context_ptr = NULL;
    pcb_t * next_pcb_ptr = NULL;
    if (PCB_READY_PTR == NULL || PCB_READY_PTR->suspended == PCB_SUSPENDED)
    {
        // no more pcbs
        if (op == EXIT)
        {
            // time to end
            next_context_ptr = default_context_ptr;
            default_context_ptr = NULL;
            next_pcb_ptr = NULL;
        }
        else 
        {
            // keep going with the current pcb
            next_pcb_ptr = PCB_RUNNING_PTR;
            
            if (next_pcb_ptr->eax_override == 0)
            {
                current_context_ptr->eax = 0; // return a 0 because good
            } 
            else
            {
                current_context_ptr->eax = next_pcb_ptr->eax_override; // return n bytes read/wrote
                next_pcb_ptr->eax_override = 0;
            }
            return current_context_ptr;
        }
    }
    else 
    {
        // get our next pcb
        next_pcb_ptr = PCB_READY_PTR;
        pcb_remove(PCB_READY_PTR);
        next_pcb_ptr->state = PCB_RUNNING; 
        next_context_ptr = (context_t * )next_pcb_ptr->stack_ptr;
    }

    if (op != EXIT)
    {
        if (PCB_RUNNING_PTR == NULL) // are idling not from a process
        {
            default_context_ptr = current_context_ptr;
        }
        else
        {
            PCB_RUNNING_PTR->stack_ptr = (unsigned char * )current_context_ptr;
            if (PCB_RUNNING_PTR->state != PCB_BLOCKED) 
            {
                PCB_RUNNING_PTR->state = PCB_READY;
            }
            pcb_insert(PCB_RUNNING_PTR);
        }
    }
    else 
    {
        if (current_context_ptr != NULL) // make sure are exiting an actual process
        {
            pcb_free(PCB_RUNNING_PTR);
            PCB_RUNNING_PTR = NULL;
        }
    }

    PCB_RUNNING_PTR = next_pcb_ptr;
    if (next_pcb_ptr == NULL || next_pcb_ptr->eax_override == 0)
    {
        next_context_ptr->eax = 0; // return a 0 because good
    } 
    else
    {
        next_context_ptr->eax = next_pcb_ptr->eax_override; // return n bytes read/wrote
        next_pcb_ptr->eax_override = 0;
    }
   
    return next_context_ptr;
}

char* get_private_interrupt_stack(void)
{
    return private_interrupt_stack + sizeof(private_interrupt_stack) - 2;
}

// eof
