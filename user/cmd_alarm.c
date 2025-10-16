#include <mpx/cmd_alarm.h>
#include <mpx/mpxio.h>
#include <mpx/io.h>
#include <mpx/isr.h>
#include <sys_req.h>
#include <mpx/user_pcb.h>
#include <mpx/kernel_pcb.h>
#include <mpx/interrupts.h>
#include <processes.h>
#include <rtc.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ALARMS       10

/**
 * Stores data for each active alarm.
 */
typedef struct {
    int  hour;
    int  minute;
    int  second; 
    char message[80];
    char pcb_name[12];  
    int  active;  // 1 if in use, 0 if free
} alarm_data_t;

static alarm_data_t g_alarms[MAX_ALARMS];
static int g_alarm_count = 0;
int the_magical_wowzer = 0; // i have completely given up doing this in any elegant way

void alarm_proc(void);

/**
 * command: alarm HH:MM:SS message...
 */
void cmd_alarm(char **toks, int token_count)
{
    // changed usage line to mention seconds
    if (token_count < 3) {
        print("Usage: alarm HH:MM:SS <message>\n");
        return;
    }

    // Check how many alarms are already active
    if (g_alarm_count >= MAX_ALARMS) {
        print(TEXT_RED "Too many alarms active.\n" TEXT_CLEAR);
        return;
    }

    // Copy time string (HH:MM:SS) into a local buffer so we can modify it
    char time_buf[16];
    for (int i = 0; i < 16; i++) {
        time_buf[i] = '\0';
    }
    strcpy(time_buf, toks[1]);

    // Find the first colon
    char *colon_ptr1 = (char *)strchr(time_buf, ':');
    if (!colon_ptr1) {
        print(TEXT_RED "Time must be in HH:MM:SS format.\n" TEXT_CLEAR);
        return;
    }
    *colon_ptr1 = '\0';  // split HH\0MM:SS
    int hh = atoi(time_buf);

    // Find the second colon
    char *colon_ptr2 = (char *)strchr(colon_ptr1 + 1, ':');
    if (!colon_ptr2) {
        print(TEXT_RED "Time must be in HH:MM:SS format.\n" TEXT_CLEAR);
        return;
    }
    *colon_ptr2 = '\0';  // split HH\0MM\0SS
    int mm = atoi(colon_ptr1 + 1);

    // The remainder is seconds
    int ss = atoi(colon_ptr2 + 1);

    // Validate hour, minute, second
    if (hh < 0 || hh > 23 || mm < 0 || mm > 59 || ss < 0 || ss > 59) {
        print(TEXT_RED "Invalid time. Must be in 00:00:00..23:59:59.\n" TEXT_CLEAR);
        return;
    }

    // Combine the rest of the tokens (2..end) into an alarm message
    char message_buf[80];
    for (int i = 0; i < 80; i++) {
        message_buf[i] = '\0';
    }
    for (int i = 2; i < token_count; i++) {
        strcat(message_buf, toks[i]);
        if (i < token_count - 1) {
            strcat(message_buf, " ");
        }
    }

    // Find a free slot in g_alarms
    int alarm_slot = -1;
    for (int i = 0; i < MAX_ALARMS; i++) {
        if (!g_alarms[i].active) {
            alarm_slot = i;
            break;
        }
    }
    if (alarm_slot < 0) {
        print(TEXT_RED "No free alarm slots.\n" TEXT_CLEAR);
        return;
    }

    // Fill out the slot
    g_alarms[alarm_slot].hour   = hh;
    g_alarms[alarm_slot].minute = mm;
    g_alarms[alarm_slot].second = ss;  // new line for seconds
    strcpy(g_alarms[alarm_slot].message, message_buf);
    g_alarms[alarm_slot].active = 1;

    // Build PCB "ALARM#"
    char idx_str[4];
    for (int i = 0; i < 4; i++) {
        idx_str[i] = '\0';
    }
    itoa(alarm_slot, idx_str, 3, 10); // base 10
    strcpy(g_alarms[alarm_slot].pcb_name, "ALARM");
    strcat(g_alarms[alarm_slot].pcb_name, idx_str);

    // Allocate a new PCB
    pcb_t *new_pcb = pcb_allocate();
    if (!new_pcb) {
        print(TEXT_RED "Failed to allocate PCB.\n" TEXT_CLEAR);
        g_alarms[alarm_slot].active = 0;
        return;
    }

    // Initialize fields
    strcpy(new_pcb->name, g_alarms[alarm_slot].pcb_name);
    new_pcb->class_type = PCB_SYSTEM;
    new_pcb->priority   = 0;
    new_pcb->state      = PCB_READY;
    new_pcb->suspended  = PCB_NOT_SUSPENDED;

    // Clear stack
    for (int i = 0; i < STACK_SIZE; i++) {
        new_pcb->stack[i] = 0;
    }

    // Prepare the context
    new_pcb->stack_ptr -= sizeof(context_t);
    context_t *ctx = (context_t *)new_pcb->stack_ptr;
    for (int i = 0; i < (int)sizeof(context_t); i++) {
        ((char *)ctx)[i] = 0;
    }

    // Segment registers
    ctx->ds = 0x10;
    ctx->es = 0x10;
    ctx->fs = 0x10;
    ctx->gs = 0x10;
    ctx->ss = 0x10;

    // CS + eflags
    ctx->cs = 0x08;
    ctx->eflags = 0x202;  // IF=1

    // Entry point
    ctx->eip = (unsigned int)alarm_proc;

    the_magical_wowzer = alarm_slot;

    // Insert into the ready queue
    pcb_insert(new_pcb);
    g_alarm_count++;

    print("Alarm created successfully.\n");
}

/**
 * The alarm process function.
 * read the alarm_slot from EBX.
 */
void alarm_proc(void)
{
    alarm_data_t *my_alarm = &g_alarms[the_magical_wowzer];

    // Wait for the clock to reach (hour:minute:second)
    while (1) {
        rtc_time_t now;
        get_rtc(&now);

        // updated comparison to include seconds
        if ( (now.hour > my_alarm->hour)
             || (now.hour == my_alarm->hour && now.minute > my_alarm->minute)
             || (now.hour == my_alarm->hour && now.minute == my_alarm->minute
                 && now.second >= my_alarm->second) ) 
        {
            printf(TEXT_MAGENTA "\aALARM (" TEXT_CYAN "%02d:%02d:%02d" TEXT_MAGENTA "):" TEXT_CLEAR " %s\n",
                   my_alarm->hour,
                   my_alarm->minute,
                   my_alarm->second,  // include seconds in print
                   my_alarm->message);

            break;
        }
        // Yield the CPU so we don't busy-wait
        sys_req(IDLE);
    }

    // Mark slot free
    my_alarm->active = 0;
    g_alarm_count--;

    // Remove PCB
    delete_pcb(my_alarm->pcb_name);

    while (1) {
        sys_req(EXIT);
    }
}
