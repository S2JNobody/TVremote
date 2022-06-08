/*==============================================================================
 Project: Intro-5-Analog-Input
 Date:    April 2, 2022
 
 This program demonstrates the use of analog input, number conversion, and
 serial output functions using an external serial function library. (Serial
 output is useful for debugging when using a logic analyzer, or an oscilloscope
 with a serial decode function.)
 
 Additional program analysis activities investigate the analog constants and
 input functions in the UBMP420.h and UBMP420.c files, as well as explore
 bit-wise AND operations for clearing and testing bits.
==============================================================================*/

#include    "xc.h"              // Microchip XC8 compiler include file
#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include    "UBMP420.h"         // Include UBMP4.2 constants and functions
#include    "Simple-Serial.h"   // Include simple serial functions

// TODO Set linker ROM ranges to 'default,-0-7FF' under "Memory model" pull-down.
// TODO Set linker code offset to '800' under "Additional options" pull-down.

// ASCII character code definitions
#define LF      10              // ASCII line feed character code
#define CR      13              // ASCII carriage return character code

// Program variable definitions
unsigned char rawADC;           // Raw ADC conversion result

// Decimal character variables used by binary to decimal conversion function
unsigned char dec0;             // Decimal digit 0 - ones digit
unsigned char dec1;             // Decimal digit 1 - tens digit
unsigned char dec2;             // Decimal digit 2 - hundreds digit

// Convert an 8-bit binary number to 3 decimal digits
void bin_to_dec(unsigned char bin)
{
    dec0 = bin;             // Store number in ones digit
    dec1 = 0;               // Clear tens digit
    dec2 = 0;               // Clear hundreds digit
    
    // Count hundreds
    while(dec0 >= 100)
    {
        dec2 ++;
        dec0 = dec0 - 100;
    }
    // Count tens, dec0 will contain ones when done
    while(dec0 >= 10)
    {
        dec1 ++;
        dec0 = dec0 - 10;
    }
}

int main(void)
{
    // Set up ports and ADC
    OSC_config();               // Configure oscillator for 48 MHz
    UBMP4_config();             // Configure I/O ports for on-board devices
    ADC_config();               // Configure ADC and enable input on Q1
    H1_serial_config();         // Prepare for serial output on H1
        
    // Enable PORTC output except for phototransistor Q1 and IR receiver U2
    TRISC = 0b00001100;
    
    // If Q1 and U2 are not installed, all PORTC outputs can be enabled instead
    // TRISC = 0b00000000;
    
    // Enable on-die temperature sensor and set high operating Vdd range
    FVRCON = FVRCON | 0b00110000;
    
    // Select the on-die temperature indicator module as ADC input
    ADC_select_channel(ANTIM);
    // Wait the recommended acquisition time before A-D conversion
    __delay_us(200);
    
    // Select Q1 phototransistor as ADC input (can be enabled if Q1 is installed)
    // ADC_select_channel(ANQ1);
    
    while(1)
    {
        // Read selected ADC channel and display the analog result on the LEDs
        rawADC = ADC_read();
        LATC = rawADC;
        
        // Add serial write code from the program analysis activities here:
        
        __delay_ms(100);
        
        // Activate bootloader if SW1 is pressed.
        if(SW1 == 0)
        {
            RESET();
        }
    }
}

/* Learn More -- Program Analysis Activities
 * 
 * 1.   The TRISC register is originally set by the UBMP4_config() function, and
 *      is used to control the data direction (input, or output) of the PORTC
 *      I/O pins. A value of 0 in any TRIS register bit position configures the
 *      pin to be an output (0 is like 'o', for output), while a value of 1 sets
 *      the pin as an input (1 is like 'i', in input).
 * 
 *      What is the original value of TRISC set in UBMP4_config? How does the
 *      statement:
 * 
 *      TRISC = 0b00001100;
 * 
 *      in the main() function change the function of the PORTC I/O pins?
 * 
 * 2.   This program converts an analog input to a digital value representing
 *      the input, and outputs the digital bits to the PORTC pins connected to
 *      the UBMP4 header pins. A voltmeter can be used to read the result on
 *      the H1-H8 pins, except for H3 and H4. H3 and H4 have been set as inputs
 *      in this program since they connect to Q1 (an ambient light sensor or
 *      phototransistor) and U2 (an IR demodulator).
 * 
 *      If your UBMP4 has Q1 and U2 installed, leave the TRISC register as set,
 *      above. If Q1 and U2 are not installed, the TRISC register can be set to
 *      output on digital data on all of the header pins by changing it to:
 
    TRISC = 0b00000000;
 
 *      Read the conversion result by connecting a voltmeter between ground (the
 *      header pin closest to the circuit board edge -- marked with a ground
 *      symbol near the H1 legend) and each header signal pin (the header pin
 *      closest to the resistors -- marked with a pulse symbol).
 * 
 *      H1 is the least significant bit of the result, and H8 is the most
 *      significant bit. Starting at H8, record all 8 bits of the analog
 *      value, representing 5V as 1, and 0V as 0. What was your result?
 * 
 * 3.   The statement that outputs the digital value of the analog input to the
 *      header pins is: LATC = rawADC;
 * 
 *      Since the upper 4 bits of PORTC are also physically connected to LEDs
 *      D2-D5, they will light up to show these first four data bits as well.
 *      Does their value match your result, above?
 * 
 *      The lower 4 bits of the result can be displayed instead of the upper 4
 *      bits by shifting the contents of the ADC conversion result left,
 *      essentially sliding each bit 4 positions to its left. The upper 4 bits
 *      will be lost during this operation, and will be replaced by the lower
 *      4 bits.
 * 
 *      Update the LATC statement to match the statement, below. Rebuild the
 *      program and run it again.
 * 
        LATC = rawADC << 4;     // Shift result 4 bits to show low nybble on LEDs
 * 
 *      The least significant bits will be the ones that change most often. If
 *      you are using the built-in temperature sensor, try heating the micro-
 *      controller with your touch, or by cradling the circuit board in your
 *      hands until you see one or more bit changes indicated on the LEDs.
 * 
 * 4.   The PIC16F1459 microcontroller has 9 analog inputs shared with its
 *      digital I/O pins. Pins used for analog input must be explicity enabled
 *      as both input pins and analog pins. Describe how the ADC_config()
 *      function configures the Q1 pin for analog input.
 * 
 * 5.   The single ADC (analog-to-digital coverter) in the PIC16F1459 uses an
 *      input multiplexer (mux) to switch one analog input at a time to the ADC.
 *      After changing the input, a short delay is necessary to allow the input
 *      potential to settle before the conversion starts. The function call:
 * 
        ADC_select_channel(ANTIM);
 * 
 *      selects the microcontroller's on-board Temperature Indicator Module
 *      (ANTIM) as the ADC input. What other inputs are available? (Hint: see
 *      the UBMP420.h file)
 * 
 *      The ADC_select_channel() function does not add a delay after switching
 *      a new input to the ADC, but there is another function that switches
 *      inputs, adds a delay, and performs the analog to digital conversion.
 *      What is this function called? Why do you think it is not used here?
 * 
 * 6.   This program contains the bin_to_dec() function, which demonstrates a
 *      simple method of converting an 8-bit binary value to three decimal
 *      digits. Can you determine how it works? Explain, or use a flow chart
 *      to describe its operation.
 * 
 * 7.   Two serial data functions are included in the Simple-Serial.c file.
 *      These functions enable the program to output RS-232 formatted serial
 *      data on the H1 header pin. This allows a logic analyser or oscilloscope
 *      to be used to read the value of the serial data more conveniently and
 *      quickly than by successivley probing each of the header pins using a
 *      voltmeter.
 * 
 *      The format of the serial data is straightforward to understand. When no
 *      data is being transmitted, the serial line is 'idle', represented by a
 *      high, or 1 logic level.
 * 
 *      Every data transmission is preceeded by a low, or logic 0 level Start
 *      bit. Next, the data bits themselves are transmitted in order from LSB
 *      (least-significant bit) to MSB (most-significant bit). The transmission
 *      is ended with a logic 1 level Stop bit.
 * 
 *      The data bits, 0 or 1, are transmitted at a low or high voltage level,
 *      respectively, and each bit remains at its level for the entire bit
 *      duration (this is known as NRZ, or non-return-to-zero format). The bit
 *      duration is a reciprocal of the data rate. For example at 9600 bps
 *      (bits-per-second), each bit is 1/9600s, or roughly 104us (microseconds)
 *      in length.
 * 
 *      If you have access to an oscilloscope or logic analyzer, add the
 *      following code to the program (below the LATC output statement) to
 *      enable serial output. Then, probe H1 to view the serial data:
 
        // Write ADC result to H1 as binary serial data
        H1_serial_write(rawADC);
        
 *      Can you record or capture the digital serial data? What is the analogue
 *      value that it transmitted?
 * 
 * 8.   Rather than interpreting or converting the analogue value yourself, this
 *      next block of code can be added to your program to convert the analog
 *      result to three decimal digits, display each of the digits in ASCII
 *      format, and add the character codes signifying the end of a line of 
 *      text (CR) and the start of a new line of text (LF). Logic analysers or
 *      digital oscilloscopes may be able to decode these digits, or the output
 *      could be connected to a serial terminal to display the text.
 
        // Convert ADC result to decimal and write each digit serially in ASCII
        bin_to_dec(rawADC);
        H1_serial_write(dec2 + 0x30);   // Convert each digit to ASCII
        H1_serial_write(dec1 + 0x30);
        H1_serial_write(dec0 + 0x30);
        H1_serial_write(CR);            // Carriage return character
        H1_serial_write(LF);            // Line feed character
                
 * 
 * 9.   Both the H1_serial_config() function and the H1_serial_write() function
 *      make use of bit-wise AND (&) operators. Below is the AND operator used
 *      by the H1_serial_config() function:
 * 
        TRISC = TRISC & 0b11111110;
 * 
 *      Can you determine what this statement is doing, and why an AND operation
 *      is being used instead of just over-writing the TRISC value? Explain.
 * 
 * 10.  The data transmission loop of the H1_serial_write() function is shown
 *      below:
 * 
    // Shift 8 data bits out LSB first
    for(unsigned char bits = 8; bits != 0; bits--)
    {
        if((data & 0b00000001) == 0)    // Check the least significant bit state
        {
            H1OUT = 0;
        }
        else
        {
            H1OUT = 1;
        }
        __delay_us(103);    // Shorter delay to account for 'for' loop overhead
        data = data >> 1;   // Prepare next bit by shifting data right 1 bit pos
    }
 *  
 *      Explain how the AND operation used in the if condition can determine
 *      the state of the least significant data bit.
 * 
 *      After the least significant data bit is transmitted, how are the other
 *      bits in the data byte isolated and transmitted?
 *      
 * 
 * Programming Activities
 * 
 * 1.   The bin_to_dec() function converts a single byte into three decimal
 *      digits. Step 8 in the program analysis activities, above, converted
 *      each of these numeric digits to the ASCII code representing the digit.
 *      Can you make a bin_to_ASCII() function that would eliminate the need
 *      for you to offset the values to ASCII separately, as was done here?
 * 
 * 2.   Does your UBMP4 have Q1 installed? If installed, it will be easier to
 *      use Q1 as an input device instead of the temperature module. Q1 can
 *      either be an ambient light sensor, which is sensitive to visible light,
 *      or a phototransistor, sensitive to infrared (IR) wavelengths. Try using
 *      your phone's flashlight (ambient light sensor) or an infrared LED to
 *      illuminate Q1.
 * 
 *      Create a program to implement threshold detection. Determine the analog
 *      level when Q1 senses low light and again when Q1 senses bright light.
 *      Use conditional code to light an LED when the Q1 light level rises or
 *      falls beyond a threshold set in your program.
 * 
 * 3.   Create a program that produces a PWM output proportional to an analog
 *      input, or a program that creates a tone having a pitch proportional
 *      to an analog input value.
 * 
 * 4.   If you have an oscilloscope available, investigate how fast you get the
 *      serial output to transmit. Try setting the bit delays inside the serial
 *      write function to 1 microsecond of delay, instead of 104 and 103. Does
 *      it work the way it should? Is each bit 1us long? What do you think is
 *      happening?
 * 
 * 5.   Creating a function to transmit serial data is relatively straight-
 *      forward. Receiving serial data reliably is similar, but should take
 *      into account slight timing differences between the transmitter and 
 *      receiver. Think about how you would create a function to receive serial
 *      data. How could you ensure that the data is received correctly even if
 *      there are slight timing differences between the transmitting and
 *      receiving devices? Create the pseudocode or a flowchart describing a
 *      serial receive function.
 * 
 * 6.   Transmitting serial data involves isolating each bit of a variable to
 *      be sent using bit-wise logical operators, and bit-shifting the remaining
 *      data from within a loop. Received serial data can be assembled into an
 *      8-bit variable using a similar technique. Try to create a function or
 *      just the main loop of code that would successively read serial data
 *      from an input pin and assemble the values into an 8-bit variable.
 */
