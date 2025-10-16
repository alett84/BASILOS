#ifndef MPX_ISR
#define MPX_ISR

#include <stdint.h>
#include <mpx/kernel_pcb.h>

struct context {
    // Segment registers
    int32_t ds;
    int32_t es;
    int32_t fs;
    int32_t gs;
    int32_t ss;
    // General purpose registers
    int32_t eax;
    int32_t ebx;
    int32_t ecx;
    int32_t edx;
    int32_t esi;
    int32_t edi;
    int32_t ebp;
    // registers pushed by CPU automatically
    int32_t eip;
    int32_t cs;
    int32_t eflags;
};
typedef struct context context_t;

extern context_t * default_context_ptr;

/**
 Performs a context switch. Must be called from sys_call_isr.s else it won't work.
 @param current_context_ptr context of last running process
 @return context of next process to run
 */
context_t *sys_call(context_t * current_context_ptr);

// MANUAL THIS R6
char* get_private_interrupt_stack(void);

#endif
