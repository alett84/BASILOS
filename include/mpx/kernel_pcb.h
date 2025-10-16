#ifndef MPX_KERNEL_PCB
#define MPX_KERNEL_PCB

/**
 @file mpx/kernel_pcb.h
 @brief Kernel functions for manipulating system PCBs
 */

#define STACK_SIZE 1024

#define PCB_READY 0u
#define PCB_RUNNING 1u
#define PCB_BLOCKED 2u

#define PCB_SYSTEM 0u
#define PCB_USER 1u

#define PCB_NOT_SUSPENDED 0u
#define PCB_SUSPENDED 1u

#define PCB_READY_PTR pcb_ptrs[0]
#define PCB_BLOCKED_PTR pcb_ptrs[1]
#define PCB_RUNNING_PTR pcb_ptrs[2]

struct pcb {
    char name[9];
    unsigned int class_type : 1;
    unsigned int suspended : 1;
    unsigned int state : 2; // ready, running, blocked
    unsigned int priority : 4; // 0-9
    int eax_override;
    struct pcb *next_ptr;
    struct pcb *prev_ptr;
    unsigned char *stack_ptr;
    unsigned char stack[STACK_SIZE];
};
typedef struct pcb pcb_t;

extern pcb_t* pcb_ptrs[3];

extern pcb_t* idle_pcb; 

/**
 Allocates memory for a new pcb
 @return the newly allocated pcb
 */
pcb_t* pcb_allocate(void);

/**
 frees the memory associated with a pcb
 @param pcb_ptr pcb to free
 @return 0 on success, non-zero on error
 */
int pcb_free(pcb_t* pcb_ptr);

/**
 Initializes a pcb
 @param name name of the pcb
 @param class_type type of the pcb (user or system)
 @param priority priority of the pcb (0, most priority, to 9, least priority)
 @return the initialized pcb
 */
pcb_t* pcb_setup(const char *name, int class_type, int priority); 

/**
 Finds a pcb struct of a given name
 @param name name of the pcb
 @return pcb with the given name, or NULL if no pcb found
 */
pcb_t* pcb_find(const char *name);

/**
 Inserts a pcb into the appropriate queue given its state (ready or blocked)
 @param pcb_ptr pcb to insert
 */
void pcb_insert(pcb_t* pcb_ptr); 

/**
 Inserts a pcb into a given priority queue at the appropriate position
 @param pcb_ptr pcb to insert
 @param pcb_queue_ptr pointer to the front pcb of the queue
 */
void pcb_insert_queue_prioritized(pcb_t* pcb_ptr, pcb_t** pcb_queue_ptr);

/**
 Inserts a pcb into a given queue at the back of it
 @param pcb_ptr pcb to insert
 @param pcb_queue_ptr pointer to the front pcb of the queue
 */
void pcb_insert_queue(pcb_t* pcb_ptr, pcb_t** pcb_queue_ptr);

/**
 Inserts a pcb after a given pcb that is a member of a queue
 @param pcb_ptr pcb to insert
 @param pcb_ptr pcb that the given pcb is inserted after
 */
void pcb_insert_spot(pcb_t* pcb_ptr, pcb_t* pcb_prev_ptr);

/**
 Removes a pcb from the appropriate queue
 @param pcb_ptr pcb to remove
 @return 0 on success, non-zero on error
 */
int pcb_remove(pcb_t* pcb_ptr); 

/**
 Confirms that a memory pointer is for a pcb
 @param pcb_ptr pointer to potential pcb
 @return (pcb_t*)addr if pcb, NULL otherwise
 */
pcb_t* pcb_find_addr(void* addr);

#endif
