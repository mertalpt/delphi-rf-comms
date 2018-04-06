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

// INCLUDES
#include "Arduino.h"
#include "Delphi.h"

// MACROS AND DEFINITIONS

// Predefined message length is 16 bits
#define MSG_LENGTH 16

/*
    Defining RX and TX pins, because I am using some standard functions
    It may change in later versions
*/
#define RX_PIN 13
#define TX_PIN 12

/*
    Pulse waveform for 1 is: ---__
    Pulse waveform for 0 is: --___
    This is done to balance time spent at LOW and HIGH

    When reading the pulse,
    tolerance modifier should be less than 0.2 to avoid mixing up signals

    Workings of the tolerance:
    Each pulse will be checked with (1-tolerance)*definedLength < pulse < (1+tolerance)*definedLength
    where definedLength is lengths of ONE and ZERO pulses to see whether pulse read is a ONE, ZERO or undefined
*/
#define PULSE_FULL_LENGTH 1000 // microseconds
#define PULSE_ONE_LENGTH   600 // Transmitter will be HIGH for 600 microseconds
#define PULSE_ZERO_LENGTH  400 // Transmitter will be HIGH for 400 microseconds
#define PULSE_READ_TOLERANCE 0.15

// Checks whether pulse read is a ONE pulse
#define pulseIsOne(PULSE_LENGTH) ( ((PULSE_ONE_LENGTH * (1 - PULSE_READ_TOLERANCE) < PULSE_LENGTH) &&
                                    (PULSE_LENGTH < PULSE_ONE_LENGTH * (1 + PULSE_READ_TOLERANCE))) ? 1 : 0 )

// Checks whether pulse read is a ZERO pulse
#define pulseIsZero(PULSE_LENGTH) ( ((PULSE_ZERO_LENGTH * (1 - PULSE_READ_TOLERANCE) < PULSE_LENGTH) &&
                                     (PULSE_LENGTH < PULSE_ZERO_LENGTH * (1 + PULSE_READ_TOLERANCE))) ? 1 : 0 )

// FUNCTIONS

/*
    Call before using DELPHI, preferably in setup
    Sets correct I/O to RX and TX pins
*/
void DELPHI_SET_DEFAULTS()
{
    pinMode(RX_PIN, INPUT);
    pinMode(TX_PIN, OUTPUT);
}

/*
    Transmits 16 bits given in the input uint8_t array
    Zero is transmitted as 0, anything else is transmitted as 1
*/
void TX_MSG(uint8_t message[])
{
    // Disable and save interrupts
    uint8_t oldSREG = SREG;
    cli();

    // Initialize counter
    uint8_t index = 0;

    // Read and send message
    for (; index < MSG_LENGTH; index++)
    {
        if (message[index] == 0)
            TX_PULSE_ZERO();
        else
            TX_PULSE_ONE();
    }

    // Restore interrupt
    SREG = oldSREG;
}

/*
    Listens to transmissions on the receiver pin until it receives a signal
    After the first bit is received,
    if the message is not fully transmitted in the following 20 bit period,
    fills rest of the return message with -1(= 255 due to overflow)

    Returns the message in the input array
*/
void RX_MSG(uint8_t message[])
{
    // Disable and save interrupts
    uint8_t oldSREG = SREG;
    cli();

    // Initialize counter
    uint8_t index = 1;

    // Declare variable to hold pulse lengths
    unsigned long pulseLength;

    // Assume whole message is invalid first
    for (uint8_t i = 0; i < MSG_LENGTH; i++)
        message[i] = -1;

    // Get first valid pulse
    do
    {
        pulseLength = RX_GET_PULSE(0);

        if (pulseIsOne(pulseLength))
            message[0] = 1;
        else if (pulseIsZero(pulseLength))
            message[0] = 0;
    } while (message[0] == -1);

    // Get starting time
    unsigned long sTime = micros();

    // Listen loop
    do
    {
        // Avoid going out of bounds
        if (index == MSG_LENGTH)
            break;

        pulseLength = RX_GET_PULSE(PULSE_FULL_LENGTH);

        if (pulseIsOne(pulseLength))
            message[index++] = 1;
        else if (pulseIsZero(pulseLength))
            message[index++] = 0;

    } while(micros() - sTime < 20 * PULSE_FULL_LENGTH);

    // Restore interrupts
    SREG = oldSREG;
}

/*
    Sends a ONE-ENCODED pulse on the transmitter pin for the hardcoded duration
    Pulse length is in microseconds
    Maximum length is 65535 microseconds by the limitation of 16 bits
*/
void TX_PULSE_ONE()
{
    // Save and disable interrupt in TX_MSG

    // Set TX to HIGH, TX_PIN = 12
    PortB |= B00010000;

    // Wait predefined length of time
    delayMicroseconds(PULSE_ONE_LENGTH);

    // Set TX to LOW, TX_PIN = 12
    PortB &= B11101111;

    // Wait predefined length of time
    delayMicroseconds(PULSE_FULL_LENGTH - PULSE_ONE_LENGTH);

    // Restore interrupt in TX_MSG
}

/*
    Sends a ZERO-ENCODED pulse on the transmitter pin for the hardcoded duration
    Pulse length is in microseconds
    Maximum length is 65535 microseconds by the limitation of 16 bits
*/
void TX_PULSE_ZERO()
{
    // Save and disable interrupt in TX_MSG

    // Set TX to HIGH, TX_PIN = 12
    PortB |= B00010000;

    // Wait predefined length of time
    delayMicroseconds(PULSE_ZERO_LENGTH);

    // Set TX to LOW, TX_PIN = 12
    PortB &= B11101111;

    // Wait predefined length of time
    delayMicroseconds(PULSE_FULL_LENGTH - PULSE_ZERO_LENGTH);

    // Restore interrupt in TX_MSG
}

/*
    ***Currently using library functions until I find a faster solution***

    Waits until timeOut microseconds on the receiver pin to turn HIGH
    If timeOut == 0, there is no time out
    If times out, returns 0
    Else, returns the duration of the pulse in microseconds
*/
unsigned long RX_GET_PULSE(uint16_t timeOut)
{
    if (timeOut == 0)
        return pulseIn(RX_PIN, HIGH);

    // Else
    return pulseIn(RX_PIN, HIGH, timeOut);
}
