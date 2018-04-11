/*
    Basic 433 MHz RF Communication Library tested on Arduino Uno.

    Author: Mert Alp Taytak (mertalptaytak@gmail.com)

    Currently it sends and receives predefined amount of bits.

    Default pins:
    - Receiver pin:    Digital Pin 11
    - Transmitter pin: Digital Pin 12
    * Transmitter pin is used with port manipulation, so it is hardcoded
    into the library. Please look at TX_TOGGLE_STATE function to change it.

    Default communication specifications:
    - Message length: 8 bits
    - Transmission speed: 100 bits per second
    * I am hoping to increase these in the future.

    Bit encoding logic:
        There is a predefined BIT_PERIOD.
    ---__ is the waveform of bit 1, where HIGH pulse is 0.6 length of BIT_PERIOD
    --___ is the waveform of bit 0, where HIGH pulse is 0.4 length of BIT_PERIOD

    Bit decoding logic:
        There are predefined lower and upper bounds for BIT_ONE and BIT_ZERO
    which are calculated according to BIT_PERIOD and encoding logic.
        Pulse read is then checked with these bounds to determine if it is a
    valid bit.
*/

#ifndef __DELPHI_H_
#define __DELPHI_H_

// INCLUDES

#include "Arduino.h"

// DEFINITIONS and MACROS

// None.

// GENERAL FUNCTIONS

/*
    Default configuration:
    - Receiver pin:    Digital Pin 11, INPUT
    - Transmitter pin: Digital Pin 12, OUTPUT
*/
void DELPHI_SET_DEFAULTS();

// RECEIVER FUNCTIONS

/*
    Takes input value, compares with bit encoded pulse lengths
    Returns 0 if bit is 0, 1 if bit is 1, 255 if else
*/
uint8_t RX_DECODE_SIGNAL(unsigned long pulseLength);

/*
    Takes input uint8_t array,
    Listens to pulses for predefined amount of bit periods
    If it receives valid pulses meanwhile it is written into array
*/
void RX_LISTEN(uint8_t arr[]);


// TRANSMITTER FUNCTIONS

/*
    Writes LOW to transmitter pin if state is 0
    Writes HIGH to transmitter pin if state is not 0
    It is equivalent to digitalWrite(),
    except it is hardcoded to change
    transmitter pin only for faster state change.
*/
void TX_TOGGLE_STATE(uint8_t state);

/*
    This function is used before transmitting a message
    to clear noise from the receiver module.
    It sends very short 1-0 pulses repeatedly.
*/
void TX_SEND_TRAINER();

/*
    Takes input uint8_t array of predefined size,
    Sends bit encoded 0 if element at index is 0,
    Sends bit encoded 1 if element at index is not 0.
*/
void TX_SEND(uint8_t *arr);

#endif // End of define Delphi.h
