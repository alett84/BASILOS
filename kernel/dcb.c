#include <mpx/dcb.h>

#include <memory.h>
#include <stddef.h>
#include <mpx/isr.h>
#include <mpx/serial.h>

char dcb_ring_pop(dcb_t* dcb_ptr)
{
    if (!dcb_ring_has_data(dcb_ptr)) // popped while empty
    {
        // the value popped is a garbage old character
        // ring in shifted forward just to ensure filled amount remains 0
        dcb_ptr->ring_in = (dcb_ptr->ring_in + 1) % RING_BUFFER_SIZE;
    }
    char val = dcb_ptr->ring_buffer[dcb_ptr->ring_out];
    dcb_ptr->ring_out = (dcb_ptr->ring_out + 1) % RING_BUFFER_SIZE;
    return val;
}

int dcb_ring_push(dcb_t* dcb_ptr, char val)
{
    if (!dcb_ring_has_room(dcb_ptr)) // pushed while full
    {
        // according to design specifications the new char is discarded
        return -1;
    }
    dcb_ptr->ring_buffer[dcb_ptr->ring_in] = val;
    dcb_ptr->ring_in = (dcb_ptr->ring_in + 1) % RING_BUFFER_SIZE;
    return 0;
}

void dcb_enqueue(dcb_t* dcb_ptr, iocb_t* iocb_ptr)
{
    if (dcb_ptr->iocb_ptr == NULL || (dcb_ptr->locking_pcb_ptr != NULL && iocb_ptr->pcb_ptr == dcb_ptr->locking_pcb_ptr && dcb_ptr->iocb_ptr->pcb_ptr != dcb_ptr->locking_pcb_ptr))
    {
        iocb_ptr->prev_ptr = NULL;
        iocb_ptr->next_ptr = dcb_ptr->iocb_ptr;

        if (iocb_ptr->next_ptr != NULL)
        {
            iocb_ptr->next_ptr->prev_ptr = iocb_ptr;
        }    

        dcb_ptr->iocb_ptr = iocb_ptr;
        return;
    }
    iocb_t* iocb_tail_ptr = dcb_ptr->iocb_ptr;
    for (; !(iocb_tail_ptr->next_ptr == NULL || (dcb_ptr->locking_pcb_ptr != NULL && iocb_ptr->pcb_ptr == dcb_ptr->locking_pcb_ptr && iocb_tail_ptr->next_ptr->pcb_ptr != dcb_ptr->locking_pcb_ptr)); iocb_tail_ptr = iocb_tail_ptr->next_ptr);

    iocb_ptr->prev_ptr = iocb_tail_ptr;
    iocb_ptr->next_ptr = iocb_tail_ptr->next_ptr;

    if (iocb_ptr->next_ptr != NULL)
    {
        iocb_ptr->next_ptr->prev_ptr = iocb_ptr;
    }

    iocb_tail_ptr->next_ptr = iocb_ptr;
    return;
}

void dcb_dequeue(dcb_t* dcb_ptr)
{
    iocb_t* iocb_ptr = dcb_ptr->iocb_ptr;
    if (iocb_ptr->operation == DCB_READING || iocb_ptr->operation == DCB_WRITING)
    {
        iocb_ptr->pcb_ptr->eax_override = iocb_ptr->buffer_pos;
    }

    if (iocb_ptr->pcb_ptr != PCB_RUNNING_PTR)
    {
        iocb_ptr->pcb_ptr->state = PCB_READY;
        pcb_remove(iocb_ptr->pcb_ptr);
        pcb_insert(iocb_ptr->pcb_ptr);
    } 
    else
    {
        iocb_ptr->pcb_ptr->state = PCB_RUNNING;
    }

    // perform dequeue operation
    dcb_ptr->iocb_ptr = iocb_ptr->next_ptr;
    if (dcb_ptr->iocb_ptr != NULL)
    {
        dcb_ptr->iocb_ptr->prev_ptr = NULL;
    }

    sys_free_mem(iocb_ptr);

    // update dcb's state
    dcb_ptr->status = DCB_IDLE;
    dcb_ptr->event_flag = 1;
    
}

int dcb_lock(device dev)
{
    /* 0) perform checks defined in detailed R6 documentation */
	int dno = serial_devno(dev);
	if (dno == -1)
	{
		return -501; // device not initialized
	}

	dcb_t* dcb_ptr = &dcb[dno];

	if (!dcb_ptr->open)
	{
		return -501; // port not open
	}

    iocb_t* iocb_ptr = dcb_ptr->iocb_ptr;

	if (dcb_ptr->status != DCB_IDLE || dcb_ptr->event_flag || !dcb_is_open_to_iocb(dcb_ptr, iocb_ptr))
	{
		return -504; // device busy
	}

    if (dcb_ptr->locking_pcb_ptr != NULL)
    {
        dcb_ptr->locking_level++;
    } 
    else 
    {
        dcb_ptr->locking_pcb_ptr = iocb_ptr->pcb_ptr;
    }
    dcb_dequeue(dcb_ptr);
    return 0;
}

int dcb_unlock(device dev)
{
    /* 0) perform checks defined in detailed R6 documentation */
	int dno = serial_devno(dev);
	if (dno == -1)
	{
		return -601; // device not initialized
	}

	dcb_t* dcb_ptr = &dcb[dno];

	if (!dcb_ptr->open)
	{
		return -601; // port not open
	}

	if (dcb_ptr->status != DCB_IDLE || dcb_ptr->event_flag)
	{
		return -604; // device busy
	}

    if (dcb_ptr->locking_level > 0)
    {
        dcb_ptr->locking_level--;
    } 
    else 
    {
        dcb_ptr->locking_pcb_ptr = NULL;
    }
    dcb_dequeue(dcb_ptr);
    return 0;
}

// eof
