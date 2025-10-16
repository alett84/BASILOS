#ifndef MPX_DCB_H
#define MPX_DCB_H

#include <stddef.h>
#include <mpx/kernel_pcb.h>
#include <mpx/device.h>

#define DCB_OPEN 1u
#define DCB_CLOSED 0u
#define DCB_IDLE 0u
#define DCB_READING 2u
#define DCB_WRITING 3u

#define DCB_LOCKING 0u
#define DCB_UNLOCKING 1u

#define RING_BUFFER_SIZE 100

// MANUAL THIS R6

#define dcb_is_open_to_iocb(dcb_ptr, iocb_ptr) ((dcb_ptr)->locking_pcb_ptr == NULL || iocb_ptr->pcb_ptr == (dcb_ptr)->locking_pcb_ptr)

// out of data if ring in and ring out at same place (buffer has 0 chars)
#define dcb_ring_has_data(dcb_ptr) (dcb_ptr->ring_out != dcb_ptr->ring_in)

// out of room if ring in immidiately behind ring out (buffer has RING_BUFFER_SIZE - 1 chars) 
// Note ring buffer can only actually hold RING_BUFFER_SIZE - 1 chars cause 
// an entirely full buffer and entirely empty buffer are indistinguishable
#define dcb_ring_has_room(dcb_ptr) (dcb_ptr->ring_out != (dcb_ptr->ring_in + 1) % RING_BUFFER_SIZE)

struct iocb;

typedef struct {
    device dev;
    unsigned int open : 1;
    unsigned int event_flag : 1;
    unsigned int status : 2;

    // intermediate ring buffer 
    char ring_buffer[RING_BUFFER_SIZE];
    unsigned int ring_in : 8;
    unsigned int ring_out : 8;

    struct iocb* iocb_ptr;
    pcb_t* locking_pcb_ptr;
    int locking_level;

} dcb_t;

struct iocb {
    pcb_t* pcb_ptr;
    dcb_t* dcb_ptr;
    unsigned int operation : 2;
    unsigned int event_flag : 1;

    char* buffer;
    size_t buffer_size;
    size_t buffer_pos;

    struct iocb* next_ptr;
    struct iocb* prev_ptr;
};
typedef struct iocb iocb_t;

/**
 Pops a character from a DCB's ring buffer
 @param dcb_ptr pointer to DCB 
 @return value popped from ring buffer. There is no error check to make sure you can pop, so use dcb_ring_has_data() before popping!
 */
char dcb_ring_pop(dcb_t* dcb_ptr);

/**
 Pushes a character into a DCB's ring buffer
 @param dcb_ptr pointer to DCB 
 @param val value to push 
 @return -1 if ring buffer out of room, 0 otherwise
 */
int dcb_ring_push(dcb_t* dcb_ptr, char val);

/**
 Enqueues an io request to a device, in the form of an iocb onto a dcb
 @param dcb_ptr dcb to enqueue iocb on
 @param iocb_ptr iocb to have enqueued
 */
void dcb_enqueue(dcb_t* dcb_ptr, iocb_t* iocb_ptr);

/**
 Slight misnomer. Deletes (does not return) front of iocb queue, returns calling pcb to ready queue, then sets proper state for dcb  
 @param dcb_ptr dcb to "dequeue" iocb from
 */
void dcb_dequeue(dcb_t* dcb_ptr);

int dcb_lock(device dev);

int dcb_unlock(device dev);

#endif
