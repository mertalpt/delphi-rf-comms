# Delphi RF Communication Library

  Basic 433 MHz RF Communication Library tested on Arduino Uno.

> Author: Mert Alp Taytak (mertalptaytak@gmail.com)

Currently it sends and receives predefined amount of bits.

  Default pins:

- Receiver pin:    Digital Pin 11
- Transmitter pin: Digital Pin 12
    
\* Transmitter pin is used with port manipulation, so it is hardcoded
into the library. Please look at TX_TOGGLE_STATE function to change it.

  Default communication specifications:

- Message length: 8 bits
- Transmission speed: 100 bits per second
    
\* I am hoping to increase these in the future.

  Bit encoding logic:

There is a predefined BIT_PERIOD.

---__ is the waveform of bit 1, where HIGH pulse is 0.6 length of BIT_PERIOD

--___ is the waveform of bit 0, where HIGH pulse is 0.4 length of BIT_PERIOD

  Bit decoding logic:

There are predefined lower and upper bounds for BIT_ONE and BIT_ZERO
which are calculated according to BIT_PERIOD and encoding logic.

Pulse read is then checked with these bounds to determine if it is a
valid bit.
