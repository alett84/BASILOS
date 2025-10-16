#include <mcb.h>
#include <mpx/cmd_mcb.h>
#include <mpx/vm.h>
#include <mpx/mpxio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys_req.h>
#include <mpx/kernel_pcb.h>

#include <stdint.h>

#if UINTPTR_MAX == 0xffffffff
    #define MY_PRIxPTR "x"
#elif UINTPTR_MAX == 0xffffffffffffffff
    #define MY_PRIxPTR "llx"
#else
    #error "Unsupported uintptr_t size"
#endif

const char* MCB_TABLE_HEAD = TEXT_GREEN "\tBLOCK NUMBER\tSTART ADDRESS\tBLOCK SIZE\tALLOCATED\n"
                             TEXT_DARKGREEN "\t=================================================================\n" TEXT_CLEAR; 

void cmd_mcb (char** toks, int token_count)
{
    if(token_count < 0 || toks[1] == NULL)
    {
        print(TEXT_RED "Memory function requires a second argument.\n" TEXT_CLEAR);
        return;
    }

    strtolower(toks[1]);
    /* if (strcmp(toks[1], "free") == 0)
    {
        cmd_mcb_free(toks, token_count);
    }
    else if(strcmp(toks[1], "allocate") == 0)
    {
        cmd_mcb_allocate(toks, token_count);
    } */
    if(strcmp(toks[1], "showallocated") == 0)
    {
        cmd_mcb_show(MCB_ALLOCATED);
    }
    else if(strcmp(toks[1], "showfree") == 0)
    {
        cmd_mcb_show(MCB_FREE);
    }
    else if(strcmp(toks[1], "showall") == 0)
    {
        cmd_mcb_show(-1);
    }
    else
    {
        print(TEXT_RED "Invalid second argument.\nSee \"help mcb\" for valid second arguments.\n" TEXT_CLEAR);
        return; 
    }
}


void cmd_mcb_free(char** toks, int token_count)
{
    //Checks that there are enough args and checks that args are not empty
    if(token_count < 3 || toks[2] == NULL)
    {
        print(TEXT_RED "Free command requires a memory address as the third argument.\n" TEXT_CLEAR);
        return;
    }

    //Convert the hex string to a pointer 

    //hex_to_uintptr() converts the string to an unsigned long long 
    //then cast it to uintptr_t to let it hold a pointer value
    uintptr_t addr = hex_to_uintptr(toks[2]);

    // check that value isn't longer than an uint otherwise printf will crash
    if ((unsigned long long)addr >= 0xFFFFFFF)
    {
        print(TEXT_RED "Free command requires a memory address as the third argument.\n" TEXT_CLEAR);
        return;
    }

    void *mem_ptr = (void*)addr; 

    // check not trying to free pcb
    if (pcb_find_addr(mem_ptr) != NULL)
    {
        printf(TEXT_RED "Failed to free memory at address 0x%" MY_PRIxPTR ": MCB allocated to PCB\n" TEXT_CLEAR, addr);
        return;
    }

    //call free_memory with the pointer 
    int result = free_memory(mem_ptr);
    //result = 0 is good 
    //result != 0 is bad

    if(result == -1)
    {
        printf(TEXT_RED "Failed to free memory at address 0x%" MY_PRIxPTR ": Not an MCB\n" TEXT_CLEAR, addr);
    }
    else if (result == -2)
    {
        printf(TEXT_RED "Failed to free memory at address 0x%" MY_PRIxPTR ": MCB not allocated\n" TEXT_CLEAR, addr);
    }
    else
    {
        printf("Successfully freed memory at address 0x%" MY_PRIxPTR "\n", addr);
    }

    return; 
}



void cmd_mcb_allocate(char** toks, int token_count)
{
    //check the number of tokens and validate that toks[2] is not null
    if (token_count < 3 || toks[2] == NULL || !isdigit(toks[2][0]))
    {
        print(TEXT_RED "Memory allocate requires the size of allocation request as the third argument\n" TEXT_CLEAR);
        return; 
    }


    size_t alloc_size = (size_t)atoi(toks[2]);
    

    void *mem_ptr = allocate_memory(alloc_size);

    if(mem_ptr == NULL)
    {
        printf(TEXT_RED "Failed to allocate memory of size %d: insufficient memory\n" TEXT_CLEAR, alloc_size); 
    } 
    else
    {
        printf("Allocated memory at address 0x%x\n", mem_ptr); 
    }
    return;
}


void cmd_mcb_show(int allocated)
{
    sys_req(LOCK, COM1);
    // int to keep track of number of blocks
    int num_blocks = 0; 

    // get heap pointer
    mcb_t* current_mcb_ptr = heap_ptr; 

    print(MCB_TABLE_HEAD);

    // iterate through mcb list 
    while(current_mcb_ptr != NULL)
    {
        // if mcb is an allocated block
        if(allocated == -1 || allocated == current_mcb_ptr->allocated)
        {
            // print block number
            printf("\t%d\t\t", num_blocks);     

            // print start address of block (not MCB address) in hex
            printf("0x%x\t", current_mcb_ptr->start_ptr); 

            // print size of block (in decimal)
            printf("%d\t\t", current_mcb_ptr->size); 

            if (current_mcb_ptr->allocated) 
            {
                print(TEXT_YELLOW "ALLOCATED" TEXT_CLEAR);
            }
            else 
            {
                print(TEXT_GREEN "FREE" TEXT_CLEAR);
            }

            // print newline to separate blocks
            printf("\n"); 
        }

        // increase num_blocks
        num_blocks++; 

        // Go to next mcb
        current_mcb_ptr = current_mcb_ptr->next_ptr; 
    }
    sys_req(UNLOCK, COM1);
    return;
}

// eof
