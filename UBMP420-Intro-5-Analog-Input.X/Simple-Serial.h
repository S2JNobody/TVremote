/*==============================================================================
 File: SimpleSerial.h
 Date: April 2, 2022
 
 UBMP4 Simple serial output function prototypes
 
 Function prototypes for RS-232 style serial output -- useful for debugging
 programs using an oscilloscope with serial bus decoding. This example code
 demonstrates simple and minimal 9600 bps serial transmission.
==============================================================================*/

/** ** *
 * Function: void H1_serial_config(void)
 * 
 * Configure H1 for serial output and set output pin high for idle state.
 */
void H1_serial_config(void);

/**
 * Function: void H1_serial_write(unsigned char)
 * 
 * Write one byte of 9600,8,N,1 serial data (9600bps, 8 bits, no parity, and 1
 * stop bit) to H1.
 */
void H1_serial_write(unsigned char);
