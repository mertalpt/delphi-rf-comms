/*
    Basic 433 MHz RF Communication Library for Arduino
    Currently working on sending bitstreams only
    Fancy features will/may be added later

    IMPORTANT:
    - Designed and tested on Arduino UNO, may not work on other boards
    - Default transmitter pin is digital pin 12, do not use this pin for anything else
    - Default receiver pin is digital pin 13, do not use this pin for anything else

    Author: Mert Alp Taytak
*/

#ifndef __DELPHI_H_
#define __DELPHI_H_

// INCLUDES
#include "Arduino.h"

// MACROS and DEFINITIONS

/*
    These are only to show default values, do not comment out
    Values are hardcoded in the functions
    #define TX_PIN = 12;     // Default transmitter pin
    #define RX_PIN = 13;     // Default receiver pin
    #define MSG_LENGTH = 16; // Default message length
*/

// FUNCTIONS

/*
    Call before using DELPHI, preferably in setup
    Sets correct I/O to RX and TX pins
*/
void DELPHI_SET_DEFAULTS();

/*
    Transmits 16 bits given in the input uint8_t array
    Zero is transmitted as 0, anything else is transmitted as 1
*/
void TX_MSG(uint8_t message[]);

/*
    Listens to transmissions on the receiver pin until it receives a signal
    After the first bit is received,
    if the message is not fully transmitted in the following 20 bit period,
    fills rest of the return message with -1(= 255 due to overflow)

    Returns the message in the input array
*/
void RX_MSG(uint8_t message[]);

/*
    Sends a HIGH pulse on the transmitter pin for the hardcoded duration
    Pulse length is in microseconds
    Maximum length is 65535 microseconds by the limitation of 16 bits
*/
void TX_PULSE_ONE();

/*
    Sends a LOW pulse on the transmitter pin for the hardcoded duration
    Pulse length is in microseconds
    Maximum length is 65535 microseconds by the limitation of 16 bits
*/
void TX_PULSE_ZERO();

/*
    ***Currently using library functions until I find a faster solution***

    Waits until timeOut microseconds on the receiver pin to turn HIGH
    If times out, returns 0
    Else, returns the duration of the pulse in microseconds
*/
unsigned long RX_GET_PULSE(uint16_t timeOut);

#endif // End of header definition
