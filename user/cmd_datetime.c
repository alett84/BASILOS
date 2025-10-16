#include <mpx/cmd_datetime.h>

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <mpx/mpxio.h>

#include <rtc.h>

void cmd_time(char** toks, int token_count) 
{
    if (toks[1] == NULL) 
    {
        print("time command requires second argument.\nUse \"help time\" to see proper usage of time.\n");
        return;
    }

    strtolower(toks[1]); 
    if (strcmp(toks[1], "get") == 0) 
    {
        cmd_time_get();
    } 
    else if (strcmp(toks[1], "set") == 0) 
    {
        cmd_time_set(toks, token_count);
    } 
    
    else 
    {
        print("Invalid 2nd argument.\nUse \"help time\" to see proper usage of time.\n");
    }
}

void cmd_time_get() 
{
    rtc_time_t now;
    get_rtc(&now);

    printf("%2d:%2d:%2d\n", now.hour, now.minute, now.second);
}

void cmd_time_set(char** toks, int token_count)
{
    if (token_count < 3 || token_count == 4) 
    {
        print("Incorrect formatting.\nPlease type time as HH:MM:SS, military time\n");
    } 
    else 
    {
        char* token = strtok(toks[2], ":");
        while (token_count < 5)
        {
            token = strtok(NULL, ":");
            if (token == NULL) 
            {
                print("Incorrect formatting.\nPlease type time as HH:MM:SS, military time\n");
            }
            toks[token_count] = token;
            token_count++;
        }

        int hour   = atoi(toks[2]);
        int minute = atoi(toks[3]);
        int second = atoi(toks[4]);

        // Validate the time values.
        if (hour < 0 || hour > 23 ||
                minute < 0 || minute > 59 ||
                second < 0 || second > 59) 
        {
            print("Invalid time values.\n");
        } 
        else 
        {
            rtc_time_t now;
            get_rtc(&now); 
            
            now.hour   = hour;
            now.minute = minute;
            now.second = second;
            
            // Call set_rtc with the updated time 
            set_rtc(now.day, now.month, now.year, now.hour, now.minute, now.second);
            print("Time updated successfully.\n");
        }
    }  
}

void cmd_date(char** toks, int token_count)
{
    if (toks[1] == NULL) 
    {
        print("Date command requires second argument.\nUse \"help date\" to see proper usage of date.\n");
        return;
    } 

    strtolower(toks[1]); 
    if (strcmp(toks[1], "get") == 0) 
    {
        cmd_date_get();
    } 
    else if (strcmp(toks[1], "set") == 0) 
    {
        cmd_date_set(toks, token_count);
    } 
    else 
    {
        print("Invalid 2nd argument.\nUse \"help date\" to see proper usage of date.\n");
    }
}

void cmd_date_get()
{
    rtc_time_t now;
    get_rtc(&now);

    printf("%2d/%2d/%2d\n", now.month, now.day, now.year);
}

void cmd_date_set(char** toks, int token_count)
{
    // Expect: date set MM/DD/YY.
    if (token_count < 3 || token_count == 4) 
    {
        print("Incorrect formatting.\nPlease type date as MM/DD/YY.\n");
    } 
    else 
    {
        char* token = strtok(toks[2], "/\\");
        while (token_count < 5)
        {
            token = strtok(NULL, "/\\");
            if (token == NULL) 
            {
                print("Incorrect formatting.\nPlease type date as MM/DD/YY.\n");
            }
            toks[token_count] = token;
            token_count++;
        }

        int month = atoi(toks[2]);
        int day   = atoi(toks[3]);
        int year  = atoi(toks[4]);
                
        // Basic validation
        if (month < 1 || month > 12 || day < 1 || day > month_length(month, year)) 
        {
            print("Invalid date values.\n");
        } 
        else 
        {
            rtc_time_t now;
            get_rtc(&now); 
            
            now.month = month;
            now.day   = day;
            now.year  = year;
            
            // Update the RTC with the new date
            set_rtc(now.day, now.month, now.year, now.hour, now.minute, now.second);
            print("Date updated successfully.\n");
        }
    }
}



// line at end of file
