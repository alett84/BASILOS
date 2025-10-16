#include <mpx/kernel_pcb.h>
#include <mpx/user_pcb.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys_req.h>
#include <mpx/mpxio.h>

//Contains user-oriented interactions with PCBs

/*
Calls pcb setup() to create a PCB, then inserts it into the appropriate queue with pcb insert()
*/
void create_pcb(const char* name, int class_type, int priority) 
{
    pcb_t* pcb_ptr = pcb_setup(name, class_type, priority);
        if (pcb_ptr != NULL) 
        {
            pcb_insert(pcb_ptr);
        }
}

/*
Finds the requested process, removes it from its queue with pcb_remove() and frees all associated memory with pcb_free()
*/
void delete_pcb(const char* name) 
{
    pcb_t* pcb_ptr = pcb_find(name);
    if (pcb_ptr != NULL) 
    {
        pcb_remove(pcb_ptr);
        pcb_free(pcb_ptr);
    }
}

//REMOVED DUE TO R6
/*
Puts a process in the blocked state, and moves it the appropriate queue

void block_pcb(const char* name) 
{
    pcb_t* pcb_ptr = pcb_find(name);
    if (pcb_ptr != NULL) 
    {
        pcb_remove(pcb_ptr);
        pcb_ptr->state = PCB_BLOCKED;
        pcb_insert(pcb_ptr);
    }
}


Puts a process in the unblocked (ready) state, and moves it the appropriate queue

void unblock_pcb(const char* name) 
{
    pcb_t* pcb_ptr = pcb_find(name);
    if (pcb_ptr != NULL) 
    {
        pcb_remove(pcb_ptr);
        pcb_ptr->state = PCB_READY;
        pcb_insert(pcb_ptr);
    }
}
*/
/*
Puts a process in the suspended state, and moves it the appropriate queue
*/
void suspend_pcb(const char* name)
{
    pcb_t* pcb_ptr = pcb_find(name);
    if (pcb_ptr != NULL) 
    {
        pcb_remove(pcb_ptr);
        pcb_ptr->suspended = PCB_SUSPENDED;
        pcb_insert(pcb_ptr);
    }
}

/*
Puts a process in the unsuspended state, and moves it the appropriate queue
*/
void resume_pcb(const char* name)
{
    pcb_t* pcb_ptr = pcb_find(name);
    if (pcb_ptr != NULL) 
    {
        pcb_remove(pcb_ptr);
        pcb_ptr->suspended = PCB_NOT_SUSPENDED;
        pcb_insert(pcb_ptr);
    }
}

/*
Changes a processs priority, and moves it to the appropriate place in the
appropriate queue
*/
void set_pcb_priority(const char* name, int priority)
{
    pcb_t* pcb_ptr = pcb_find(name);
    if (pcb_ptr != NULL) 
    {
        pcb_remove(pcb_ptr);
        pcb_ptr->priority = priority;
        pcb_insert(pcb_ptr);
    }
}

void show_pcb(const char* name)
{
    pcb_t* pcb_ptr = pcb_find(name);
    if (pcb_ptr != NULL) 
    {
        if (pcb_ptr->state == PCB_BLOCKED)
        {
            sys_req(WRITE, COM1, TEXT_GREEN "BLOCKED:" TEXT_CLEAR, 17);
        }
        else if (pcb_ptr->state == PCB_READY)
        {
            sys_req(WRITE, COM1, TEXT_GREEN "READY  :" TEXT_CLEAR, 17);  
        }
        else // is running
        {
            sys_req(WRITE, COM1, TEXT_GREEN "RUNNING:" TEXT_CLEAR, 17);  
        }
        show_pcb_t(pcb_ptr);
    }
}

/*
Displays the processes; name, class, state, suspended status, priority
*/
void show_pcb_t(pcb_t* pcb_ptr)
{
    //Prints the name
    sys_req(WRITE, COM1, pcb_ptr->name, strlen(pcb_ptr->name));
    if (strlen(pcb_ptr->name) == 8)
    {
        sys_req(WRITE, COM1, "\t", 2);
    }
    else
    {
        sys_req(WRITE, COM1, "\t\t", 2);
    }

    //Converts the class type value to the correct printable form, then writes it
    const char* class_type_string;
        if (pcb_ptr->class_type == PCB_SYSTEM) {
            class_type_string = "SYSTEM";
        } else {
            class_type_string = "USER";
        }
    sys_req(WRITE, COM1, class_type_string, strlen(class_type_string));
    sys_req(WRITE, COM1, "\t", 1);

    //Converts the state value to the correct printable form, then writes it
    const char* state_string;
        if (pcb_ptr->state == PCB_READY) {
            state_string = TEXT_YELLOW "READY" TEXT_CLEAR;
        } else if (pcb_ptr->state == PCB_BLOCKED) {
            state_string = TEXT_RED "BLOCKED" TEXT_CLEAR;
        } else {
            state_string = TEXT_GREEN "RUNNING" TEXT_CLEAR;
        }
    sys_req(WRITE, COM1, state_string, strlen(state_string));
    sys_req(WRITE, COM1, "\t", 1);

    //Converts the suspended value to the correct printable form, then writes it
    const char* suspended_string;
        if (pcb_ptr->suspended == PCB_SUSPENDED) {
            suspended_string = TEXT_RED "SUSPENDED" TEXT_CLEAR;
        } else {
            suspended_string = "NOT SUSPENDED";
        }

    sys_req(WRITE, COM1, suspended_string, strlen(suspended_string));
    sys_req(WRITE, COM1, "\t", 1);

    //Prints the priority
    char prioritybuffer[2];
    itoa(pcb_ptr->priority, prioritybuffer, 1, 10);
    sys_req(WRITE, COM1, prioritybuffer, strlen(prioritybuffer));

    sys_req(WRITE, COM1, "\n", 1);
}

/*
For all processes in queue: displays the processes; name, class, state, suspended status, priority
*/
void show_queue(pcb_t* queue_ptr, const char* row_name) 
{
    pcb_t* current_process = queue_ptr;
    int tab_this = 0;

    printf(TEXT_GREEN "%s" TEXT_CLEAR, row_name);
    if (current_process == NULL)
    {
        sys_req(WRITE, COM1, TEXT_GREY "None!\n" TEXT_CLEAR, 22);
        return;
    }
    while (current_process != NULL) 
    {
        if (tab_this)
        {
            sys_req(WRITE, COM1, "\t", 1);
        }
        else
        {
            tab_this = 1;
        }
        show_pcb_t(current_process);
        current_process = current_process->next_ptr;
    }
}

void show_ready(void)
{
    show_queue(PCB_READY_PTR,   "READY  :");
}
void show_blocked(void)
{
    show_queue(PCB_BLOCKED_PTR, "BLOCKED:");
}
void show_running(void)
{
    show_queue(PCB_RUNNING_PTR, "RUNNING:");
}

/*
For all processes in any state: displays the processes; name, class, state, suspended status, priority
*/
void show_all(void)
{
    show_running();
    show_ready();
    show_blocked();
}
