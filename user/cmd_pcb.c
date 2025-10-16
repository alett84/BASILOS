#include <mpx/cmd_pcb.h>

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <mpx/mpxio.h>
#include <sys_req.h>
#include <mpx/user_pcb.h>
#include <processes.h>
#include <mpx/isr.h>
#include <memory.h>

#define NUM_R3_PROCESSES 5

const char* PCB_TABLE_HEAD = TEXT_GREEN "\tNAME\t\tCLASS\tSTATE\tSUSPENDED_STATE\tPRIORITY\n"
                             TEXT_DARKGREEN "\t=================================================================\n" TEXT_CLEAR; 

void cmd_pcb(char** toks, int token_count)
{
    if (toks[1] == NULL) 
    {
        print(TEXT_RED "pcb command requires second argument.\nUse \"help pcb\" to see proper usage of pcb.\n" TEXT_CLEAR);
        return;
    } 

    strtolower(toks[1]); 
    if (strcmp(toks[1], "show") == 0)
    {
        cmd_pcb_show(toks, token_count);
    }
    else if (strcmp(toks[1], "delete") == 0)
    {
        cmd_pcb_delete(toks, token_count);
    }
    //REMOVED DUE TO R6
    /*else if (strcmp(toks[1], "block") == 0)
    {
        cmd_pcb_block(toks, token_count);
    }
    else if (strcmp(toks[1], "unblock") == 0)
    {
        cmd_pcb_unblock(toks, token_count);
    } */
    else if (strcmp(toks[1], "suspend") == 0)
    {
        cmd_pcb_suspend(toks, token_count);
    }
    else if (strcmp(toks[1], "resume") == 0)
    {
        cmd_pcb_resume(toks, token_count);
    }
    else if (strcmp(toks[1], "priority") == 0)
    {
        cmd_pcb_priority(toks, token_count);
    }
    else if (strcmp(toks[1], "showready") == 0)
    {
        cmd_pcb_showready();
    }
    else if (strcmp(toks[1], "showrunning") == 0)
    {
        cmd_pcb_showrunning();
    }
    else if (strcmp(toks[1], "showblocked") == 0)
    {
        cmd_pcb_showblocked();
    }
    else if (strcmp(toks[1], "showall") == 0)
    {
        cmd_pcb_showall();
    }
    else if (strcmp(toks[1], "loadr3") == 0)
    {
        cmd_pcb_loadr3(toks);
    }
    else
    {
        print(TEXT_RED "Command not recognized.\nUse \"help pcb\" to see proper usage of pcb.\n" TEXT_CLEAR);
    }
}
/*   REMOVED DUE TO R3
void cmd_pcb_create(char** toks, int token_count)
{
    // Expected input pcb create name type priority
    if (token_count < 5) 
    {
        print("Incorrect formatting.\nPlease type as:pcb create [name] [type] [priority].\n");
    } 
    else 
    {
        const char* name = toks[2];
        const char* type = toks[3];
        int priority = atoi(toks[4]);
        int type_int = -1;

        // Error checking & PCB validation
        // Flag to validate parameters 
        int valid = 1;

        // check name isnt too long
        if(strlen(name) > 8)
        {
            print("Invalid PCB Name: PCB name is too long. 8 characters max, please.\n");
            valid = 0;
        }

        // Check for unique and valid name
        if(pcb_find(name) != NULL)
        {
            print("Invalid PCB Name: Please enter a unique and valid PCB name.\n");
            valid = 0;  
        }

        //Check for valid class 
        if (strcmp(type, "user") == 0)
        {
            type_int = 1;
        }
        else if (strcmp(type, "system") == 0)
        {
            type_int = 0; 
        }
        else
        {
            print("Invalid PCB Class: Class type must be either \"user\" or \"system\".\n"); 
            valid = 0;
        }

        // Check for valid priority
        if (!isnumber(toks[4]) || priority < 0 || priority > 9)
        {
            print("Invalid PCB Priority: Priority must be an integer between 0-9.\n"); 
            valid = 0;
        }

        // If flag is true, PCB is valid and created 
        if(valid)
        {
            print("PCB created successfully.\n");
            create_pcb(name, type_int, priority);
        }                    
    }
}
*/
void cmd_pcb_show(char** toks, int token_count)
{
    // Expected input:pcb show name
    if ((token_count < 3) || (token_count > 3)) 
    {
        print("Incorrect formatting.\nPlease type as:pcb show [name].\n");
    }
    else
    {
        const char* name = toks[2];
        pcb_t* pcb_ptr = pcb_find(name);
        if (pcb_ptr == NULL)
        {
            print("PCB not found.\n");
        }
        else 
        {
        sys_req(LOCK, COM1);
        print(PCB_TABLE_HEAD);
        show_pcb(name);
        sys_req(UNLOCK, COM1);
        }
    }
}

void cmd_pcb_delete(char** toks, int token_count)
{
    // Expected input:pcb delete name
    if ((token_count < 3) || (token_count > 3))
    {
        print("Incorrect formatting.\nPlease type as:pcb delete [name].\n");
    }
    else 
    {
        const char* name = toks[2];
        pcb_t* pcb_ptr = pcb_find(name);

        if (pcb_ptr == NULL)
        {
            print("PCB not found.\n");
        }
        else if(pcb_ptr->class_type == PCB_SYSTEM)
        {
            print("This pcb is a system PCB; users cannot delete it.\n");
        }
        else
        {
            delete_pcb(name);
            print("PCB deleted successfully.\n");
        }
    }
}

//REMOVED DUE TO R6
/*
void cmd_pcb_block(char** toks, int token_count)
{
    // Expected input:pcb block name
    if ((token_count < 3) || (token_count > 3))
    {
        print("Incorrect formatting.\nPlease type as:pcb block [name].\n");
    }
    else 
    {
        const char* name = toks[2];
        pcb_t* pcb_ptr = pcb_find(name);

        if (pcb_ptr == NULL)
        {
            print("PCB not found.\n");
        }
        else if(pcb_ptr->class_type == PCB_SYSTEM)
        {
            print("This pcb is a system PCB; users cannot block it.\n");
        }
        else
        {
            block_pcb(name);
            print("PCB blocked successfully.\n");
        }
    }
}

void cmd_pcb_unblock(char** toks, int token_count)
{
    // Expected input:pcb unblock name
    if ((token_count < 3) || (token_count > 3))
    {
        print("Incorrect formatting.\nPlease type as:pcb unblock [name].\n");
    }
    else 
    {
        const char* name = toks[2];
        pcb_t* pcb_ptr = pcb_find(name);

        if (pcb_ptr == NULL)
        {
            print("PCB not found.\n");
        }
        else if(pcb_ptr->class_type == PCB_SYSTEM)
        {
            print("This pcb is a system PCB; users cannot unblock it.\n");
        }
        else
        {
            unblock_pcb(name);
            print("PCB unblocked successfully.\n");
        }
    }
}
*/

void cmd_pcb_suspend(char** toks, int token_count)
{
    // Expected input:pcb suspend name
    if ((token_count < 3) || (token_count > 3))
    {
        print("Incorrect formatting.\nPlease type as:pcb suspend [name].\n");
    }
    else 
    {
        const char* name = toks[2];
        pcb_t* pcb_ptr = pcb_find(name);

        if (pcb_ptr == NULL)
        {
            print("PCB not found.\n");
        }
        else if(pcb_ptr->class_type == PCB_USER)
        {
            suspend_pcb(name);
            print("PCB suspended successfully.\n");
        }
        else
        {
            print("This pcb is a system PCB; users cannot suspend it.\n");
        }
    }
}

void cmd_pcb_resume(char** toks, int token_count)
{
    // Expected input:pcb resume name
    if ((token_count < 3) || (token_count > 3))
    {
        print("Incorrect formatting.\nPlease type as:pcb resume [name].\n");
    }
    else 
    {
        const char* name = toks[2];
        pcb_t* pcb_ptr = pcb_find(name);

        if (pcb_ptr == NULL)
        {
            print("PCB not found.\n");
        }
        else if(pcb_ptr->class_type == PCB_SYSTEM)
        {
            print("This pcb is a system PCB; users cannot resume it.\n");
        }
        else
        {
            resume_pcb(name);
            print("PCB resumed successfully.\n");
        }
    }
}

void cmd_pcb_priority(char** toks, int token_count)
{
    // Expected input:pcb priority name new priority
    if ((token_count < 4) || (token_count > 4))
    {
        print("Incorrect formatting.\nPlease type as:pcb priority [name] [new priority].\n");
    }
    else 
    {
        const char* name = toks[2];
        int priority = atoi(toks[3]);
        pcb_t* pcb_ptr = pcb_find(name);

        if (pcb_ptr == NULL)
        {
            print("PCB not found.\n");
        }
        else if(pcb_ptr->class_type == PCB_SYSTEM)
        {
            print("This pcb is a system PCB; users cannot modify its priority.\n");
        }
        else if (isnumber(toks[3]) && priority >=0 && priority <=9)
        {
            set_pcb_priority(name, priority);
            print("PCB priority changed successfully.\n");
        }
        else
        {
            print("PCB priority value is integer restricted to valued 0-9.\n");
        }
    }
}

void cmd_pcb_showready()
{
    sys_req(LOCK, COM1);
    print(PCB_TABLE_HEAD);
    show_ready();
    sys_req(UNLOCK, COM1);
}

void cmd_pcb_showrunning()
{
    sys_req(LOCK, COM1);
    print(PCB_TABLE_HEAD);
    show_running();
    sys_req(UNLOCK, COM1);
}

void cmd_pcb_showblocked()
{
    sys_req(LOCK, COM1);
    print(PCB_TABLE_HEAD);
    show_blocked();
    sys_req(UNLOCK, COM1);
}

void cmd_pcb_showall()
{
    sys_req(LOCK, COM1);
    print(PCB_TABLE_HEAD);
    show_all();
    sys_req(UNLOCK, COM1);
}

/*
void cmd_pcb_yield()
{
    sys_req(IDLE); 
    print(TEXT_GREEN "~~ All processes have finished executing, returning control to comhand ~~\n" TEXT_CLEAR);
}
*/

void cmd_pcb_loadr3(char** toks)
{
    unsigned int sus = PCB_NOT_SUSPENDED;
    if (toks[2] != NULL && tolower(toks[2][0]) == 's')
    {
        sus = PCB_SUSPENDED;
    }

    void (*r3_functions[NUM_R3_PROCESSES])(void) = {proc1, proc2, proc3, proc4, proc5};

    pcb_t* new_pcb_ptrs[5] = {NULL};

    const char* process_names[NUM_R3_PROCESSES] = {"proc1", "proc2", "proc3", "proc4", "proc5"};
    
    int process_count = 0;
    for (int i = 0; i < NUM_R3_PROCESSES; i++)
    {
        if (pcb_find(process_names[i]) != NULL)
        {
            printf(TEXT_RED "Couldn't create \"%s\" because a PCB already exists with that name\n" TEXT_CLEAR, process_names[i]);
            continue;
        }
        process_count++;

        //allocate and initialize the new PCB 
        new_pcb_ptrs[i] = pcb_allocate();

        //give new process name and priority 
        strcpy(new_pcb_ptrs[i]->name, process_names[i]);
        new_pcb_ptrs[i]->priority = 5; //assuming it gets middlest
        new_pcb_ptrs[i]->state = PCB_READY; //ready states
        new_pcb_ptrs[i]->suspended = sus; 
        new_pcb_ptrs[i]->class_type = PCB_USER;

        //CLEAR STACK 
        memset(new_pcb_ptrs[i]->stack, 0, STACK_SIZE); 

        //Record this top-of-stack pointer in the pcb so the dispatcher can load it when the process is scheduled
        new_pcb_ptrs[i]->stack_ptr = new_pcb_ptrs[i]->stack_ptr - sizeof(context_t);

        //context saved at the top of the new stack 
        context_t* ctx = (context_t * )new_pcb_ptrs[i]->stack_ptr;

        //initialize registers 
        
        ctx->ds = 0x10;
        ctx->es = 0x10;
        ctx->fs = 0x10;
        ctx->gs = 0x10;
        ctx->ss = 0x10;

        ctx->eax = 0;
        ctx->ebx = 0;
        ctx->ecx = 0;
        ctx->edx = 0;
        ctx->esi = 0;
        ctx->edi = 0;

        //ebp = bottom of the allocated stack 
        ctx->ebp = (unsigned int)(ctx + sizeof(context_t));

        //EIP points to the start function
        ctx->eip = (unsigned int)r3_functions[i]; 
        ctx->cs  = 0x08;
        ctx->eflags = 0x0202;  // IF=1, bit 1 set => 0x0202 in hex

    }
    if (process_count > 0)
    {
        if (sus == PCB_NOT_SUSPENDED) 
        {
            printf("%1d out of 5 processes loaded\n", process_count);
        }
        else 
        {
            printf("%1d out of 5 processes loaded in suspended state\n", process_count);
        }
    }
    else 
    {
        print(TEXT_RED "No processes loaded due above to conflicts\n" TEXT_CLEAR);
    }
    for (int i = 0; i < 5; i++)
    {
        if (new_pcb_ptrs[i] != NULL)
        {
            pcb_insert(new_pcb_ptrs[i]);
        }
    }
}
// line at end of file
