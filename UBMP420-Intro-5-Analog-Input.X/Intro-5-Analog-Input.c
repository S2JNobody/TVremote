/*==============================================================================
 Project: TV-Remote control transmitter - Project starter code
 Date:    June 8, 2022
 
 This program provides basic functions and a program outline to start a TV
 remote control transmitter programming project. 
==============================================================================*/

#include    "xc.h"              // Microchip XC8 compiler include file
#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include    "UBMP420.h"         // Include UBMP4.2 constant and function definitions

// TODO Set linker ROM ranges to 'default,-0-7FF' under "Memory model" pull-down.
// TODO Set linker code offset to '800' under "Additional options" pull-down.

#define pressed 0
#define notPressed 1

// Sony IR command/data codes
#define SONY_VOLUP  0x12        // Volume up button command code
#define	SONY_VOLDN  0x13        // Volume down button command code
#define SONY_POWER  0x15        // Power button command code

#define SHARP_POWER     0b01101000
#define SHARP_ENTER     0b00000000 //TODO: Get the code
#define SHARP_UP        0b11101010
#define SHARP_RIGHT     0b01101111
#define SHARP_DOWN      0b00011010
#define SHARP_LEFT      0b10101111
#define SHARP_VOLUP     0b00101000
#define SHARP_VOLDOWN   0b10101000
#define SHARP_CHUP      0b00000000 //TODO: Get the code
#define SHARP_CHDOWN    0b00000000 //TODO: Get the code
#define SHARP_MENU      0b00000100

// Sony IR device/address codes
#define SONY_TV     0x01        // Sony TV device address code
#define SHARP_TV    0b10000     // Sharp TV address code

// Sony IR protocol timing and definitions
#define SONY_START_PULSES   96  // Number of 40kHz IR pulses to make Start bit
#define SONY_ZERO_PULSES    24  // Number of IR pulses to make 'zero' bit
#define SONY_ONE_PULSES     48  // Number of IR pulses to make 'one' bit
#define SONY_BIT_DELAY  600     // Delay between bits in microseconds
#define SONY_DATABITS   7       // Number of data/command bits to send
#define SONY_ADDRBITS   5       // Number of address/device bits to send


#define SHARP_PULSES 12 //Number of IR pulses for either bit. Technically 12.16 pulses to make 320us
#define SHARP_ZERO_BIT_DELAY 680 //Delay between bits for a zero: 680us
#define SHARP_ONE_BIT_DELAY 1680 //Delay between bits for a one: 1680us
#define SHARP_DATABITS   8       // Number of data/command bits to send
#define SHARP_ADDRBITS   5       // Number of address/device bits to send


// Program variable definitions
unsigned char bits;             // Bit counter variable for bit transmission

//Order of switches in here: SW2, SW3...
bool SwitchesPressed[4] = {0, 0, 0, 0};

void pulse_40kHz(unsigned int pulses)
{
    for(pulses; pulses != 0; pulses --)
    {
        IRLED = 1;
        __delay_us(12);
        IRLED = 0;
        __delay_us(11);
    }
}

void pulse_38kHz(unsigned int pulses)
{
    for(pulses; pulses != 0; pulses--)
    {
        IRLED = 1;
        _delay(156);
        IRLED = 0;
        _delay(143);
    }
}

//EXP is always 1, CHK is always 0
void transmit_SHARP(unsigned char address, unsigned char command)
{
    // Save !(address), !(command)
    
    // Transmit address
    for(bits = SHARP_ADDRBITS; bits != 0; bits--)
    {
        pulse_38kHz(SHARP_PULSES);
        if((address & 0b10000) == 0)
        {
            __delay_us(SHARP_ZERO_BIT_DELAY);
        }
        else
        {
            __delay_us(SHARP_ONE_BIT_DELAY);
        }
        address = address << 1;
    }
    // Transmit !(address)
    
    // Transmit command
    for(bits = SHARP_DATABITS; bits != 0; bits--)
    {
        pulse_38kHz(SHARP_PULSES);
        if((command & 0b10000000) == 0)
        {
            __delay_us(SHARP_ZERO_BIT_DELAY);
        }
        else
        {
            __delay_us(SHARP_ONE_BIT_DELAY);
        }
        command = command << 1;
    }

    // Transmit !(command)

    //Transmit EXP (Always 1 uninverted)
    pulse_38kHz(SHARP_PULSES);
    __delay_us(SHARP_ONE_BIT_DELAY);

    //Transmit CHK (Always 0 uninverted)
    pulse_38kHz(SHARP_PULSES);
    __delay_us(SHARP_ZERO_BIT_DELAY);

    pulse_38kHz(SHARP_PULSES);

    //Delay between uninverted and inverted signal
    __delay_ms(40);

    //Now do it with the command, EXP, and CHK inverted
    
    // Transmit address
    for(bits = SHARP_ADDRBITS; bits != 0; bits--)
    {
        pulse_38kHz(SHARP_PULSES);
        if((address & 0b10000) == 0)
        {
            __delay_us(SHARP_ZERO_BIT_DELAY);
        }
        else
        {
            __delay_us(SHARP_ONE_BIT_DELAY);
        }
        address = address << 1;
    }

    // Transmit command inverted
    for(bits = SHARP_DATABITS; bits != 0; bits--)
    {
        pulse_38kHz(SHARP_PULSES);
        if((command & 0b10000000) == 0)
        {
            __delay_us(SHARP_ONE_BIT_DELAY);
        }
        else
        {
            __delay_us(SHARP_ZERO_BIT_DELAY);
        }
        command = command << 1;
    }

    //Transmit EXP inverted (Always 0 inverted)
    pulse_38kHz(SHARP_PULSES);
    __delay_us(SHARP_ZERO_BIT_DELAY);

    //Transmit CHK (Always 1 inverted)
    pulse_38kHz(SHARP_PULSES);
    __delay_us(SHARP_ONE_BIT_DELAY);

    pulse_38kHz(SHARP_PULSES);

}

int main(void)
{
    OSC_config();               // Configure internal oscillator for 48 MHz
    UBMP4_config();             // Configure on-board UBMP4 I/O devices

    while(1)
	{
        /*
        pulse_38kHz(SHARP_PULSES);
        __delay_us(SHARP_ONE_BIT_DELAY);
        pulse_38kHz(SHARP_PULSES);
        __delay_us(SHARP_ZERO_BIT_DELAY);
        */

        // Read pushbuttons
        /*if (SW2 == pressed) {
            transmit_SHARP(SHARP_TV, SHARP_POWER);
        }*/

        if (SW2 == pressed && SwitchesPressed[0] == 0) {
            SwitchesPressed[0] = 1;
            //Power button
            transmit_SHARP(SHARP_TV, SHARP_POWER);
        } else if (SW2 == notPressed) {
            SwitchesPressed[0] = 0;
        }

        if (SW3 == pressed && SwitchesPressed[1] == 0) {
            SwitchesPressed[1] = 1;
            //VOLUP button
            transmit_SHARP(SHARP_TV, SHARP_VOLUP);
        } else if (SW3 == notPressed) {
            SwitchesPressed[1] = 0;
        }

        if (SW4 == pressed && SwitchesPressed[2] == 0) {
            SwitchesPressed[2] = 1;
            //VOLDOWN button
            transmit_SHARP(SHARP_TV, SHARP_VOLDOWN);
        } else if (SW4 == notPressed) {
            SwitchesPressed[2] = 0;
        }

        if (SW5 == pressed && SwitchesPressed[3] == 0) {
            SwitchesPressed[3] = 1;
            //MENU button
            transmit_SHARP(SHARP_TV, SHARP_MENU);
        } else if (SW5 == notPressed) {
            SwitchesPressed[3] = 0;
        }

        // Is it a new button press? Transmit address and command for button.
        
        // Is button a repeat press? Transmit repeat code - protocol dependent.
        
        // Delay between button presses (adjust based on protocol)
        __delay_ms(20);
        
        // Activate bootloader if SW1 is pressed.
        if(SW1 == 0)
        {
            RESET();
        }
    }
}

/* Learn More - Program Analysis Activities
 * 
 * 1. Modify the UBMP4_config() function to allow output on the IR led pin. Use
*     the test code and an oscilloscope to measure the width of the high and
 *    low pulses.
 * 
 * 2. Why are the delays in the pulse_40kHz function different by 1us, yet the
 *    pulses are similar in length?
 * 
 * 3. Adjust the delays in the pulse_40kHz() function to make the high and low
 *    pulses as close to 25us as possible. Add NOP(); single cycle delays, or
 *    replace __delay_us() functions with _delay() clock cycle functions to
 *    modify the pulse high and low period delays.
 * 
 * 4. Determine the number of pulses required to create a start bit, a zero bit,
 *    and a one bit for the protocol that your remote control uses.
 * 
 * 5. Complete the transmit_Sony() function by adding the code to transmit the
 *    5-bit device address code after the command code.
 *
 * 6. Create the definitions and the functions required to transmit the protocol
 *    used by your IR device.
 */
