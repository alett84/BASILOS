#include <mpx/io.h>
#include <mpx/serial.h>
#include <string.h>
#include <sys_req.h>
#include <mpx/interrupts.h>
#include <mpx/dcb.h>

extern void serial_isr(void*);

#define GNOME 1

//private function only used here (R6)
static unsigned short dev_from_dno(int dno);


enum uart_registers {
	RBR = 0,	// Receive Buffer
	THR = 0,	// Transmitter Holding
	DLL = 0,	// Divisor Latch LSB
	IER = 1,	// Interrupt Enable
	DLM = 1,	// Divisor Latch MSB
	IIR = 2,	// Interrupt Identification
	FCR = 2,	// FIFO Control
	LCR = 3,	// Line Control
	MCR = 4,	// Modem Control
	LSR = 5,	// Line Status
	MSR = 6,	// Modem Status
	SCR = 7,	// Scratch
};

const int valid_bauds[] = {110, 150, 300, 600, 1200, 2400, 4800, 9600, 19200};

static int initialized[4] = { 0 };

dcb_t dcb[4] = { 0 };

const int pic_level_per_dno[4] = {4, 3, 4, 3};

int serial_devno(device dev)
{
	switch (dev) {
	case COM1: return 0;
	case COM2: return 1;
	case COM3: return 2;
	case COM4: return 3;
	}
	return -1;
}

int serial_init(device dev)
{
	int dno = serial_devno(dev);
	if (dno == -1) {
		return -1;
	}
	outb(dev + IER, 0x00);	//disable interrupts
	outb(dev + LCR, 0x80);	//set line control register
	outb(dev + DLL, 115200 / 9600);	//set bsd least sig bit
	outb(dev + DLM, 0x00);	//brd most significant bit
	outb(dev + LCR, 0x03);	//lock divisor; 8bits, no parity, one stop
	outb(dev + FCR, 0xC7);	//enable fifo, clear, 14byte threshold
	outb(dev + MCR, 0x0B);	//enable interrupts, rts/dsr set
	(void)inb(dev);		//read bit to reset port
	initialized[dno] = 1;
	return 0;
}

int serial_out(device dev, const char *buffer, size_t len)
{
	int dno = serial_devno(dev);
	if (dno == -1 || initialized[dno] == 0) {
		return -1;
	}
	for (size_t i = 0; i < len; i++) {
		outb(dev, buffer[i]);
	}
	return (int)len;
}

int serial_poll(device dev, char *buffer, size_t len)
{
	// error handling to confirm the device is available and initialized
	int dno = serial_devno(dev);
	if (dno == -1 || initialized[dno] == 0) 
	{
		return -1;
	}
	
	unsigned int pos = 0;
	unsigned int buffer_filled = 0;

	while (buffer_filled < (unsigned int)len - 1) // buffer isn't full
	{
		if (serial_data_available(dev)) {
			char c = inb(dev);

			if (c == '\r' || c == '\n') // is carriage return or newline
			{ 
				break;
			} 
			else if (c == '\b') // is backspace
			{ 
				serial_buffer_backspace(dev, buffer, &pos, &buffer_filled);
			}
			else if (c == '\x7F') // is delete (backspace in windows cmd line)
			{ 
				#if GNOME == 0
				serial_buffer_backspace(dev, buffer, &pos, &buffer_filled);
				#else
				serial_buffer_delete(dev, buffer, &pos, &buffer_filled);
				#endif
			}
			else if (c == '\x1B') // is escape character
			{ 
				char seq[10] = {0};
				for (unsigned int i = 0; serial_data_available(dev) && i < 10; i++) 
				{
					seq[i] = inb(dev);
				}

				if (strcmp(seq, &UP[1]) == 0) // up 
				{
					/* some functionality for up key */
				} 
				else if (strcmp(seq, &DOWN[1]) == 0) // down
				{
					/* some functionality for down key */
				} 
				else if (strcmp(seq, &RIGHT[1]) == 0) // right
				{ 
					if (pos >= buffer_filled) 
					{
						continue;
					}
					serial_out(dev, RIGHT, 3);
					pos++;
				} 
				else if (strcmp(seq, &LEFT[1]) == 0) // left
				{ 
					if (pos <= 0) 
					{
						continue;
					}
					serial_out(dev, LEFT, 3);
					pos--;
				} 
				else if (strcmp(seq, &DELETE[1]) == 0) // delete in windows cmd line
				{ 
					#if GNOME == 0
					serial_buffer_delete(dev, buffer, &pos, &buffer_filled);
					#else
					serial_buffer_backspace(dev, buffer, &pos, &buffer_filled);
					#endif
				}
					
			} 
			else // is regular character
			{ 
				outb(dev, c);
				for (unsigned int i = buffer_filled; i > pos; i--) 
				{
					// shift characters forward if typing from middle of buffer
					buffer[i] = buffer[i - 1];	
				}
				serial_out_post(dev, &buffer[pos + 1], buffer_filled - pos, 0);

				buffer[pos] = c;
				buffer_filled++;
				pos++;
			}
		}
	}

	buffer[buffer_filled] = '\0';
	buffer_filled++;
	outb(dev, '\n');

	return (int)buffer_filled;
}

int serial_out_post(device dev, const char *buffer, size_t len, const unsigned int extra_whitespace)
{
	int res = serial_out(dev, buffer, len);
	if (res == -1) 
	{
		// error handling to confirm the device is available and initialized
		return -1;
	}

	for (unsigned int i = 0; i < extra_whitespace; i++) 
	{
		outb(dev, ' ');
	}

	// shift displayed cursor back to internal position
	for (unsigned int i = 0; i < len + extra_whitespace; i++) 
	{
		serial_out(dev, LEFT, 3);
	}

	return res + (int) extra_whitespace;
}

int serial_buffer_backspace(device dev, char *buffer, unsigned int * const pos_ptr, unsigned int * const buffer_filled_ptr) 
{
	if (*pos_ptr <= 0) 
	{
		return -1;
	}
	
	(*pos_ptr)--;
	serial_out(dev, LEFT, 3);
	for (unsigned int i = *pos_ptr; i < *buffer_filled_ptr - 1; i++) 
	{
		// shift characters backward after cursor
		buffer[i] = buffer[i + 1];					
	}
	(*buffer_filled_ptr)--;

	serial_out_post(dev, &buffer[*pos_ptr], *buffer_filled_ptr - *pos_ptr, 1);
	return 0;
}

int serial_buffer_delete(device dev, char *buffer, unsigned int * const pos_ptr, unsigned int * const buffer_filled_ptr) 
{
	if (*pos_ptr >= *buffer_filled_ptr) 
	{
		return -1;
	}
	
	for (unsigned int i = *pos_ptr; i < *buffer_filled_ptr - 1; i++) 
	{
		// shift characters backward after cursor
		buffer[i] = buffer[i + 1];					
	}
	(*buffer_filled_ptr)--;

	serial_out_post(dev, &buffer[*pos_ptr], *buffer_filled_ptr - *pos_ptr, 1);
	return 0;
}

// R6 MANUAL THIS

int serial_open(device dev, int speed)
{
	/* 0) perform checks defined in detailed R6 documentation */
	int dno = serial_devno(dev);
	if (dno == -1) 
	{
		return -101; // invalid devno
	}

	int baud_check = 0;
	for (unsigned int i = 0; i < (sizeof(valid_bauds) / sizeof(int)); i++)
	{
		if (valid_bauds[i] == speed)
		{
			baud_check = 1;
			break;
		}
	}
	if (!baud_check)
	{
		return -102; // invalid baud rate
	}

	/* 2) Initialize the associated DCB */
	dcb_t* dcb_ptr = &dcb[dno];

	dcb_ptr->dev = dev;
	dcb_ptr->open = DCB_OPEN;
	dcb_ptr->event_flag = 0;
	dcb_ptr->status = DCB_IDLE;
	dcb_ptr->locking_pcb_ptr = NULL;
	dcb_ptr->locking_level = 0;

	dcb_ptr->ring_in = 0;
	dcb_ptr->ring_out = 0;
	dcb_ptr->iocb_ptr = NULL;

	/* 3) Install the appropriate ISR */
	idt_install(0x20 + pic_level_per_dno[dno], serial_isr);

	/* 4) Set the port speed to the speed parameter */
	int baud_rate_div = 115200 / (long) speed;

	/* 5) Set other line characteristics (8 data bits, no parity, 1 stop bit) */
	outb(dev + IER, 0x00);							//disable interrupts
	outb(dev + LCR, 0x80);							//set line control register to access dl (baud rate div)
	outb(dev + DLL, baud_rate_div & 0xFF);			//set bsd least sig bit
	outb(dev + DLM, (baud_rate_div >> 8) & 0xFF);	//brd most significant bit
	outb(dev + LCR, 0x03);							//lock divisor; 8bits, no parity, one stop

	/* 6) Enable all necessary interrupts */
	cli();
	int mask = inb(0x21);
	mask &= ~(1 << pic_level_per_dno[dno]); 		// Clear (enable) IRQ for dev
	outb (0x21, mask);
	sti();

	outb(dev + MCR, 0x08);							// Enable overall serial port interrupts
	outb(dev + IER, 0x01);							// enable input ready interrupts only
	initialized[dno] = 1;

	/* 7) Return an integer indicating success or failure */
	return 0;
}

int serial_close(device dev)
{
	/* 0) perform checks defined in detailed R6 documentation */
	int dno = serial_devno(dev);
	if (dno == -1 || !initialized[dno])
	{
		return -201; // device not initialized
	}

	/* 2) Clear open indicator of associated DCB */
	dcb_t* dcb_ptr = &dcb[dno];

	dcb_ptr->open = DCB_CLOSED;

	/* 3) Disable associated interrupt in PIC mask */
	cli();
	int mask = inb(0x21);
	mask |= (1 << dno); 							// SET (disable) IRQ for dev
	outb (0x21, mask);
	sti();

	outb(dev + MSR, 0x00);							// disable interrupts
	outb(dev + IER, 0x00);							// disable interrupts

	initialized[dno] = 0;

	/* 4) Return an integer indicating success or failure */
	return 0;
}

int serial_read(device dev)
{
	/* 0) perform checks defined in detailed R6 documentation */
	int dno = serial_devno(dev);
	if (dno == -1 || !initialized[dno])
	{
		return -301; // device not initialized
	}

	dcb_t* dcb_ptr = &dcb[dno];

	if (!dcb_ptr->open)
	{
		return -301; // port not open
	}

	iocb_t* iocb_ptr = dcb_ptr->iocb_ptr;

	if (dcb_ptr->status != DCB_IDLE || dcb_ptr->event_flag || !dcb_is_open_to_iocb(dcb_ptr, iocb_ptr))
	{
		return -304; // device busy
	}

	dcb_ptr->status = DCB_READING;
	dcb_ptr->event_flag = 0;

	// iterate through all of ring buffer
	cli();
	while (dcb_ring_has_data(dcb_ptr) && iocb_ptr->buffer_pos <= iocb_ptr->buffer_size && dcb_ptr->ring_buffer[dcb_ptr->ring_out] != '\n' && dcb_ptr->ring_buffer[dcb_ptr->ring_out] != '\r')
	{
		iocb_ptr->buffer[iocb_ptr->buffer_pos++] = dcb_ring_pop(dcb_ptr);
	}
	sti();

	if (iocb_ptr->buffer_pos <= iocb_ptr->buffer_size && dcb_ptr->ring_buffer[dcb_ptr->ring_out] != '\n' && dcb_ptr->ring_buffer[dcb_ptr->ring_out] != '\r')
	{
		// need more data
		return 0;
	} 

	if (dcb_ring_has_data(dcb_ptr) && (dcb_ptr->ring_buffer[dcb_ptr->ring_out] == '\n' || dcb_ptr->ring_buffer[dcb_ptr->ring_out] == '\r')) 
	{
		// encountered ENTER
		cli();
		dcb_ring_pop(dcb_ptr);
		sti();
	}

	// clear the stuff cause we are done
	dcb_dequeue(dcb_ptr);

	return iocb_ptr->buffer_pos;
}

int serial_write(device dev)
{
	/* 0) perform checks defined in detailed R6 documentation */
	int dno = serial_devno(dev);
	if (dno == -1 || !initialized[dno])
	{
		return -401; // device not initialized
	}

	dcb_t* dcb_ptr = &dcb[dno];

	if (!dcb_ptr->open)
	{
		return -401; // port not open
	}

	iocb_t* iocb_ptr = dcb_ptr->iocb_ptr;

	if (dcb_ptr->status != DCB_IDLE || dcb_ptr->event_flag || !dcb_is_open_to_iocb(dcb_ptr, iocb_ptr))
	{
		return -404; // device busy
	}
	
	dcb_ptr->status = DCB_WRITING;
	dcb_ptr->event_flag = 0;

	outb(dev + THR, iocb_ptr->buffer[iocb_ptr->buffer_pos++]);

	char val = inb(dev + IER);
	outb(dev + IER, val | 0x02);	// enable write interrupts

	return 0;
}

void serial_interrupt(void)
{
	unsigned char iir;
    dcb_t *curr;
    int dno;

	/* 1) interrupts enabled/disabled by assembly isr */

    /* 2) Figure out which UART(s) raised an IRQ and dispatch */
    for (dno = 0; dno < 4; dno++) {
        if (!initialized[dno]) {
            /* port never opened/initialized, skip */
            continue;
        }

        unsigned short base = dev_from_dno(dno);
        iir = inb(base + IIR);

        /* bit0 == 1 means “no interrupt pending” */
        if (iir & 0x01) {
            continue;
        }

        curr = &dcb[dno];

        /* bits 1–3 of IIR tell which interrupt */
        switch (iir & 0x0E) {
            case 0x04:  /* Received Data Available */
            case 0x0C:  /* Character Timeout Indicator */
                serial_input_interrupt(curr);
                break;

            case 0x02:  /* Transmitter Holding Register Empty */
                serial_output_interrupt(curr);
                break;

            default:
                /* line status, modem status, etc —
                   you can handle or just ignore */
                break;
        }
		break;
    }

    // Issue EOI to the PIC(s) 
    // master PIC EOI 
    outb(0x20, 0x20);
}

void serial_input_interrupt(dcb_t *dcb_ptr)
{
	//Read the incoming character 
	char c = inb(dcb_ptr->dev + RBR);

	//if user-read in progress, deposit into IOCB buffer
	if(dcb_ptr->status == DCB_READING)
	{
		iocb_t* iocb_ptr = dcb_ptr->iocb_ptr;
		iocb_ptr->buffer[iocb_ptr->buffer_pos++] = c;

		//on newline or full buffer signal complete 
		if(c == '\r' || c == '\n' || iocb_ptr->buffer_pos >= iocb_ptr->buffer_size)
		{
			dcb_dequeue(dcb_ptr);
		}
	}	
	else 
	{
		//else push into ring buffer if theres room 
		dcb_ring_push(dcb_ptr, c);
		//else drop the byte if the ring is full 
	}

}

void serial_output_interrupt(dcb_t *dcb_ptr)
{
	/* Only handle it if we’re in a write operation */
	if (dcb_ptr->status != DCB_WRITING)
	{
		return;
	}

	/* Are there more chars to send? */
	if (dcb_ptr->iocb_ptr->buffer_pos < dcb_ptr->iocb_ptr->buffer_size) 
	{
		/* Send next byte */
		outb(dcb_ptr->dev + THR, dcb_ptr->iocb_ptr->buffer[dcb_ptr->iocb_ptr->buffer_pos++]);
	}
	else 
	{
		/* Last byte was already sent → turn off TX interrupts */
		outb(dcb_ptr->dev + IER, inb(dcb_ptr->dev + IER) & ~(1 << 1)); // clear bit-1 = THR empty interrupt
	
		/* Signal completion */
		dcb_dequeue(dcb_ptr);
		
	}
}

/* Helper to turn device number into COM port base address */
/*R6 CHANGE*/
static unsigned short dev_from_dno(int dno) 
{
    switch (dno) {
    case 0: return COM1;   // 0x3F8
    case 1: return COM2;   // 0x2F8
    case 2: return COM3;   // 0x3E8
    case 3: return COM4;   // 0x2E8
    }
    return 0;
}

// eof
