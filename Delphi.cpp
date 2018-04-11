/*
    Implementation of Delphi Library

    Look at the header file for details.

    ABOUT THE COMMENTED OUT CODE IN TX_SEND and RX_LISTEN:
    It seems that disabling interrupts interfere with serial communication
    of the Arduino board. Also, time out logic is too slow to work currently.
    I will be looking into solutions.
*/

// INCLUDES

#include "Delphi.h"

// DEFINITIONS and MACROS

#define RX_PIN 11
#define TX_PIN 12

#define MESSAGE_LENGTH        8 // bits

#define BIT_PERIOD        10000 // microseconds
#define BIT_ONE_MODIFIER    0.6 // 60 percent of full pulse is HIGH for bit 1
#define BIT_ZERO_MODIFIER   0.4 // 40 percent of full pulse is HIGH for bit 0
#define BIT_TOLERANCE      0.10 // Should be <0.2 with current bit modifiers
#define TRAINER_PERIOD       50 // microseconds
#define TRAINER_REPEATS      40 // times
//#define NOISE_TOLERANCE       0 // noise signals between valid signals are tolerated

// GENERAL FUNCTIONS

/*
    Default configuration:
    - Receiver pin:    Digital Pin 11, INPUT
    - Transmitter pin: Digital Pin 12, OUTPUT
*/
void DELPHI_SET_DEFAULTS()
{
    // Performance is not important in this case
    pinMode(RX_PIN, INPUT);
    pinMode(TX_PIN, OUTPUT);
    digitalWrite(TX_PIN, LOW);
}

// RECEIVER FUNCTIONS

/*
    Takes input value, compares with bit encoded pulse lengths
    Returns 0 if bit is 0, 1 if bit is 1, 255 if else
*/
uint8_t RX_DECODE_SIGNAL(unsigned long pulseLength)
{
    if (((1 - BIT_TOLERANCE) * BIT_ZERO_MODIFIER * BIT_PERIOD < pulseLength) && ((1 + BIT_TOLERANCE) * BIT_ZERO_MODIFIER * BIT_PERIOD > pulseLength))
    {
        return 0;
    }
    if (((1 - BIT_TOLERANCE) * BIT_ONE_MODIFIER * BIT_PERIOD < pulseLength) && ((1 + BIT_TOLERANCE) * BIT_ONE_MODIFIER * BIT_PERIOD > pulseLength))
    {
        return 1;
    }

    // If pulse is not valid
    return 255;
}

/*
    Takes input uint8_t array,
    Listens to pulses for predefined amount of bit periods
    If it receives valid pulses meanwhile it is written into array
*/
void RX_LISTEN(uint8_t arr[])
{
    // Disable and save interrupts
    //uint8_t oldSREG = SREG;
    //cli();

    // Initialize counters
    uint16_t counter = 0;
    //uint8_t noiseCounter = 0;

    // Variable to hold pulse readings
    unsigned long pulse;
    uint8_t pulseDecoded;

    // Calculate timeout
    //unsigned long timeout = micros() + (MESSAGE_LENGTH * BIT_PERIOD);

    // Read pulses
    while ((counter < MESSAGE_LENGTH) /*&& (micros() < timeout)*/)
    {
        // Reset noise counter
        //noiseCounter = 0;

        do
        {
            // pulseIn is a standart Arduino function
            pulse = pulseIn(RX_PIN, HIGH/*, BIT_PERIOD*/);

            // Decode pulse
            pulseDecoded = RX_DECODE_SIGNAL(pulse);

            // Increment noise counter if pulse is noise
            //if (pulseDecoded == 255)
            //    noiseCounter++;

        } while (pulseDecoded == 255); // Repeat until you get a valid signal

        // There is a valid reading and it did not come from random noise
        //if (noiseCounter <= NOISE_TOLERANCE)
            arr[counter++] = pulseDecoded;
    }

    // If there was a timeout, fill the rest of the array with 255
    for (; counter < MESSAGE_LENGTH; counter++)
    {
        arr[counter] = 255;
    }

    // Restore interrupts
    //SREG = oldSREG;
}


// TRANSMITTER FUNCTIONS

/*
    Writes LOW to transmitter pin if state is 0
    Writes HIGH to transmitter pin if state is not 0
    It is equivalent to digitalWrite(),
    except it is hardcoded to change
    transmitter pin only for faster state change.
*/
void TX_TOGGLE_STATE(uint8_t state)
{
    // Port manipulation, TX_PIN = 12 is in PORTB
    if (state == 0)
    {
        // Write 0 to TX_PIN, do not touch anything else
        PORTB &= B11101111;
    }
    else
    {
        // Write 1 to TX_PIN, do not touch anything else
        PORTB |= B00010000;
    }
}

/*
    This function is used before transmitting a message
    to clear noise from the receiver module.
    It sends very short 1-0 pulses repeatedly.
*/
void TX_SEND_TRAINER()
{
    for (uint8_t counter = 0; counter < TRAINER_REPEATS; counter++)
    {
        TX_TOGGLE_STATE(1);
        delayMicroseconds(TRAINER_PERIOD / 2);
        TX_TOGGLE_STATE(0);
        delayMicroseconds(TRAINER_PERIOD / 2);
    }
}

/*
    Takes input uint8_t array of predefined size,
    Sends bit encoded 0 if element at index is 0,
    Sends bit encoded 1 if element at index is not 0.
*/
void TX_SEND(uint8_t *arr)
{
    // Disable and save interrupts
    //uint8_t oldSREG = SREG;
    //cli();

    // Send trainer
    TX_SEND_TRAINER();

    // Send message
    for (uint8_t index = 0; index < MESSAGE_LENGTH; index++)
    {
        if (arr[index] == 0) // Send 0
        {
            TX_TOGGLE_STATE(1);
            delayMicroseconds(BIT_PERIOD * BIT_ZERO_MODIFIER);
            TX_TOGGLE_STATE(0);
            delayMicroseconds(BIT_PERIOD * (1 - BIT_ZERO_MODIFIER));
        }
        else // Send 1
        {
            TX_TOGGLE_STATE(1);
            delayMicroseconds(BIT_PERIOD * BIT_ONE_MODIFIER);
            TX_TOGGLE_STATE(0);
            delayMicroseconds(BIT_PERIOD * (1 - BIT_ONE_MODIFIER));
        }
    }

    // Send trainer
    TX_SEND_TRAINER();

    // Restore interrupts
    //SREG = oldSREG;
}
