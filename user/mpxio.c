#include <mpx/mpxio.h>
#include <mpx/io.h>
#include <sys_req.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <stddef.h>
#include <mpx/serial.h>

void print(const char* str) 
{
    sys_req(WRITE, COM1, str, strlen(str));
}

void printf(const char* str, ...) 
{
    sys_req(LOCK, COM1);

    va_list args;
	va_start(args, str);

    const char* str_rem = str;
    for (const char* f = strchr(str_rem, '%'); f != NULL; f = strchr(str_rem, '%'))
    {
        if ((f - str_rem) != 0)
        {
            sys_req(WRITE, COM1, str_rem, f - str_rem);
        }

        const char* chr = strpbrk(f+1, "%sdixX");
        if (chr == NULL)
        {
            // unrecognized format sequence :/
            sys_req(UNLOCK, COM1);
            return;
        }
        
        if (chr[0] == '%')
        {
            char percent[1] = {'%'};
            sys_req(WRITE, COM1, percent, 1);
        }
        else if (chr[0] == 's')
        {
            char* strarg = va_arg(args, char*);
            print(strarg);
        }
        else if (chr[0] == 'd' || chr[0] == 'i' || chr[0] == 'x' || chr[0] == 'X')
        {
            int base = 10;
            if (chr[0] == 'x' || chr[0] == 'X')
            {
                base = 16;
            } 
            int intarg = va_arg(args, int);
            int width;
            if (chr - f > 1)
            {
                width = atoi(f + 1);
            }
            else 
            {
                width = get_i_width(intarg, base);
            }
            char* intstr = sys_alloc_mem(sizeof(char) * (width + 1));
            itoa(intarg, intstr, width, base);
            intstr[width] = '\0';
            if (chr[0] == 'x')
            {
                strtolower(intstr);
            }
            print(intstr);
            sys_free_mem(intstr);
        }
        str_rem = chr + 1;
    }
    print(str_rem);
    va_end(args);

    sys_req(UNLOCK, COM1);
}

int input(char** toks, int token_size, char* buf, int buffer_size)
{
    sys_req(LOCK, COM1);

    print(TEXT_GREY "> " TEXT_CLEAR);
    int nread = mpx_poll(buf, buffer_size);

    if(nread <= 1) 
    {
        sys_req(UNLOCK, COM1);
        return -1; //ignore failed and empty reads
    }

    //Tokenize the input line
    int token_count = 0; 
    char* token = buf;
    for (;;)
    {
        for (; strchr(WHITESPACE, token[0]) != NULL; token++);
        if (token[0] == '\0' || token_count >= token_size)
        {
            break;
        }
        else if (token[0] == '\"')
        {
            token[0] = '\0';
            token++;
            toks[token_count++] = token;
            token = (char*) strchr(token, '\"');
            if (token == NULL)
            {
                print(TEXT_RED "Malformed command: Missing a closing parenthesis\n" TEXT_CLEAR);
                sys_req(UNLOCK, COM1);
                return -1;
            }
            token[0] = '\0';
            token++;
        }
        else
        {
            toks[token_count++] = token;
            token = (char*) strpbrk(token, WHITESPACE);
            if (token == NULL) 
            {
                break;
            }
            token[0] = '\0';
            token++;
        }
    }

    sys_req(UNLOCK, COM1);
    return token_count;
}


int mpx_poll(char *buffer, size_t len) {
    size_t idx = 0;
    char   c;

    while (1) {
        // BLOCK until a byte arrives on COM1
        sys_req(READ, COM1, &c, 1);

        // Handle ENTER/RETURN
        if (c == '\r' || c == '\n') {
            if (idx < len - 1) {
                buffer[idx++] = '\n';
            }
            print("\r\n");
            break;
        }

        // Handle BACKSPACE/DELETE
        if ((c == '\b' || (unsigned char)c == 0x7F) && idx > 0) {
            idx--;
            const char bs[3] = {'\b',' ','\b'};
            sys_req(WRITE, COM1, bs, 3);
            continue;
        }

        // Printable ASCII?
        if ((unsigned char)c >= 0x20 && (unsigned char)c <= 0x7E) {
            // Store in buffer if there’s room
            if (idx < len - 1) {
                buffer[idx++] = c;
                // Echo to console
                sys_req(WRITE, COM1, &c, 1);
            }
        }
    }

    buffer[idx] = '\0';
    return (int)idx;
}
// eof
