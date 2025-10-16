#include <mpx/comhand.h>

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

#include <sys_req.h>

#include <mpx/device.h>
#include <mpx/mpxio.h>
#include <mpx/user_pcb.h>

#include <mpx/cmd_datetime.h>
#include <mpx/cmd_pcb.h>
#include <mpx/cmd_alarm.h>
#include <mpx/cmd_mcb.h>



#define VERSION "6.0"
#define TOKEN_SIZE 6

void comhand(void)
{
    // Print the ASCII art banner on startup
    const char *BANNER =
        TEXT_RED    "               ,---.        ,-,--.   .=-.-.              _,.---._      ,-,--.  \n"
        TEXT_ORANGE "    _..---.  .--.'  \\     ,-.'-  _\\ /==/_ / _.-.       ,-.' , -  `.  ,-.'-  _\\ \n"
        TEXT_YELLOW "  .' .'.-. \\ \\==\\-/\\ \\   /==/_ ,_.'|==|, |.-,.'|      /==/_,  ,  - \\/==/_ ,_.' \n"
        TEXT_GREEN  " /==/- '=' / /==/-|_\\ |  \\==\\  \\   |==|  |==|, |     |==|   .=.     \\==\\  \\    \n"
        TEXT_CYAN   " |==|-,   '  \\==\\,   - \\  \\==\\ -\\  |==|- |==|- |     |==|_ : ;=:  - |\\==\\ -\\   \n"
        TEXT_BLUE   " |==|  .=. \\ /==/ -   ,|  _\\==\\ ,\\ |==| ,|==|, |     |==| , '='     |_\\==\\ ,\\  \n"
        TEXT_MAGENTA" /==/- '=' ,/==/-  /\\ - \\/==/\\/ _ ||==|- |==|- `-._   \\==\\ -    ,_ //==/\\/ _ | \n"
        TEXT_RED    "|==|   -   /\\==\\ _.\\=\\.-'\\==\\ - , //==/. /==/ - , ,/   '.='. -   .' \\==\\ - , / \n"
        TEXT_ORANGE "`-._`.___,'  `--`         `--`---' `--`-``--`-----'      `--`--''    `--`---'  \n\n"
        TEXT_CLEAR;

    print(BANNER);
    print("Welcome, to Pidsoft's BasilOS! Type \"help\" for a list of all commands.\n");

    for (;;) 
    {
        sys_req(IDLE);

        char buf [100] = {0};
        char* toks [TOKEN_SIZE] = {NULL};
        int token_count = input(toks, TOKEN_SIZE, buf, 100);

        if (token_count <= 0) 
        {
            continue; // ignore reads with only whitespace
        }
        strtolower(toks[0]);

        // process the command 
        if (strcmp(toks[0], "shutdown") == 0) 
        {
            cmd_shutdown(toks, buf);
        } 
        else if (strcmp(toks[0], "version") == 0)
        {
            print("MPX Version " VERSION "\n");
        } 
        else if (strcmp(toks[0], "help") == 0)
        {
            cmd_help(toks);
        } 
        else if (strcmp(toks[0], "time") == 0) 
        {
            cmd_time(toks, token_count);
        } 
        else if (strcmp(toks[0], "date") == 0) 
        {
            cmd_date(toks, token_count);
        }
        else if (strcmp(toks[0], "pcb") == 0)
        {
            cmd_pcb(toks, token_count);
        } 
        else if (strcmp(toks[0], "clear") == 0)
        {
            print(CLEAR HOME);
        }
        //REMOVED DUE TO R4? R3?
        /*
        else if (strcmp(toks[0], "yield") == 0)
        {
            cmd_pcb_yield();
        }
        */
        else if (strcmp(toks[0], "alarm") == 0)
        {
            cmd_alarm(toks, token_count);
        }
        else if (strcmp(toks[0], "mcb") == 0)
        {
            cmd_mcb(toks, token_count);
        }
        else // invalid command
        {
            printf(TEXT_RED "\"%s\" is not a valid command.\ntype \"help\" for a list of valid commands.\n" TEXT_CLEAR, toks[0]);
        }
    }
}

int cmd_shutdown(char** toks, char* buf)
{
    // print message to confirm if user wants to shutdown
    print("Are you sure you want to shutdown? y/n\n"); 
            
    while(1)
    {
        // read user input into buffer 
        int token_count = input(toks, 6, buf, 100);
        if(token_count <= 0) 
        {
            print("Please enter y/n to confirm shutdown.\n"); 
            continue;
        }
        
        tolower_inplace(toks[0]);
        if(toks[0][0] == 'y') // user wants to shutdown, shutdown 
        {
            // delete everything in ready and blocked
            for (unsigned int i = 0; i < 2; i++) 
            { 
                while (pcb_ptrs[i]) 
                {
                    pcb_t* pcb_ptr = pcb_ptrs[i];
                    pcb_remove(pcb_ptr);
                    pcb_free(pcb_ptr);
                }
            }

            sys_req(EXIT);
        } 
        else if (toks[0][0] == 'n') // user does not want to shutdown, break while loop 
        {
            print("Shutdown sequence terminated.\n"); 
            break; 
        }
        else 
        {
            print("Please enter y/n to confirm shutdown.\n"); 
        }
    }
    return 0;
}

void cmd_help(char** toks)
{
    sys_req(LOCK, COM1);
    const char* HELP_SEPARATOR = TEXT_DARKCYAN "===========================< GIVE A HELPING HAND >==========================\n" TEXT_CLEAR;
    const char* HELP_CLOSER =    TEXT_DARKCYAN "============================================================================\n" TEXT_CLEAR;
    print(HELP_SEPARATOR);
    if (toks[1] == NULL)
    {
        const char* HELP = 
            "shutdown - shuts down BasilOS\n"
            "version  - prints the current version of BasilOS\n"
            "help     - displays this help screen\n"
            "time     - get/set the current system time\n"
            "date     - get/set the current system date\n"
            "pcb      - perform user interactions with PCBs (Process Control Blocks)\n"
            "clear    - clears the terminal\n"
            //"yield    - Yields comhand so queued processes can run\n"
            "alarm    - Sets a alarm to go off at a specified time with a message\n"
            "mcb      - Contains commands for interactions with MCBs (Memory Control Blocks)\n"
            "\n"
            "Type \"help " TEXT_YELLOW "[command name]" TEXT_CLEAR "\" for more help on any specific command.\n";
        print(HELP);
        print(HELP_CLOSER);
        sys_req(UNLOCK, COM1);
        return;
    }
    strtolower(toks[1]);

    if (strcmp(toks[1], "shutdown") == 0)
    {
        const char* HELP_SHUTDOWN = 
            "shutdown - shuts down BasilOS.\n"
            "Will prompt the user to type y/n to confirm shutdown.\n"
            "\n"
            "Shutdown takes no additional arguments.\n";
        print(HELP_SHUTDOWN);
    }
    else if (strcmp(toks[1], "version") == 0)
    {
        const char* HELP_VERSION = 
            "version - prints the current version of BasilOS.\n"
            "\n"
            "Version takes no additional arguments.\n";
        print(HELP_VERSION);
    }
    else if (strcmp(toks[1], "help") == 0)
    {
        const char* HELP_HELP = 
            "help - displays the help screen.\n"
            "\n"
            "Additional args:\n"
            "help " TEXT_YELLOW  "[command name]" TEXT_CLEAR " - displays help for a specific command\n";
        print(HELP_HELP);
    }
    else if (strcmp(toks[1], "time") == 0)
    {
        const char* HELP_TIME = 
            "time - gets/sets the system time.\n"
            "\n"
            "Additional args:\n"
            "time get            - print the system time to the screen\n"
            "time set " TEXT_YELLOW "[HH:MM:SS]" TEXT_CLEAR " - set system time to HH:MM:SS, military time\n";
        print(HELP_TIME);
    }
    else if (strcmp(toks[1], "date") == 0)
    {
        const char* HELP_DATE = 
            "date - gets/sets the system date.\n"
            "\n"
            "Additional args:\n"
            "date get            - print the system date to the screen\n"
            "date set " TEXT_YELLOW "[MM/DD/YY]" TEXT_CLEAR " - set system date to MM/DD/YY\n";
        print(HELP_DATE);
    }
    else if (strcmp(toks[1], "pcb") == 0)
    {
        const char* HELP_KERNEL = 
            "pcb - user-interactions with the PCBs (Process Control Blocks).\n"
            "\n"
            "Additional args:\n"
            //"pcb create     " TEXT_YELLOW "[name] [type] [priority (0 -9)]" TEXT_CLEAR " - create a pcb\n"
            "pcb delete   " TEXT_YELLOW "[name*]             " TEXT_CLEAR " - delete a pcb\n"
            //"pcb block    " TEXT_YELLOW "[name*]             " TEXT_CLEAR " - block a pcb\n"
            //"pcb unblock  " TEXT_YELLOW "[name*]             " TEXT_CLEAR " - unblock a pcb\n"
            "pcb suspend  " TEXT_YELLOW "[name*]             " TEXT_CLEAR " - suspends a pcb\n"
            "pcb resume   " TEXT_YELLOW "[name*]             " TEXT_CLEAR " - unsuspends a pcb\n"
            "pcb priority " TEXT_YELLOW "[name*] [priority**]" TEXT_CLEAR " - sets a pcbs priority\n"
            "pcb show     " TEXT_YELLOW "[name*]             " TEXT_CLEAR " - Displays specific process\n"
            "pcb loadR3   " TEXT_YELLOW "<suspended***>      " TEXT_CLEAR " - Creates 5 processes for testing contexts\n"
            //"pcb yield                         - Yields comhand so queued processes can run\n"
            "pcb showready                     - Displays all ready processes\n"
            "pcb showrunning                   - Displays all (1) running processes\n"
            "pcb showblocked                   - Displays all blocked processes\n"
            "pcb showall                       - Displays all processes\n"
            TEXT_YELLOW "*name must be, at most, 8 characters long.\n"
            "**priority must be between 0 and 9: 0 the highest priority, 9 the lowest.\n" 
            "***include \"suspended\" to load processes initially in suspended state.\n" TEXT_CLEAR;
        print(HELP_KERNEL);
    }
    else if (strcmp(toks[1], "clear") == 0)
    {
        const char* HELP_CLEAR = 
            "clear - Clears the terminal.\n"
            "\n"
            "Clear takes no additional arguments.\n";
        print(HELP_CLEAR);
    }
   /* else if (strcmp(toks[1], "yield") == 0)
    {
        const char* HELP_YIELD = 
            "yield - Yields comhand so queued processes can run.\n"
            "Is an alias for the command \"pcb yield\".\n"
            "\n"
            "Yield takes no additional arguments.\n";
        print(HELP_YIELD);
    }*/
   else if (strcmp(toks[1], "alarm") == 0)
   {
        const char* HELP_ALARM = 
        "alarm - creates a alarm process that will display a message at\n"
        "a specified time\n"
        "\n"
        "Additional args:\n"
        "alarm " TEXT_YELLOW "[HH:MM:SS] [message]" TEXT_CLEAR " - sets alarm for time HH:MM:SS (military time) \n" 
        "that will display message.\n"; 
        print(HELP_ALARM); 
   }
   else if (strcmp(toks[1], "mcb") == 0)
   {
    const char* HELP_MCB = 
    "mcb - user-interactions with MCBs (Memory Control Blocks).\n"
    "\n"
    "Additional args:\n"
    //"mcb allocate " TEXT_YELLOW "[mem size*]" TEXT_CLEAR " - allocates heap memory, printing the address in hex.\n"
    //"mcb free     " TEXT_YELLOW "[address**]" TEXT_CLEAR " - frees heap memory.\n"
    "mcb showallocated        - Shows all allocated MCBs.\n"
    "mcb showfree             - Shows all free MCBs.\n"
    "mcb showall              - shows all MCBs.\n";
    //TEXT_YELLOW " *the size of memory to be allocated in decimal (with an absolute maximum\n  of 50,000) - ie. 'mcb allocate 1000'\n"
    //"**the memory address (in hex) to be freed - ie. 'mcb free 0x---'\n" TEXT_CLEAR;
    print(HELP_MCB);
   }
   else 
   {
        printf(TEXT_RED "\"%s\" is not a valid command.\ntype \"help\" for a list of valid commands.\n" TEXT_CLEAR, toks[1]);
   }
    print(HELP_CLOSER);
    sys_req(UNLOCK, COM1);
}
