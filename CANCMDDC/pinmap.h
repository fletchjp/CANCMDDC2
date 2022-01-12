/// file@ pinmap.h
/// brief@ to hold the pin map for the Mega
///

/*
 This is copied from CANCMDDC and needs updating. Yes it does.
 This is particularly the case as it will be different for different hardware options.
 Pin Use map:
 Digital pin 2 (PWM)    PWM0  H1a       
 Digital pin 3 (PWM)    PWM1  H1b
 Digital pin 4 (PWM)    Enable  Buzzer
 Digital pin 5 (PWM)    PWM2  H2a
 Digital pin 6 (PWM)    PWM3  H2b
 Digital pin 7 (PWM)    PWM4  H3a
 Digital pin 8 (PWM)    PWM5  H3b
 Digital pin 9 (PWM)    Encoder 7 S unused
 Digital pin 10 (PWM)   Mode  unused - pull low for CANCMD master
 Digital pin 11 (PWM)   PWM6  unused - H-bridge 4a
 Digital pin 12 (PWM)   PWM7  unused - H-bridge 4b
 Digital pin 13 (PWM)       LED (onboard)
 Digital pin 14 (TX3)   EnableA unused - H-bridge 4a
 Digital pin 15 (RX3)   EnableB unused - H-bridge 4a
 Digital pin 16 (TX2)   EnableA unused - H-bridge 4b
 Digital pin 17 (RX2)   EnableB unused - H-bridge 4b
 Digital pin 18 (TX1)   Int'upt CAN
 Digital pin 19 (RX1)   
 Digital pin 20 (SDA)   SDA   LCD
 Digital pin 21 (SCL)   SCL   LCD
 Digital pin 22       EnableA H1a
 Digital pin 23       EnableB H1a
 Digital pin 24       EnableA H1b
 Digital pin 25       EnableB H1b
 Digital pin 26       EnableA H2a
 Digital pin 27       EnableB H2a
 Digital pin 28       EnableA H2b
 Digital pin 29       EnableB H2b
 Digital pin 30       EnableA H3a
 Digital pin 31       EnableB H3a
 Digital pin 32       EnableA H3b
 Digital pin 33       EnableB H3b
 Digital pin 34       LED Green  - unused
 Digital pin 35       LED Yellow - unused - extra pin for 4 by 4 keypad (c0)
 Digital pin 36       Encoder 8 Switch
                      The following are for the 4 by 3 Keypad
 Digital pin 37       c0    Keypad - uses odd-numbered pins only so that the 7-way header plugs straight in
 Digital pin 38             Encoder 1 Switch
 Digital pin 39       c1    Keypad
 Digital pin 40             Encoder 2 Switch
 Digital pin 41       c2    Keypad
 Digital pin 42             Encoder 3 Switch
 Digital pin 43       r0    Keypad
 Digital pin 44             Encoder 4 Switch
 Digital pin 45 (PWM)   r1    Keypad
 Digital pin 46             Encoder 5 Switch
 Digital pin 47       r2    Keypad
 Digital pin 48             Encoder 6 Switch
 Digital pin 49       r3    Keypad
 Digital pin 50 (MISO)    SO    CAN
 Digital pin 51 (MOSI)    SI    CAN
 Digital pin 52 (SCK)     Sck   CAN
 Digital pin 53 (SS)      CS    CAN
 Digital / Analog pin 0     Encoder 1 A
 Digital / Analog pin 1     Encoder 2 A
 Digital / Analog pin 2     Encoder 3 A
 Digital / Analog pin 3     Encoder 4 A
 Digital / Analog pin 4     Encoder 5 A
 Digital / Analog pin 5     Encoder 6 A
 Digital / Analog pin 6     
 Digital / Analog pin 7     Hall Effect Input
 Digital / Analog pin 8     Encoder 1 B
 Digital / Analog pin 9     Encoder 2 B
 Digital / Analog pin 10    Encoder 3 B
 Digital / Analog pin 11    Encoder 4 B
 Digital / Analog pin 12    Encoder 5 B
 Digital / Analog pin 13    Encoder 6 B
 Digital / Analog pin 14    Encoder 7 B - unused
 Digital / Analog pin 15    Encoder 8 B - unused
*/
