#ifndef MPX_SERIAL_H
#define MPX_SERIAL_H

/**
 @file mpx/serial.h
 @brief Kernel functions and constants for handling serial I/O
*/

#include <stddef.h>
#include <mpx/device.h>
#include <mpx/dcb.h>

#define serial_data_available(dev) (inb(dev + LSR) & 1) == 1

extern dcb_t dcb[4];

int serial_devno(device dev);

/**
 Initializes devices for user input and output
 @param device A serial port to initialize (COM1, COM2, COM3, or COM4)
 @return 0 on success, non-zero on failure
*/
int serial_init(device dev);

/**
 Writes a buffer to a serial port
 @param device The serial port to output to
 @param buffer A pointer to an array of characters to output
 @param len The number of bytes to write
 @return The number of bytes written
*/
int serial_out(device dev, const char *buffer, size_t len);

/**
 Reads a string from a serial port
 @param device The serial port to read data from
 @param buffer A buffer to write data into as it is read from the serial port
 @param count The maximum number of bytes to read
 @return The number of bytes read on success, a negative number on failure
*/   		   

int serial_poll(device dev, char *buffer, size_t len);

/**
 Writes a buffer to a serial port without moving the cursor
 @param device The serial port to output to
 @param buffer A pointer to an array of characters to output
 @param len The number of bytes to write
 @param extra_whitespace Number of extra spaces written at the end to clear old characters written
 @return The number of bytes written
*/
int serial_out_post(device dev, const char *buffer, size_t len, const unsigned int extra_whitespace);

/**
 Perform a backspace at a position in buffer, echoing the result to the given serial port
 @param device The serial port to output to
 @param buffer The buffer that the backspace is performed in
 @param pos_ptr Pointer to where position in buffer is stored
 @param buffer_filled_ptr Pointer to where buffer filled capacity is stored
 @return -1 if backspace is attempted at beginning of buffer, 0 otherwise
 */
int serial_buffer_backspace(device dev, char *buffer, unsigned int * const pos_ptr, unsigned int * const buffer_filled_ptr);

/**
 Perform a delete at a position in buffer, echoing the result to the given serial port
 @param device The serial port to output to
 @param buffer The buffer that the delete is performed in
 @param pos_ptr Pointer to where position in buffer is stored
 @param buffer_filled_ptr Pointer to where buffer filled capacity is stored
 @return -1 if delete is attempted at end of buffer, 0 otherwise
 */
int serial_buffer_delete(device dev, char *buffer, unsigned int * const pos_ptr, unsigned int * const buffer_filled_ptr);

// R6 MANUAL THIS


int serial_open(device dev, int speed);

int serial_close(device dev);

int serial_read(device dev);

int serial_write(device dev);

void serial_interrupt(void);

void serial_input_interrupt(dcb_t *dcb_ptr);

void serial_output_interrupt(dcb_t *dcb_ptr);



#endif
