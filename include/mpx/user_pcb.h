#ifndef MPX_USER_PCB
#define MPX_USER_PCB

/**
 @file mpx/mpxio.h
 @brief Contains user-oriented interactions with PCBs
*/

#include <mpx/kernel_pcb.h>

/*
Calls pcb setup() to create a PCB, then inse
*/

/* REMOVED DUE TO R3
void create_pcb(const char* name, int class_type, int priority);
*/

/*
Finds the requested process, removes it from its queue with pcb_remove() and frees all associated memory with pcb_free()
*/
void delete_pcb(const char* name);

/*
Puts a process in the blocked state, and moves it the appropriate queue
*/
//REMOVED DUE TO R6
//void block_pcb(const char* name);

/*
Puts a process in the unblocked (ready) state, and moves it the appropriate queue
*/
//void unblock_pcb(const char* name);

/*
Puts a process in the suspended state, and moves it the appropriate queue
*/
void suspend_pcb(const char* name);

/*
Puts a process in the unsuspended state, and moves it the appropriate queue
*/
void resume_pcb(const char* name);

/*
Changes a processs priority, and moves it to the appropriate place in the
appropriate queue
*/
void set_pcb_priority(const char* name, int priority);

/*
Displays the processes; name, class, state, suspended status, priority from String
*/
void show_pcb(const char* name);

/*
Displays the processes; name, class, state, suspended status, priority from Struct
*/
void show_pcb_t(pcb_t* pcb_ptr);

void show_queue(pcb_t* queue_ptr, const char* row_name);

/*
For all processes in the ready state: displays the processes; name, class, state, suspended status, priority
*/
void show_ready(void);

/*
For all processes in the blocked state: displays the processes; name, class, state, suspended status, priority
*/
void show_blocked(void);

void show_running(void);

/*
For all processes in any state: displays the processes; name, class, state, suspended status, priority
*/
void show_all(void);

#endif
