   #define VERSION 2.9
////////////////////////////////////////////////////////////////////////////
// CANCMDDC Main Branch retaining H Bridge control.
////////////////////////////////////////////////////////////////////////////
// CANCMDDC_V2a Beta 9
// Ideas for using IO Abstraction library for task scheduling.
//
// https://www.thecoderscorner.com/products/arduino-libraries/io-abstraction/
//
// The only other example working is CANTEXT2
// I will also use Martin Da Costa's modifications e.g. no defs.h file.
// First version put together for testing.
// At the moment I have no way to turn off the buzzer if a power alarm happens.
///////////////////////////////////////////////////////////////////////////////
// CANCMDDC_V2a Beta 8
// Adding the KEYPAD and ENCODER code
// I have missed out 2.7 as I think someone is already using it.
// These may not be complete - leave out for now.
//////////////////////////////////////////////////////////////////////////////////////
// CANCMDDC_V2a Beta 6
// 12th November 2020
// Correcting some bugs and starting to tidy up the code.
// The changes are in eventhandler.
// I have taken out all the code associated with the old version of the code structure there.
// Changed trainController.h following Dave Radcliffe for initial state of the loco.
//////////////////////////////////////////////////////////////////////////////////////
// CANCMDDC_V2a Beta 5
// Adding code to explore request operations.
// Changing to use switch based on opcode in eventhandler
// Testing use of short events to poll the signal.
//////////////////////////////////////////////////////////////////////////////////////
// CANCMDDC_V2a Beta 4
// Adding code to send events with OPC_ACONx, OPC_ACOFx with x = 1,2,3
// Sent if CBUS_EVENTS is 1
// Events now defined in a table.
// Question: I have set up a message about stopping all. When should I cancel it?
// I have put a cancel into the setup loop.
// I need something else as well and have not sorted that out.
// For the moment I have set up hand cancellation using the push button.
//////////////////////////////////////////////////////////////////////////////////////
// CANCMDDC_V2.3d
// Experiments to look for source of intermittent operation.
// Currently 2 outputs.
//////////////////////////////////////////////////////////////////////////////////////
// CANCMDDC_V2.3c
// Modified for L298N output boards
//////////////////////////////////////////////////////////////////////////////////////
// CANCMDDC_V2.3b
//
// Motor shield power connection and Hall Effect connected
//
//////////////////////////////////////////////////////////////////////////////////////
// CANCMDDC_V2.3a
//
// This is a new major version as I am starting to implement the motor controls.
// I first needed to modify the message processing code to get the speed updated correctly.
// This is now happening as the update code is in a function called as needed.
//
// I now need to sort out the Arduino pins for the motor shield.
// Some of the pins in use for other things may need to be changed.
//
//////////////////////////////////////////////////////////////////////////////////////
// CANCMDDC_V2.2h
// Adding more things including the Hall Effect current detection code from Ian Hogg.
// Also adding a lot more of the code from CANCMDDC V1.10 for the displays.
// As I now have a 4 by 20 display I will remove code for the 2 by 16.
//
// CANCMDDC_V2.2g adding 2 x 16 LCD Display not completed.
// I have put in strings for the error codes
//
// CANCMDDC_V2.2f exploring allocation of locos.
//
// CANCMDDC_V2_2e using CBUSBUZZER library optionally
// I am going to make this optional as I do not want to have code that others cannot use
// without the CBUSBUZZER library - until Duncan has decided what to do about it.
// I also want to go on to add the motor control code and don't want to have two versions of that code.
// I have started to add motor control by changing the pin numbers for the LINKSPRITE version.
//
// CANCMDDC_V2_2c development version to work with a CANCAB
// This is the first version with a fix for the code error in the sendMessage routine.
//
// V2.2 The main change is to extend the opcodes array.
//
// CANCMDDC_V2_1a adapted from 2_0m using information from David Ratcliffe's CANCMDDC V1.10
//
// Version being configured for the Arduino MEGA with some other
// changed settings.

// First experiment to turn CBUS_1in1out2 into a CANCMDDC using the Arduino CBUS Library

// I am not going to include any display or the keypad at this stage.

// Copyright (c) 2019 John Fletcher
// I have included copyrights from CANCMDDC_v1_9
// and Duncan Greenwood for the Arduino CBUS Library and example code.

/**
 * CANCMDDC - A DC "command station" for use with MERG CBUS systems
 * Copyright (c) 2015 Mark Riddoch
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License
 */

/*
 * This sketch makes use of the MCP_CAN library, modified to accept
 * either an 8MHz clock or a 16MHz clock.
 * Hardware requires are an MCP2515 based CBUS interface and a set
 * of H-Bridges that can be used to drive the DC tracks.
 */
 
/*
 * Updated and extended by Ian Morgan.
 * 
 *  Now uses hardware interrupt to receive incoming CBus messages into a FIFO buffer for later processing.
 *  Also uses a 100Hz timer interrupt to control LED, buzzer and accelleration and deccelleration.
 *  This sketch creates one instance of the FIFO buffer class, and 5 instances for the train controller class.
 */
// NOTE: Interrupt handling may be different - notes to be ammended later.
/*
  Copyright (C) Duncan Greenwood 2017 (duncan_greenwood@hotmail.com)

  This work is licensed under the:
      Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
   To view a copy of this license, visit:
      http://creativecommons.org/licenses/by-nc-sa/4.0/
   or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

   License summary:
    You are free to:
      Share, copy and redistribute the material in any medium or format
      Adapt, remix, transform, and build upon the material

    The licensor cannot revoke these freedoms as long as you follow the license terms.

    Attribution : You must give appropriate credit, provide a link to the license,
                  and indicate if changes were made. You may do so in any reasonable manner,
                  but not in any way that suggests the licensor endorses you or your use.

    NonCommercial : You may not use the material for commercial purposes. **(see note below)

    ShareAlike : If you remix, transform, or build upon the material, you must distribute
                 your contributions under the same license as the original.

    No additional restrictions : You may not apply legal terms or technological measures that
                                 legally restrict others from doing anything the license permits.

   ** For commercial use, please contact the original copyright holder(s) to agree licensing terms

    This software is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE

*/

 /*
 * Updated and extended by David W Radcliffe M3666.
 *
 *  Now creates 6 (and upto 8) instances of the train controller class.
 *  Use an alternate LCD display: this is the Geekcreit� IIC I2C 2004 204 20 x 4 Character LCD Display SWITCH.
 *  Uses an (optional) 4 x 3 keypad for local control of the track outputs and their speed.
 *  Up to 8 CANCMDDCs can be added to one CAN bus, each with its own address and DCC Address range.
 *  DCC Addresses x001 - x00n are used, and the last digit is used by the keypad to select the loco.
 *  This allows loco 0 (and 9) to be used for some other (as yet undefined) function - maybe 'select all' ??
 */

/*
 This is copied from CANCMDDC and needs updating.
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
 Digital pin 18 (TX1)   Shutdown       - pull low when short-circuit detected
 Digital pin 19 (RX1)   Int'upt CAN
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
 Digital pin 35       LED Yellow - unused
 Digital pin 36       Encoder 8 Switch
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
 Digital pin 52 (SCK)   Sck   CAN
 Digital pin 53 (SS)    CS    CAN
 Digital / Analog pin 0     Encoder 1 A
 Digital / Analog pin 1     Encoder 2 A
 Digital / Analog pin 2     Encoder 3 A
 Digital / Analog pin 3     Encoder 4 A
 Digital / Analog pin 4     Encoder 5 A
 Digital / Analog pin 5     Encoder 6 A
 Digital / Analog pin 6     Encoder 7 A - unused
 Digital / Analog pin 7     Encoder 8 A - unused
 Digital / Analog pin 8     Encoder 1 B
 Digital / Analog pin 9     Encoder 2 B
 Digital / Analog pin 10    Encoder 3 B
 Digital / Analog pin 11    Encoder 4 B
 Digital / Analog pin 12    Encoder 5 B
 Digital / Analog pin 13    Encoder 6 B
 Digital / Analog pin 14    Encoder 7 B - unused
 Digital / Analog pin 15    Encoder 8 B - unused
*/



/*
      3rd party libraries needed for compilation: (not for binary-only distributions)

      Streaming   -- C++ stream style output, v5, (http://arduiniana.org/libraries/streaming/)
      ACAN2515    -- library to support the MCP2515/25625 CAN controller IC
*/

//////////////////////////////////////////////////////////////////////////

// IoAbstraction libraries
#include <IoAbstraction.h>
#include <TaskManagerIO.h>
// IoAbstraction reference to the arduino pins.
IoAbstractionRef arduinoPins = ioUsingArduino();

// Set this to 1 for CANBUS modules with 8 Mhz Crystal
// Set this to 0 for Sparkfun CANBUS shields.
#define CANBUS8MHZ 1

// CANCMDDC 1.10 added extra display/keypad options plus CANBUS
#define DEBUG         1 // set to 0 for no debug messages, 1 for messages to console
#define OLED_DISPLAY  0 // set to 0 if 128x32 OLED display is not present
#define LCD_DISPLAY   1 // set to 0 if 4x20 char LCD display is not present
#define KEYPAD        0 // set to 0 if 4x3 keypad is not present
#define CANBUS        1 // set to 0 if CAN h/w is not present
#define HALL_EFFECT   1  // set to 0 if Hall Effect current detection is not present.
#define CBUS_EVENTS   1  // set to 0 if CBUS events are supressed

#define USE_CBUSBUZZER 0 // set to 0 if CBUSBUZZER library is not present

#define ACCESSORY_REQUEST_EVENT 1  // 1 to code making a request for state.
#define USE_SHORT_EVENTS 1 // Use short events to poll the signal.

// Set this to 0 for the other hardware options
#define LINKSPRITE 1  // Defined to use Linksprite Motor Shield
#define L298N      1  // Linksprite modified with L298N output board


// 3rd party libraries as I use C++ style streaming output as used in Arduino CBUS library examples.
#include <Streaming.h>

// CBUS library header files
#include <CBUS2515.h>               // CAN controller and CBUS class
#include <CBUSswitch.h>             // pushbutton switch
#include <CBUSLED.h>                // CBUS LEDs
#if USE_CBUSBUZZER
#include <CBUSBUZZER.h>              // CBUS Buzzer
#endif
#include <CBUSconfig.h>             // module configuration
#include <cbusdefs.h>               // MERG CBUS constants

// CANCMDDC
#include <PWM.h>     // Library for controlling PWM Frequency
// This may need to become something which depends on LINKSPRITE.
#include "trainController.h"

// local header which is going to have to be adapted for pin numbers.
//#include "defs.h"

#if LCD_DISPLAY || OLED_DISPLAY 
#include <Wire.h>    // Library for I2C comminications for display
  #if OLED_DISPLAY
  /* libraries for Adafruit display module */
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  /*
  Adafruit 128x32 size display using I2C to communicate
  3 pins are required to interface (2 I2C (SCL & SDA)and one reset (user definable))
  */
  #define OLED_RESET 22
  #endif

  #if LCD_DISPLAY
  /* libraries for LCD display module */
  #include <hd44780.h>
  #include <hd44780ioClass/hd44780_I2Cexp.h >
        
  /*       Geekcreit� IIC I2C 2004 204 20 x 4 Character LCD Display Module using I2C to communicate
         2 pins are required to interface(I2C(SCL & SDA))

         see http://arduino-info.wikispaces.com/LCD-Blue-I2C Use Sketch from I2C LCD DISPLAY VERSION 1. Do not Forget to adjust contrast!
         https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
  */
  #endif
#endif

#if KEYPAD
#include <Keypad.h>
#endif

#define TRUE    true    // Changed to use the internal definition
#define FALSE   false   // Changed to use the internal definition
#define ON      1
#define OFF     0

// CBUS objects
CBUS2515 CBUS;                      // CBUS object
CBUSConfig config;                  // configuration object
CBUSLED ledGrn, ledYlw;             // LED objects
CBUSSwitch pb_switch;               // switch object

// module objects
CBUSSwitch moduleSwitch;            // an example switch as input
CBUSLED moduleLED;                  // an example LED as output
#if USE_CBUSBUZZER
CBUSBUZZER moduleBuzzer;             // an example Buzzer as output
#endif

// CBUS module parameters
unsigned char params[21];

// module name
unsigned char mname[7] = { 'C', 'M', 'D', 'D', 'C', ' ', '2'};

// Set GROVE 1 for a GROVE switch which is HIGH when pressed, otherwise 0
#define GROVE 1

// forward function declarations
void eventhandler(byte index, byte opc);
void framehandler(CANFrame *msg);
void checkSwitch(); // Forward declaration of the task function
void checkOverload(); // Forward declaration of the task function
// This needs to be extended.
byte nopcodes = 19;
byte opcodes[] = {OPC_ACON, OPC_ACOF, OPC_BON, OPC_ARST, 0x08, 0x09, 0x21, 0x22, 0x23, 
                  0x40, 0x41, 0x44, 0x45, 0x46, 0x47, 0x61, 0x63, OPC_PLOC, OPC_RESTP };
//
/// setup - runs once at power on
//

/* pin used for manual selection of use with CANCMD or standalone. */
/* Link pin to +5V if standalone required. */
#define MODESELECT  48
// Note: For the moment I do not have a CANCMD.
// Checking for it is retained for testing elsewhere.

/* Pins used by SPI interface to CAN module */
/* These are the  values for the MEGA2560 */
#define CHIPSELECT  53
#define CBUSINTPIN  18 // 49

#if LINKSPRITE
// Module pins defined here - these are not the CBUS pins.
const byte MODULE_LED_PIN    = 4;        // Module LED Pin using Green Pin
const byte MODULE_SWITCH_PIN = 3;        // Module Switch Pin
const byte MODULE_SOUNDER    = 7;        // Module buzzer pin

// 
// This is a problem as at the moment 5 and 6 are used for the
// CBUS lights and push button.
// The list is going to have to be changed anyway for the 
// Linksprite Motor Shield.
// I am going to have to define different pin sets for the LINKSPRITE case.
#define NUM_CONTROLLERS  2 // the number of controllers (pairs of pwmpins)

#if L298N
int pinI1=22;//define I1 interface
int pinI2=23;//define I2 interface 
int speedpinA=9;//enable motor A
int pinI3=12;//define I3 interface 
int pinI4=13;//define I4 interface 
int speedpinB=10;//enable motor B
#else
// Values from the motor shield example code.
int pinI1=8;//define I1 interface
int pinI2=11;//define I2 interface 
int speedpinA=9;//enable motor A
int pinI3=12;//define I3 interface 
int pinI4=13;//define I4 interface 
int speedpinB=10;//enable motor B
#endif

static int pwmpins[] = {
  // Check these values
  speedpinA, speedpinB, 0
};

#else
// These can be defined separately for this case
#define LED         13     // Pin number for the LED
#define SOUNDER     4
#define SWITCH      8

#define NUM_CONTROLLERS  5 // the number of controllers (pairs of pwmpins)
/*
 * The following are the PWM outputs used to drive the tracks.
 *
 * Pins are used in pairs and fed to H Bridges, the first pair is
 * assigned to the first address and so on.
 * List terminates with a zero
 * 
 * The following pins can handle the high frequency PWM
 * 7, 8, 11, 12, 5, 6, 2, 3
 */
// This is a problem as at the moment 5 and 6 are used for the
// CBUS lights and push button.
// I am not sure that these are used at all.
// There are different values in the calls to trainControllerClass.
static int pwmpins[] = {
  7, 8, 11, 12, 5, 6, 2, 3, 0
};
#endif

#if KEYPAD
/*
* The following are the inputs/outputs used to drive the keypad.
*/
static byte keypins[] = {
  37, 39, 41, 43, 45, 47, 49 // uses odd-numbered pins only so that the 7-way header plugs straight in
};
#endif



#if HALL_EFFECT
// Code for Hall Effect overload detection
// This requires an ACS712 Hall Effect sensor connected in the positive feed to the motors.
// I need to decide what pins to use for these on Linksprite.
#define overloadLED  A2 // used as digital output
#define overloadDetect A7 // used for analogue input

#define MAXCURRENT  50 // 50 is approximately 1 Amp. Maximum value 250ish

int currentDetectQuiescent = 0;
int currentDetected;
int countHall = 0 ;
#endif

// CANCMDDC  These may need to be different fo
// Adding a buzzer output for taught event
int buzzer = MODULE_SOUNDER;
#define TONE 1000    // Set the tone for the buzzer

#define MAXTIMEOUT 30      // Max number of seconds before session is timed out
                           // if no stayalive received for the session

// CANCMDDC 
#define TIMER_PRELOAD 64911 //59286 //65536 - ((16000000/256)/100);       // preload timer 65536-(16MHz/256/100Hz)
/**
 * The preset tests the jumper in the mode select pin to
 * determine the mode of operation. Connect pin to +5V if CANCMD is present.
 * Leave pin disconnected for standalone operation.
 * It is true if the CANCMDDC is working on a CBUS that has a CANCMD present.
 */

boolean cancmd_present = false;
volatile byte timer_counter = 0;
volatile byte flash_counter = 0;
volatile byte beep_counter = 0;
volatile byte update_counter = 0;
volatile boolean updateNow = false;
volatile boolean shutdownFlag = false;
/**
 * Definitions of the flags bits
 */
#define SF_FORWARDS  0x01      // Train is running forwards
#define SF_REVERSE   0x00      // Train is running in reverse
#define SF_LONG      0xC0      // long DCC address. top 2 bits of high byte. both 1 for long, both 0 for short.
#define SF_INACTIVE  -1        // CAB Session is not active
#define SF_UNHANDLED -1        // DCC Address is not associated with an analogue controller
#define SF_LOCAL     -2        // DCC Address is operated only by the keypad, and not part of a CAB Session

#define startAddress 1000     // multiplier for DCC address offset from device address. Device 0 uses 1000, device 1 uses 2000,...
byte deviceAddress = 0;       // assume only unit on CAN bus (for now)

struct {
  int             session;
  unsigned int    DCCAddress;
  byte            longAddress;
  byte            timeout;
  boolean         shared;       // this loco shared by > 1 CAB (this includes the keypad)
  struct {
      byte      address;      // DCC short address of consist. 0 = unused.
      byte      session;      // Session id of consist. 0 = unused.
      boolean   reverse;
  } consist;
  trainControllerClass trainController;
} controllers[NUM_CONTROLLERS] = {
#if LINKSPRITE
                // Values taken from the motor shield example code
                {SF_INACTIVE, (startAddress * (deviceAddress + 1)) + 1, SF_LONG, 0, false, { 0, 0, false }, trainControllerClass(pinI1, pinI2, pwmpins[0])}
               ,{SF_INACTIVE, (startAddress * (deviceAddress + 1)) + 2, SF_LONG, 0, false, { 0, 0, false }, trainControllerClass(pinI3, pinI4, pwmpins[1])}
#else
                  {SF_INACTIVE, (startAddress * (deviceAddress + 1)) + 1, SF_LONG, 0, false, { 0, 0, false }, trainControllerClass(22, 23, pwmpins[0])}
                   ,{SF_INACTIVE, (startAddress * (deviceAddress + 1)) + 2, SF_LONG, 0, false, { 0, 0, false }, trainControllerClass(24, 25, pwmpins[1])}
                   ,{SF_INACTIVE, (startAddress * (deviceAddress + 1)) + 3, SF_LONG, 0, false, { 0, 0, false }, trainControllerClass(26, 27, pwmpins[2])}
                   ,{SF_INACTIVE, (startAddress * (deviceAddress + 1)) + 4, SF_LONG, 0, false, { 0, 0, false }, trainControllerClass(28, 29, pwmpins[3])}
                   ,{SF_INACTIVE, (startAddress * (deviceAddress + 1)) + 5, SF_LONG, 0, false, { 0, 0, false }, trainControllerClass(30, 31, pwmpins[4])}
                   ,{SF_INACTIVE, (startAddress * (deviceAddress + 1)) + 6, SF_LONG, 0, false, { 0, 0, false }, trainControllerClass(32, 33, pwmpins[5])}
                 //,{SF_INACTIVE, (startAddress * (deviceAddress + 1)) + 7, SF_LONG, 0, false, { 0, 0, false }, trainControllerClass(14, 15, pwmpins[6])}
                 //,{SF_INACTIVE, (startAddress * (deviceAddress + 1)) + 8, SF_LONG, 0, false, { 0, 0, false }, trainControllerClass(16, 17, pwmpins[7])}
#endif
                                  };

#if ENCODER
//#define ENCODER_USE_INTERRUPTS
#define ENCODER_DO_NOT_USE_INTERRUPTS
#include "encoderController.h"
struct {
  encoderControllerClass encoderController;
} encoders[NUM_CONTROLLERS] = {
#if LINKSPRITE // Only 2 controllers in this case.
                {encoderControllerClass(A8,  A0, 38)},
                {encoderControllerClass(A9,  A1, 40)}
#else
                {encoderControllerClass(A8,  A0, 38)},
                {encoderControllerClass(A9,  A1, 40)},
                {encoderControllerClass(A10, A2, 42)},
                {encoderControllerClass(A11, A3, 44)},
                {encoderControllerClass(A12, A4, 46)},
                {encoderControllerClass(A13, A5, 48)},
                //{Encoder(A14, A6, 9), 0, 0, 0},
                //{Encoder(A15, A7, 36), 0, 0, 0}
#endif
};
#endif

boolean                cbusActive = true;


#if OLED_DISPLAY
  // construct a display object
  Adafruit_SSD1306 display(OLED_RESET);
#endif

#if (OLED_DISPLAY && SSD1306_LCDHEIGHT != 32)
  #error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

enum errorStates {
  blankError,
  noError,
  emergencyStop,
  CANbusError,
  locoStackFull,
  locoTaken,
  noSession,
  consistEmpty,
  locoNotFound,
  invalidRequest,
  sessionCancelled,
  motorOverload,
  invalidError
};


// This is following the methods in EzyBus_master to provide error messages.
// These have been limited to 16 chars to go on an LCD 2 by 16 display.
const char blank_string[]   PROGMEM  = "                ";
const char error_string_0[] PROGMEM  = "no error";
const char error_string_1[] PROGMEM  = "Emergency Stop";
const char error_string_2[] PROGMEM  = "CANbus error";
const char error_string_3[] PROGMEM  = "loco stack full";
const char error_string_4[] PROGMEM  = "loco taken";
const char error_string_5[] PROGMEM  = "no session";
const char error_string_6[] PROGMEM  = "consist empty";
const char error_string_7[] PROGMEM  = "loco not found";
const char error_string_8[] PROGMEM  = "invalid request";
const char error_string_9[] PROGMEM  = "session cancel";
const char error_string_10[] PROGMEM  = "motor overload";
const char error_string_11[] PROGMEM  = "invalid error";

const char* const error_string_table[] PROGMEM = {
  blank_string, 
  error_string_0, error_string_1, error_string_2, error_string_3, error_string_4, 
  error_string_5, error_string_6, error_string_7, error_string_8, error_string_9,
  error_string_10, error_string_11,
};

#define MAX_ERROR_NO 13

#if CBUS_EVENTS
// Event Nos for different events to be sent
enum eventNos {
  noEvent,  // not used
  testEvent,
  stopEvent,
  requestEvent
};

// Flag for stopevent
byte stopEventOn = false;
#endif

// Buffer for string output.
// This has been made safe for line termination.
#define MAX_LENGTH_OF_STRING 16
#define LENGTH_OF_BUFFER (MAX_LENGTH_OF_STRING + 1)
char error_buffer[LENGTH_OF_BUFFER];

// Add check for invalid error
void getErrorMessage(int i)
{
  if (i >= 0 && i <= MAX_ERROR_NO) {
     strncpy_P(error_buffer, (char*)pgm_read_word(&(error_string_table[i])),MAX_LENGTH_OF_STRING); 
  } else {
     strncpy_P(error_buffer, (char*)pgm_read_word(&(error_string_table[MAX_ERROR_NO])),MAX_LENGTH_OF_STRING); 
  }
}

void serialPrintError(int i)
{
 getErrorMessage(i);Serial.print(error_buffer); 
}
void serialPrintErrorln(int i)
{
 getErrorMessage(i);Serial.println(error_buffer);
}

// Display images left out for now.
#if OLED_DISPLAY
 // 'Merg logo_for_adafruit_display'
const unsigned char mergLogo [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x3c, 0x00, 0xe3, 0xff, 0xcf, 0xfc, 0x0f, 0xfe, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x7c, 0x00, 0xe3, 0xff, 0xcf, 0xfe, 0x1f, 0xff, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x62, 0x00, 0x23, 0x00, 0x0c, 0x02, 0x18, 0x01, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x61, 0x02, 0x23, 0x00, 0x0c, 0x02, 0x18, 0x01, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x60, 0x84, 0x23, 0x00, 0x08, 0x02, 0x18, 0x01, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x60, 0x48, 0x23, 0x00, 0x08, 0x02, 0x18, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x60, 0x70, 0x23, 0x00, 0x08, 0x02, 0x18, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x60, 0x20, 0x23, 0x00, 0x08, 0x02, 0x18, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x60, 0x00, 0x23, 0x00, 0x0c, 0x03, 0x18, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x70, 0x00, 0x23, 0xff, 0xcf, 0xff, 0x1c, 0x1e, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x78, 0x00, 0x23, 0xff, 0xcf, 0xff, 0x9c, 0x1f, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x78, 0x00, 0x23, 0xc0, 0x0f, 0x01, 0x9e, 0x19, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x78, 0x78, 0x23, 0x80, 0x0f, 0x01, 0x9e, 0x19, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x78, 0x78, 0x23, 0x80, 0x0f, 0x00, 0x9e, 0x19, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x78, 0x78, 0x23, 0x80, 0x0f, 0x00, 0x9e, 0x01, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x78, 0x78, 0x23, 0x80, 0x0f, 0x00, 0x9e, 0x01, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x78, 0x78, 0x23, 0x80, 0x0f, 0x00, 0x9e, 0x01, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x78, 0x78, 0x23, 0x80, 0x0f, 0x00, 0x9e, 0x01, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x78, 0x78, 0x23, 0x80, 0x0f, 0x00, 0x9e, 0x01, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x78, 0x78, 0x23, 0xc0, 0x0f, 0x00, 0x9e, 0x01, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x78, 0x78, 0x23, 0xc0, 0x0f, 0x00, 0x9f, 0x03, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x78, 0x78, 0x23, 0xff, 0x8f, 0x00, 0x9f, 0xfe, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x78, 0x78, 0x23, 0xff, 0x8f, 0x00, 0x8f, 0xfe, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00
};

 // 'bnhmrslogo'
const unsigned char bnhmrsLogo [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0xf0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x04, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x70, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x1c, 0x03, 0xff, 0x80, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x78, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x0c, 0x00, 0xff, 0x80, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x38, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x06, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x3f, 0x01, 0xbc, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x03, 0x80, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x3f, 0xbb, 0xbc, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xc0, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xbb, 0xdc, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x60, 0x07, 0xff, 0xf3, 0x3b, 0xfe, 0x3f, 0xc1, 0x9c, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x30, 0x03, 0xff, 0xff, 0xff, 0xdf, 0x3f, 0xc1, 0x9c, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x20, 0x07, 0xfd, 0xe6, 0x20, 0x8f, 0x3f, 0xc1, 0x9c, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x40, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xc1, 0x9c, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0xe0, 0x00, 0x00, 0x00, 0x3f, 0xc1, 0x9c, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x03, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xbc, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x06, 0x00, 0xff, 0x80, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x0c, 0x01, 0xff, 0x00, 0x01, 0xff, 0xff, 0xff, 0xc7, 0xfc, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0x3c, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x83, 0x3c, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x03, 0x38, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x03, 0x78, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x03, 0x70, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x70, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0xe0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x01, 0xe0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x01, 0xc0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x03, 0xc0, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x07, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x1f, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x00
};

#endif

#if LCD_DISPLAY
// The hd44780 library figures out what to do.  This corresponds to a display with an I2C expander pack.
// I could provide alternatives for other hardware.
hd44780_I2Cexp display(0x27);

volatile unsigned long previousTurnon    = 0;
volatile unsigned long alight            = 10000;
volatile boolean       barGraphicsLoaded = false;
volatile boolean       showingSpeeds     = false;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
// constants
const byte VER_MAJ = 2;                  // code major version
const char VER_MIN = 'a';                // code minor version
const byte VER_BETA = 9;                 // code beta sub-version
const byte MODULE_ID = 99;               // CBUS module type

const byte LED_GRN = 4;                  // CBUS green SLiM LED pin
const byte LED_YLW = 5;                  // CBUS yellow FLiM LED pin
const byte SWITCH0 = 6;                  // CBUS push button switch pin


/****** Function Prototypes *****/

int getSessionIndex (byte session);
int getDCCIndex (unsigned int dcc_address, byte long_address);
void releaseLoco(byte session);
void queryLoco(byte session);
void incrementTimeoutCounters(); // Run as a task.
void keepaliveSession(byte session);
void locoRequest(unsigned int address, byte long_address, byte flags);
void locoSession(byte session, unsigned int address, byte long_address, byte direction_, byte speed_);
void sendPLOC(byte session);
void sendDSPD(byte controllerIndex);
void sendError(unsigned int address, byte long_address, errorStates code);
void sendSessionError(byte session, errorStates code);
void addSessionConsist(byte session, byte consist);
void removeSessionConsist(byte session);
void sendReset(void);
void setSpeedAndDirection(byte controllerIndex, byte requestedSpeed, byte reverse);
void setSpeedSteps(byte session, byte steps);
void emergencyStopAll(void);
void receiveCBusMessage(void);
void nBeeps (byte numBeeps, int durationMillis);
void setupDisplay(void);
void showSpeeds(void);
void displaySpeed(byte sessionIndex);
void displayImage(const uint8_t *imageBitmap);
void displayVersion(void);
void initialiseDisplay(void);
void customChars(const uint8_t chars[][8]);
byte sid2byte(uint32_t canId);

void(* resetArduino) (void) = 0;//declare reset function at address 0
bool sendEvent(byte opCode,unsigned int eventNo);

// New function prototypes for Arduino CBUS Library

void messagehandler(CANFrame *msg); //Handle messages from CANCABs with message transferred
bool sendMessage(byte len, const byte *buf); // Send a message with the config.CANID
void updateProcessing();


#if KEYPAD
#pragma region initialise keypad

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
           //define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  { '1','2','3' },
  { '4','5','6' },
  { '7','8','9' },
  { '*','0','#' }
};
byte rowPins[ROWS] = { keypins[6], keypins[5], keypins[4], keypins[3] }; //connect to the row pinouts of the keypad
byte colPins[COLS] = { keypins[2], keypins[1], keypins[0] };             //connect to the column pinouts of the keypad

                                     //initialize an instance of class NewKeypad
Keypad keyPad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
#pragma endregion

/*
  The keys perform loco/speed selection according to the following FSM:
  Start in idle mode with loco 0 selected.
  Press * - Enter Loco select mode:
        Press * to stop current loco, then press # to reverse direction or press * to stop all locos. Press 0-n - Enter Speed select mode with that loco selected (if valid).
  Press # - eStop currently selected loco. Press # - eStop all locos.
  Press 0-9 - Enter speed digit. Scroll to left on subsequent digits. Press # - accept entered speed. If invalid (> 127), or Press *, or timeout, cancel current input and enter Speed select mode.
  Timeout after 3 secs to Speed select mode in all cases except eStop all locos.
*/
enum fsmStates {
  idle,
  locoSelect,
  locoStop,
  speedSelect,
  speedDigit,
  locoEmergStop,
  stealOrShare
};

struct {
  fsmStates state;
  byte      currentLoco;
  byte      previousLoco;
  char      digits[3];
} keyFSM = {
  idle, 255, 255, { ' ', ' ', ' ' }
};

volatile unsigned long previousKeypress = 0;
volatile unsigned long interval = 4000;
#endif

// Forward declaration of variable used to detect switch change.
byte previous_switch;
byte ledOn;
//
// here we globally store the task ID of our repeating task, we need this to cancel it later.
//
int taskId = TASKMGR_INVALIDID; // Set to this value so that it won't get cancelled before it exists!


/***************************************************************************************************
 * Arduino setup routine
 * Configures the I/O pins, initialises the CANBUS library
 * and sets up the initial session stack
 */
 void setup() {

//#if DEBUG
// Some use of the Serial port turned off if DEBUG = 0
  Serial.begin (115200);
  Serial << endl << endl << F("> ** CBUS CMDDCC2 module v2.2h ** ") << __FILE__ << endl;
//#endif

  // set config layout parameters
  config.EE_NVS_START = 10;
  config.EE_NUM_NVS = 10;
  config.EE_EVENTS_START = 50;
  config.EE_MAX_EVENTS = 64;
  config.EE_NUM_EVS = 1;
  config.EE_BYTES_PER_EVENT = (config.EE_NUM_EVS + 4);

  // initialise and load configuration
  config.setEEPROMtype(EEPROM_INTERNAL);
  config.begin();

#if DEBUG
  Serial << F("> mode = ") << ((config.FLiM) ? "FLiM" : "SLiM") << F(", CANID = ") << config.CANID;
  Serial << F(", NN = ") << config.nodeNum << endl;

  // show code version and copyright notice
  printConfig();
#endif

  // set module parameters
  params[0] = 20;                     //  0 num params = 10
  params[1] = 0xa5;                   //  1 manf = MERG, 165
  params[2] = VER_MIN;                //  2 code minor version
  params[3] = MODULE_ID;              //  3 module id, 99 = undefined
  params[4] = config.EE_MAX_EVENTS;   //  4 num events
  params[5] = config.EE_NUM_EVS;      //  5 num evs per event
  params[6] = config.EE_NUM_NVS;      //  6 num NVs
  params[7] = VER_MAJ;                //  7 code major version
  params[8] = 0x07;                   //  8 flags = 7, FLiM, consumer/producer
  params[9] = 0x32;                   //  9 processor id = 50
  params[10] = PB_CAN;                // 10 interface protocol = CAN, 1
  params[11] = 0x00;
  params[12] = 0x00;
  params[13] = 0x00;
  params[14] = 0x00;
  params[15] = '3';
  params[16] = '2';
  params[17] = '8';
  params[18] = 'P';
  params[19] = CPUM_ATMEL;
  params[20] = VER_BETA;

  // assign to CBUS
  CBUS.setParams(params);
  CBUS.setName(mname);

#if OLED_DISPLAY || LCD_DISPLAY
  initialiseDisplay();
  delay(2000);
#if OLED_DISPLAY
  displayImage(mergLogo);
  displayImage(bnhmrsLogo);
#endif

#if LCD_DISPLAY
  displayMergLogo();
#endif
  delay(2000);

  setupBarGraph();
  displayVersion();

  // start the speed display.
  showSpeeds();

#endif

#if KEYPAD
  // wire up keypad events
  keyPad.addEventListener(keypadEvent); // Add an event listener for this keypad
#endif


//  //initialize all timers except for 0, to save time keeping functions
  InitTimersSafe();
  Serial << F("> InitTimerssafe()") << endl;


  for (byte controllerIndex = 0; controllerIndex < NUM_CONTROLLERS; controllerIndex++)
  {
   controllers[controllerIndex].trainController.setPWMFrequency ();
  }
  Serial << F("> Set PWM frequency") << endl;

  // initialise CBUS switch
#if GROVE
  pb_switch.setPin(SWITCH0, HIGH);
  Serial << F("> Switch set to go HIGH when pressed") << endl;
#else
  pb_switch.setPin(SWITCH0, LOW);
  Serial << F("> Switch set to go LOW when pressed") << endl;
#endif

  // module reset - if switch is depressed at startup and module is in SLiM mode
  pb_switch.run();

  if (pb_switch.isPressed() && !config.FLiM) {
//#if DEBUG
    Serial << F("> switch was pressed at startup in SLiM mode") << endl;
//#endif
    config.resetModule(ledGrn, ledYlw, pb_switch);
  }

  // register our CBUS event handler, to receive event messages of learned events
  CBUS.setEventHandler(eventhandler);

  CBUS.setFrameHandler(framehandler, opcodes, nopcodes);

  // set LED and switch pins and assign to CBUS
  ledGrn.setPin(LED_GRN);
  ledYlw.setPin(LED_YLW);
  CBUS.setLEDs(ledGrn, ledYlw);
  CBUS.setSwitch(pb_switch);

  // set CBUS LEDs to indicate mode
  CBUS.indicateMode(config.FLiM);

  // configure and start CAN bus and CBUS message processing
  CBUS.setNumBuffers(4);
#if CANBUS8MHZ
  CBUS.setOscFreq(8000000UL);   // MCP2515 CANBUS 8Mhz 
#endif
  CBUS.setPins(CHIPSELECT,CBUSINTPIN);
  CBUS.begin();

#if GROVE
//  moduleSwitch.setPin(SWITCH, HIGH);
  ioDevicePinMode(arduinoPins, MODULE_SWITCH_PIN, LOW);
  Serial << F("> Module Switch ") << MODULE_SWITCH_PIN << F(" set to go HIGH when pressed") << endl;
#else
//  moduleSwitch.setPin(SWITCH, LOW);
  ioDevicePinMode(arduinoPins, MODULE_SWITCH_PIN, INPUT_PULLUP);
  Serial << F("> Module Switch ") << SWITCH << F(" set to go LOW when pressed") << endl;
#endif 
  // Capture the initial state of the switch.
  previous_switch = ioDeviceDigitalReadS(arduinoPins, MODULE_SWITCH_PIN);

  // configure the module LED, attached to pin via a 1K resistor
  ioDevicePinMode(arduinoPins, MODULE_LED_PIN, OUTPUT);
  // Set initial state of the LED.
  ioDeviceDigitalWrite(arduinoPins, MODULE_LED_PIN, LOW);

  // Schedule task to run every 250 milliseconds.
  taskManager.scheduleFixedRate(250, checkSwitch);
  // add additional taskManager functions here
  taskManager.scheduleFixedRate(250, checkOverload);
  // Increment counters every second
  taskManager.scheduleFixedRate(1000, incrementTimeoutCounters);
  // add additional taskManager functions here

#if HALL_EFFECT
  // Get an average value for the current detector, which should be about 512
  // Should I put some sort of delay in this loop?
  currentDetectQuiescent =0;
  countHall = 0;
  for (int i = 0; i < 20; i++)
  {
    currentDetectQuiescent = currentDetectQuiescent + analogRead(overloadDetect);
  }
  currentDetectQuiescent = currentDetectQuiescent / 20;
  #if DEBUG
    Serial.print  (F("> currentDetectQuiescent = "));
    Serial.println(currentDetectQuiescent);
  #if LCD_DISPLAY
    if (NUM_CONTROLLERS < 3) {
       display.setCursor(0,1);
       display.print("Q current");
       display.setCursor(10,1);
       display.print(currentDetectQuiescent);
    }
  #endif
  #endif

#endif

// Clear stop event if it is on.
#if CBUS_EVENTS
   if (stopEventOn) {
     sendEvent(OPC_ACOF,stopEvent);
     stopEventOn = false;
   }
   sendEvent1(OPC_ACON1,testEvent,noError); // Test of new code
#endif

#if KEYPAD
 Serial << F("> Keypad code present") << endl;
#endif

#if ENCODER
 Serial << F("> Encoder code present") << endl;
#endif

  // end of setup
//#if DEBUG
  Serial << F("> ready") << endl << endl;
//#endif
}


void checkOverload() {

// Overload code moved into a task.
#if HALL_EFFECT
  // Check for overload
  currentDetected = analogRead(overloadDetect);
  #if LCD_DISPLAY
    countHall++;
    if (countHall == 10) { // See how this works out
    if (NUM_CONTROLLERS < 3) {
       display.setCursor(0,2);
       display.print("  current         ");
       display.setCursor(10,2);
       display.print(currentDetected);
       // Display the values on the PWM pins.
       //for (int i = 0; i < NUM_CONTROLLERS; i++)
       //{
       //   display.setCursor(18,i+1);
       //  display.print(analogRead(pwmpins[i]));          
       //}
    }
    countHall = 0;
    }
  #endif
    if (currentDetected - currentDetectQuiescent > MAXCURRENT)
    {
      // Overload detected
      emergencyStopAll();
      tone(buzzer, 1000);
    // digitalWrite(SOUNDER, ON);
    // Not implemented yet
    //  digitalWrite(overloadLED, ON);
  #if DEBUG
      serialPrintError(motorOverload);
      //Serial.println  ("Overload detected.");
  #endif
     // I am not sure what to do about this.
     // I need some way to cancel the buzzer.
     // delay(500);
     // noTone(buzzer);
     //digitalWrite(SOUNDER, OFF);
    // Not implemented yet
    // digitalWrite(overloadLED, OFF);
     // delay(100);
    }
#endif
}

//
/// loop - runs forever
//

void loop() {

  //
  /// do CBUS message, switch and LED processing
  //

  CBUS.process();

  //
  /// process console commands
  //

  processSerialInput();

  // Run IO_Abstraction tasks.
  // This replaces actions taken here in the previous version.
  taskManager.runLoop();

  //
  /// bottom of loop()
  //
}

  //
  /// test for switch input
  /// as an example, it must be have been pressed or released for at least half a second
  /// then send a long CBUS event with opcode ACON for on and ACOF for off
  /// event number (EN) is 1

  /// you can just watch for this event in FCU or JMRI, or teach it to another CBUS consumer module
  //

// Check the switch and act if it has changed.
// This code has been moved from the loop()
void checkSwitch()
{
  // Use IO_Abstraction method to read the switch pin.
  byte new_switch = ioDeviceDigitalReadS(arduinoPins, MODULE_SWITCH_PIN);
  bool button_has_changed = (new_switch != previous_switch);
  // NOTE: new_switch is 0 if the switch is pressed and 1 otherwise.
  if(button_has_changed) 
  {
    previous_switch = new_switch;

#if DEBUG
    Serial << F("> button state change detected")  << new_switch << endl;  
    if(!new_switch) {
      Serial << F("> button pressed, send 0x") << _HEX(OPC_ACON) << endl;
    }  else {
      Serial << F("> button released, send 0x") << _HEX(OPC_ACOF)  << endl;      
    }
#endif


    byte opCode = (!new_switch ? OPC_ACON : OPC_ACOF);
    //byte eventNo = 1;
#if CBUS_EVENTS
    sendEvent(opCode,testEvent); // Test of new code.
    // Reset stopEvent by hand.
    if (stopEventOn) {
      sendEvent(OPC_ACOF,stopEvent);
      stopEventOn = false;
    }

#if REMOTE_SIGNAL_INPUT_EVENT
    if (!new_switch ) { // Send when button is pressed.
      Serial << F("Send request event") << endl;
      sendEvent(OPC_AREQ,requestEvent); // Test of event request.
    }
#endif

#endif

   }
}

// Send an event routine built to start sending events based on input from a CANCAB
bool sendEvent(byte opCode,unsigned int eventNo)
{
    CANFrame msg;
    msg.id = config.CANID;
    msg.len = 5;
    msg.data[0] = opCode;
    msg.data[1] = highByte(config.nodeNum);
    msg.data[2] = lowByte(config.nodeNum);
    msg.data[3] = highByte(eventNo); // event number (EN) could be > 255
    msg.data[4] = lowByte(eventNo); 
    msg.ext = false;
    msg.rtr = false;

    bool res = CBUS.sendMessage(&msg);
#if DEBUG
    if (res) {
      Serial << F("> sent CBUS event with opCode [ 0x") << _HEX(opCode) << F(" ] and event No ") << eventNo << endl;
    } else {
      Serial << F("> error sending CBUS event wit opcode [ 0x") <<  _HEX(opCode) << F(" ]") << endl;
    }
#endif
    return res;
}


// Send an event routine built to start sending events based with one extra byte
// The events can be ACON1 or ACOF1 with 1 byte of data.
bool sendEvent1(byte opCode, unsigned int eventNo, byte item)
{
    CANFrame msg;
    msg.id = config.CANID;
    msg.len = 6;
    msg.data[0] = opCode;
    msg.data[1] = highByte(config.nodeNum);
    msg.data[2] = lowByte(config.nodeNum);
    msg.data[3] = highByte(eventNo); // event number (EN) could be > 255
    msg.data[4] = lowByte(eventNo); 
    msg.data[5] = item;    // The extra byte
    msg.ext = false;
    msg.rtr = false;

    bool res = CBUS.sendMessage(&msg);
#if DEBUG
    if (res) {
      Serial << F("> sent CBUS event with opCode [ 0x") << _HEX(opCode) << F(" ] and event No ") << eventNo << endl;
    } else {
      Serial << F("> error sending CBUS event wit opcode [ 0x") <<  _HEX(opCode) << F(" ]") << endl;
    }
#endif
    return res;
}

// Send an event routine built to start sending events based with extra bytes
bool sendEventN(byte opCode,unsigned int eventNo, byte n, const byte* buf)
{
  // The events can be ACON1, ACOF1, ACON2, ACOF2, ACON3, ACOF3 with 1 2 or 3 bytes of data.
  // I am not checking the match between opcode and data length (as yet)
  if (n == 0) {
     // no data, use the old method.
     return sendEvent(opCode, eventNo);
  } else {
    CANFrame msg;
    msg.id = config.CANID;
    msg.len = 5+n;
    msg.data[0] = opCode;
    msg.data[1] = highByte(config.nodeNum);
    msg.data[2] = lowByte(config.nodeNum);
    msg.data[3] = highByte(eventNo); // event number (EN) could be > 255
    msg.data[4] = lowByte(eventNo); 
    for(byte i = 0;  i< n; i++)
    {
       msg.data[i+5] = buf[i];
    }
    msg.ext = false;
    msg.rtr = false;

    bool res = CBUS.sendMessage(&msg);
#if DEBUG
    if (res) {
      Serial << F("> sent CBUS event with opCode [ 0x") << _HEX(opCode) << F(" ] and event No ") << eventNo << endl;
    } else {
      Serial << F("> error sending CBUS event wit opcode [ 0x") <<  _HEX(opCode) << F(" ]");
      Serial << F(" with ") << n << F(" data : ");
      for (byte i = 0;  i< n; i++ ) Serial << buf[i] << F(" ");
      Serial << endl;
    }
#endif
    return res;
  }
}


// This replaces the CAN0.SendMsgBuff usage.
// It uses the CANID of the current configuration.
bool sendMessage(byte len, const byte *buf)
{
    CANFrame msg;
    msg.id = config.CANID;
    msg.len = len;
    for(byte i = 0;  i< len; i++)
    {
       msg.data[i] = buf[i];
    }
    msg.ext = false;
    msg.rtr = false;
  
    bool res = CBUS.sendMessage(&msg);
#if DEBUG
    if (res) {
      Serial << F("> sent CBUS message with code [ 0x") << _HEX(buf[0]) << F(" ] and size ") << len << endl;
      if (len > 1) {
        Serial << F("> Data : [");
        for(byte i = 1;  i< len; i++)
         {
            Serial << F(" 0x") << buf[i];
         }
        Serial << F(" ]") << endl;
      }
    } else {
      Serial << F("> error sending CBUS message") << endl;
    }
#endif
    return res;
 }

//
/// user-defined event processing function
/// called from the CBUS library when a learned event is received
/// it receives the event table index and the CAN frame
//

void eventhandler(byte index, CANFrame *msg) {

  // as an example, control an LED if the first EV equals 1

  Serial << F("> event handler: index = ") << index << F(", opcode = 0x") << _HEX(msg->data[0]) << endl;

/* 
 *  I am going to change the event handler to process based on the opcode as is done in the
 *  message handler.  This is a big change and I am going to maintain both versions of the
 *  code for the time being
 *  
 */
//#if USE_CODE_SWITCH 
    byte op_code = msg->data[0];
    unsigned int node_number = (msg->data[1] << 8 ) + msg->data[2];
    // This is not true in all cases.
    // For some it is the device number
    unsigned int event_number = (msg->data[3] << 8 ) + msg->data[4];
    // For now get the first event value
    byte ev = 1;
    //byte evval = config.getEventEVval(index, ev - 1); I think the library has changed.
    byte evval = config.getEventEVval(index, ev);
    Serial << F("> NN = ") << node_number << F(", EN = ") << event_number << endl;
    Serial << F("> op_code = ") << op_code << endl;
    Serial << F("> EV1 = ") << evval << endl;
    switch (op_code)
    {
         // Event on and off
         // Handle these together based on event no.
         case OPC_ACON:
         case OPC_ACOF:
         if (evval == 1) {
            if (op_code == OPC_ACON) {
               Serial << F("> switching the LED on") << endl;
               moduleLED.blink();
            } else if (op_code == OPC_ACOF) {
               Serial << F("> switching the LED off") << endl;
               moduleLED.off();
            }
         }

         // This is my buzzer example changed to use op codes and test off as well as on.
         // Now changed to use the CBUSBUZZER library
         if (evval == 99) { //Corrected bug 
            if ( op_code == OPC_ACON) {
               Serial << F("> switching the LED on") << endl;
               moduleLED.blink();
#if USE_CBUSBUZZER
               moduleBuzzer.on();
#else
               tone(buzzer, 1000);
#endif
               Serial << F("> BUZZER ON") << endl;
            } else if ( op_code == OPC_ACOF){
               Serial << F("> switching the LED off") << endl;
               moduleLED.off();
#if USE_CBUSBUZZER
               moduleBuzzer.off();
#else
               noTone(buzzer);
#endif
               Serial << F("> BUZZER OFF") << endl;
               }
         }

         break;
#if ACCESSORY_REQUEST_EVENT 
         // Accessory response events
         // Handle these together based on event no.
         case OPC_ARON:
         case OPC_AROF:
            Serial << F("> Handling long event response to remote request ");
            if ( op_code == OPC_ARON) {
               Serial << F(" : remote event is on") << endl;            
            } else if (op_code == OPC_AROF) {
               Serial << F(" : remote event is off") << endl;            
            }
         break;
#if USE_SHORT_EVENTS
         // Accessory response short events
         // Handle these together based on event no.
         case OPC_ARSON:
         case OPC_ARSOF:
            Serial << F("> Handling short event response to remote request ");
            Serial << F(" device number ") << event_number;
            if ( op_code == OPC_ARSON) {
               Serial << F(" : remote event is on") << endl;            
            } else if (op_code == OPC_ARSOF) {
               Serial << F(" : remote event is off") << endl;            
            }

#endif
#endif
         // Space for more op codes.
         default:
         // ignore any other CBUS events
#if DEBUG
         Serial << F("Event ignored with Opcode [ 0x") << _HEX(op_code) << F(" ]")<< endl;
#endif
    }
//#else // USE_CODE_SWITCH
// Whole section removed.
//#endif // USE_CODE_SWITCH

  return;
}

//
/// print code version config details and copyright notice
//

void printConfig(void) {

  // code version
  Serial << F("> code version = ") << VER_MAJ << VER_MIN << F(" beta ") << VER_BETA << endl;
  Serial << F("> compiled on ") << __DATE__ << F(" at ") << __TIME__ << F(", compiler ver = ") << __cplusplus << endl;
#if CANBUS8MHZ
  Serial << F("> Set for 8Mhz crystal") << endl;
#endif
  Serial << F("> CS Pin ") << CHIPSELECT << endl;
  Serial << F("> INT Pin ") << CBUSINTPIN << endl; 
// copyrights collected so far
  Serial << F("> © Mark Riddoch (MERG M1118) 2015") << endl;
  Serial << F("> © Ian Morgan (MERG M2775) 2019") << endl;
  Serial << F("> © David Radcliffe (MERG M3666) 2019") << endl;
  Serial << F("> © Duncan Greenwood (MERG M5767) 2019") << endl;
  Serial << F("> © John Fletcher (MERG M6777) 2019") << endl;
#if DEBUG
   Serial << F("> Error code test noError: ") << noError << endl;
   Serial << F("> Error code text is : ");
   serialPrintErrorln(noError);
   Serial << F("> The number of controllers is: ") << NUM_CONTROLLERS << endl;
  byte controllerIndex;
  for (controllerIndex = 0; controllerIndex < NUM_CONTROLLERS; controllerIndex++)
  {
    Serial << F("> Controller ") << controllerIndex << F(" is ") << controllers[controllerIndex].DCCAddress << endl;
  }
  #if OLED_DISPLAY || LCD_DISPLAY
    #if OLED_DISPLAY
    Serial << F("> OLED display available") << endl;
    #else
    Serial << F("> LCD display available") << endl;
    #endif
  #endif
  #if CBUS_EVENTS
    Serial << F("> CBUS Events available") << endl;
  #endif
  #if LINKSPRITE
     Serial << F("> LINKSPRITE Output Pins") << endl;
     Serial << F("> pinI1 = ") << pinI1 << endl; //define I1 interface
     Serial << F("> pinI2 = ") << pinI2 << endl; //define I1 interface
     Serial << F("> speedpinA = ") << speedpinA << endl; //define I1 interface
//int pinI3=12;//define I3 interface 
//int pinI4=13;//define I4 interface 
//int speedpinB=10;//enable motor B
  
  #endif
  #endif
  return;
}

//
/// user-defined frame processing function
/// called from the CBUS library for *every* CAN frame received
/// it receives a pointer to the received CAN frame
//  Added from new version of CBUS_empty

// Converted to get an opcode array now done when calling SetFrameHandler.
void framehandler(CANFrame *msg) {

  // as an example, format and display the received frame

#if DEBUG
  Serial << F("[ ") << (msg->id & 0x7f) << F("] [") << msg->len << F("] [");
  if ( msg->len > 0) {
    for (byte d = 0; d < msg->len; d++) {
      Serial << F(" 0x") << _HEX(msg->data[d]);
    }
  Serial << F(" ]") << endl;
  }

  if (nopcodes > 0) {
    Serial << F("Opcodes [ ");
    for(byte i = 0;  i < nopcodes; i++)
    {
       Serial << F(" 0x") << _HEX(opcodes[i]);
    }
    Serial << F(" ]") << endl;
  }
#endif

  if (nopcodes > 0) {
#if DEBUG
          Serial << F("Message received with Opcode [ 0x") << _HEX(msg->data[0]) << F(" ]")<< endl;
#endif
    for(byte i = 0;  i < nopcodes; i++)
    {
       if ( msg->data[0] == opcodes[i]) {
#if DEBUG
           Serial << F("Message recognised with Opcode [ 0x") << _HEX(opcodes[i]) << F(" ]")<< endl;
#endif
     // This will be executed if the code matches.
           messagehandler(msg);
           break;       
        }
    }
  }
  return;
}

void messagehandler(CANFrame *msg){

  int id;
  long unsigned int dcc_address;
  int long_address;
  byte controllerIndex;
  int lastInBufPtr = 0;
  int lastOutBufPtr = 0;
  //messageRecordType nextMessage;  //Defined in FIFO.h which may not be used.
/*
#if OLED_DISPLAY
  if (MessageBuffer.bufferOverflow == 1)
  {
    display.setCursor(64, 24);
    display.println("Overflow!");
    MessageBuffer.bufferOverflow = 0;
  }
#endif
*/

    //nextMessage = *msg;  // I think this should work for now.
    // This simple thing to do is to avoid nextMessage and get things direct from msg
    //nextMessage = fifoMessageBuffer.getMessage();

    if(msg->len > 0)            // Check to see whether data is received.
    {
 #if DEBUG
          Serial.print("CAN msg: ID ");
          Serial.print(msg->id, HEX);
          Serial.print(" OpCode:");
          for(int i = 0; i< msg->len; i++)                // Print each byte of the data
          {
            if (i == 1)
            {
              Serial.print(" Data:");
            }
            Serial.print(" ");
            if(msg->data[i] < 0x10)                     // If data byte is less than 0x10, add a leading zero
            {
              Serial.print("0");
            }
            Serial.print(msg->data[i], HEX);
          }
          Serial.println();
#endif

      // Perform the action for the received message
      switch (msg->data[0])
      {

        case 0x07:                              // System Reset (Sent by CANCMD on power up)
#if DEBUG
          Serial.println(F("System Reset (Sent by CANCMD on power up)"));
#endif
          cancmd_present = TRUE; // message came from CANCMD, so must be present
          for (controllerIndex = 0; controllerIndex < NUM_CONTROLLERS; controllerIndex++)
          {
            // release all active controllers
            if (controllers[controllerIndex].session > SF_INACTIVE)
            {
              controllers[controllerIndex].trainController.emergencyStop(); // Emergency Stop
              controllers[controllerIndex].session = SF_INACTIVE;
              // update the speed display.
              displaySpeed(controllerIndex);
            }
            // release all consists
            controllers[controllerIndex].consist = { 0, false, false };
          }
          break;

          // -------------------------------------------------------------------
        case 0x08:
#if DEBUG
          Serial.println(F("RTOFF - Request Track Off"));
#endif
          stopAll(true);
          break;

          // -------------------------------------------------------------------
        case 0x09:
#if DEBUG
          Serial.println(F("RTON - Request Track On"));
#endif
          break;
         
        // -------------------------------------------------------------------
        case 0x21:                              // Release loco command
#if DEBUG
          Serial.println(F("REL - Release loco"));
#endif
          releaseLoco(msg->data[1]);
          break;

        // -------------------------------------------------------------------
        case 0x22:                              // Query loco command
#if DEBUG
          Serial.println(F("QLOC - Query loco"));
#endif
          queryLoco(msg->data[1]);
          break;

        // -------------------------------------------------------------------
        case 0x23:                              // CAB Session keep alive command
#if DEBUG
          Serial.println(F("DKEEP - keep alive"));
#endif
// Temporary fix to call update when keep alive comes in.
          updateNow = true;
          updateProcessing();
          keepaliveSession(msg->data[1]);
          break;
          
       // -------------------------------------------------------------------
       case 0x40:                              // Request loco session (0x40)
#if DEBUG
          Serial.println(F("RLOC - Request loco session"));
#endif
           dcc_address = msg->data[2] + ((msg->data[1] & 0x3f) << 8);
           long_address = (msg->data[1] & SF_LONG);
 #if DEBUG
           Serial.print("Req Sess. ");
           Serial.print(F(" Addr: "));
           Serial.println(dcc_address);
          if (long_address == 0)
           {
             Serial.print("Short");
           }
           else
           {
             Serial.print("Long");
           }
           Serial.print(" Addr: ");
           Serial.println(dcc_address);
 #endif
           if (getDCCIndex (dcc_address, long_address) != SF_UNHANDLED)
           {
#if DEBUG
             Serial << F("Calling locoRequest (") << dcc_address << F(",") << long_address << F(")")<< endl;
#endif
             locoRequest(dcc_address, long_address, 0);
           } else {
#if DEBUG
             Serial << F("Calling locoRequest not called for (") << dcc_address << F(",") << long_address << F(")")<< endl;
#endif
           }
           break;
           
          // -------------------------------------------------------------------
        case 0x41:
#if DEBUG
          Serial.println(F("Query Consist......."));
#endif
          break;

        // -------------------------------------------------------------------
        case 0x44:                              // Set Speed Step Range
#if DEBUG
          Serial.println(F("STMOD - Set speed steps"));
#endif
          setSpeedSteps(msg->data[1],msg->data[2]);
          break;
          
          // -------------------------------------------------------------------
        case 0x45:
#if DEBUG
          Serial.println(F("PCON - Put loco in Consist"));
#endif
          addSessionConsist(msg->data[1], msg->data[2]);
          break;

          // -------------------------------------------------------------------
        case 0x46:
#if DEBUG
          Serial.println(F("KCON - Remove loco from Consist"));
#endif
          removeSessionConsist(msg->data[1]);
          break;

       // -------------------------------------------------------------------
        case 0x47:                              // Session speed and direction
#if DEBUG
          Serial.println(F("DSPD - Set speed & direction"));
#endif
        {
          byte session = msg->data[1];
          byte requestedSpeed = msg->data[2]; // & 0x7f;
          //byte requestedDirection = msg->data[2] & 0x80;
          controllerIndex = getSessionIndex(session);
          if (controllerIndex > SF_INACTIVE)
          {
            /*if (requestedSpeed == 1)
            {
              // emergency stop
              controllers[controllerIndex].trainController.emergencyStop();
            }
            else */
            //{
               setSpeedAndDirection(controllerIndex, requestedSpeed, 0);
            // controllers[controllerIndex].trainController.setSpeedAndDirection(msg->data[2] & 0x80, requestedSpeed);
            //}
            // update the speed display now done in setSpeedAndDirection
            //displaySpeed(controllerIndex);
          }
          // update processing and reset the timeout
          updateNow = true;
          updateProcessing();
          keepaliveSession(session);

          break;
        }         
       // -------------------------------------------------------------------
        case 0x61:                              // Request Steal or Share loco session (GLOC)
           dcc_address = msg->data[2] + ((msg->data[1] & 0x3f) << 8);
           long_address = (msg->data[1] & SF_LONG);
           locoRequest(dcc_address, long_address, msg->data[3]);
           break;
          
        // -------------------------------------------------------------------
        case 0xE1:                              // PLOC session Allocate from CANCMD
           cancmd_present = TRUE; // message came from CANCMD, so must be present
           dcc_address = msg->data[3] + ((msg->data[2] & 0x3f) << 8);
           long_address = (msg->data[2] & SF_LONG);
 #if DEBUG
           Serial.print("PLOC from CANCMD. ");
           if (long_address == 0)
           {
             Serial.print("Short");
           }
           else
           {
             Serial.print("Long");
           }
           Serial.print(" Addr: ");
           Serial.println(dcc_address);
 #endif
           // only interested in the addresses of our analogue outputs
           for (controllerIndex = 0; controllerIndex < NUM_CONTROLLERS; controllerIndex++)
           {
             if (controllers[controllerIndex].DCCAddress == dcc_address  && controllers[controllerIndex].longAddress == long_address)
             {
               int requestedSpeed = msg->data[4] & 0x7f;
               controllers[controllerIndex].session = msg->data[1];
               if (requestedSpeed = 1)
               {
                // emergency stop
                controllers[controllerIndex].trainController.emergencyStop();
               }
               else
               {
                 controllers[controllerIndex].trainController.setSpeedAndDirection((msg->data[4] & 0x80) >> 3, msg->data[4] & 0x7f);
               }
             }
           }
          break;
          
        // -------------------------------------------------------------------
        case 0x0A:                              // Emergency stop all OPC_RESTP
          for (controllerIndex = 0; controllerIndex < NUM_CONTROLLERS; controllerIndex++)
          {
            // stop all active controllers
            if (controllers[controllerIndex].session != SF_INACTIVE)
            {
#if DEBUG
              Serial << F("Controller ") << controllerIndex << F(" active")<< endl;
#endif
              controllers[controllerIndex].trainController.emergencyStop ();
              // update the speed display.
              displaySpeed(controllerIndex);
            } else {
#if DEBUG
              Serial << F("Controller ") << controllerIndex << F(" inactive")<< endl;
#endif
            }
          }
          // Tell all the CABs and Throttles
          emergencyStopAll();
          break;
 
        // -------------------------------------------------------------------
        case OPC_ACOF:
        case OPC_ACON:
#if DEBUG
           Serial << F("Message handled with Opcode [ 0x") << _HEX(msg->data[0]) << F(" ]")<< endl;
           Serial << F("Test code to see if a message is getting sent") << endl;
#endif
#if ACCESSORY_REQUEST_EVENT
#if USE_SHORT_EVENTS
      unsigned int device_number = 513;
      Serial << F("Send request short event with device number ") << device_number << endl;
      sendEvent(OPC_ASRQ,device_number); // Test of short event request.
#else
    //if (moduleSwitch.isPressed() ) { // Send when button is pressed.
      Serial << F("Send request long event") << endl;
      sendEvent(OPC_AREQ,requestEvent); // Test of long event request.
    //}
#endif
#endif

          // All this is here for testing purposes
          // Tell all the CABs and Throttles
          //emergencyStopAll();
          //queryLoco(1);
          //testMessage(1);
          //locoRequest(1,0,0);
          //locoRequest(1001,SF_LONG,0);
        break;
        // -------------------------------------------------------------------
        case OPC_AROF:
        case OPC_ARON:
        case OPC_ARSOF:
        case OPC_ARSON:
        // Process responses to AREQ and ASRQ messages.
        // I may want to set this up to handle only certain device nos.
        // It will also be possible to send data or events based on this information.
        byte local_opcode = msg->data[0];
#if ACCESSORY_REQUEST_EVENT
         unsigned int node_number = (msg->data[1] << 8 ) + msg->data[2];
         unsigned int event_number = (msg->data[3] << 8 ) + msg->data[4];
#if USE_SHORT_EVENTS
        if (local_opcode == OPC_ARSON) {
          Serial << F(" ON message from device ") << event_number << endl;
        } else if (local_opcode == OPC_ARSOF) {
          Serial << F(" OFF message from device ") << event_number << endl;
        }
#endif
        if (local_opcode == OPC_ARON) {
          Serial << F(" ON message from event ") << node_number << F(" event ") << event_number << endl;
        } else if (local_opcode == OPC_AROF) {
          Serial << F(" OFF message from node ") << node_number << F(" event ") << event_number << endl;
        }
#endif        
        break;
        // -------------------------------------------------------------------
        default:
          // ignore any other CBus messages
#if DEBUG
           Serial << F("Message handled with Opcode [ 0x") << _HEX(msg->data[0]) << F(" ]")<< endl;
#endif
          break;
      }
    }
}

// Task to increment timeout counters on active tasks.
void incrementTimeoutCounters()
{
#if DEBUG
    // Temporary output
    Serial << F("incrementTimeoutCounters() called") << endl;
#endif
    
    // increment timeout counters every second
  for (byte controllerIndex = 0; controllerIndex < NUM_CONTROLLERS; controllerIndex++)
  {
    if (controllers[controllerIndex].session != SF_INACTIVE)
    {
    ++controllers[controllerIndex].timeout; // increment timeout counter by 1
    }
  }

}

// New routine for update processing which can be called as needed.
void updateProcessing()
{
   byte controllerIndex;
   // No CBus message received this time round the loop, so check the sessions for timeout
    for (controllerIndex = 0; controllerIndex < NUM_CONTROLLERS; controllerIndex++)
    {
      if ((controllers[controllerIndex].session != SF_INACTIVE) && (controllers[controllerIndex].timeout > MAXTIMEOUT))
      {
#if DEBUG
  Serial.print("Session ");
  Serial.print(controllers[controllerIndex].session);
  Serial.print(" Address ");
  Serial.print(controllers[controllerIndex].DCCAddress);
  Serial.println(" Timed Out.");
#endif
        controllers[controllerIndex].trainController.setSpeedAndDirection(0, 0);
        releaseLoco(controllers[controllerIndex].session);
        sendSessionError(controllers[controllerIndex].session, sessionCancelled); // Send session cancelled message out to CABs
      }

      if (updateNow)
      {
        controllers[controllerIndex].trainController.matchToTargets ();
        // update the speed display.
        displaySpeed(controllerIndex);
      }
    }
    updateNow = false;
}  


void testMessage(byte i)
{
  unsigned char buf[4];
  buf[0] = 0x98;
  buf[1] = 0;
  buf[2] = 0;
  buf[3] = i;
  sendMessage(4,buf);
}
//
// Execution routines to be added.
//

/* *******************************************************************************
 * functions and procedures
 * *******************************************************************************/
int getSessionIndex (byte session)
{
  int controllerIndex;
  for (controllerIndex = 0; controllerIndex < NUM_CONTROLLERS; controllerIndex++)
  {
    if (controllers[controllerIndex].session == session)
    {
      return controllerIndex;
    }
  }
  // session not found, so
  return SF_INACTIVE;
}

int getDCCIndex (unsigned int dcc_address, byte long_address)
{
  int controllerIndex;
  for (controllerIndex = 0; controllerIndex < NUM_CONTROLLERS; controllerIndex++)
  {
    if ((controllers[controllerIndex].DCCAddress == dcc_address) && (controllers[controllerIndex].longAddress == long_address))
    {
      return controllerIndex;
    }
  }
  // controller not found, for this DCC address so
  return SF_INACTIVE;
}

/*
 * A loco release command for the given session
 */
void
releaseLoco(byte session)
{
  int controllerIndex = getSessionIndex(session);
  if (controllerIndex >= 0)
  {
    controllers[controllerIndex].session = SF_INACTIVE;
    controllers[controllerIndex].timeout = 0;
#if DEBUG
  Serial.print("Session ");
  Serial.print(session);
  Serial.print(" Address ");
  Serial.print(controllers[controllerIndex].DCCAddress);
  Serial.println(" Released.");
#endif
    // update the speed display.
    displaySpeed(controllerIndex);
  }
}

/*
 * A QLOC command for the given session from a CAB
 */
void
queryLoco(byte session)
{  
  int controllerIndex = getSessionIndex(session);
  // only respond if working standalone
  if (cancmd_present == FALSE)
  {
#if DEBUG
    Serial.print("Query Loco Session ");
    Serial.println(session);
#endif
    if (controllerIndex >= 0)
    {
      // session found, send a PLOC
      sendPLOC(session);
    }
    else
    {
      // session not active, so send back error
      sendSessionError(session, noSession);
    }
  }
}

/*
 * The command station has allocated a session to a locomotive
 */
void
locoSession(byte session, unsigned int address, byte long_address, byte direction_, byte speed_)
{
  int controllerIndex = getDCCIndex (address, long_address);
   #if DEBUG
     Serial.print("locoSession ");
     Serial.print(session);
     if (long_address == 0)
     {
       Serial.print(" Short");
     }
     else
     {
       Serial.print(" Long");
     }
     Serial.print(" DCC address ");
     Serial.println(address);
   #endif
  if (controllerIndex >= 0)
  {
    controllers[controllerIndex].session = session;
    controllers[controllerIndex].trainController.setSpeedAndDirection(direction_, speed_);
    // update the speed display.
    displaySpeed(controllerIndex);
  }
}

/*
 * Keep alive received, so reset the timeout counter
 */
void keepaliveSession(byte session)
{
  int controllerIndex = getSessionIndex(session);
  if (controllerIndex >= 0)
  {
    controllers[controllerIndex].timeout = 0;
  }
}

/*
 * A throttle has requested access to a particular loco address
 * This routine is only used if there is no CANCMD on the bus that will
 * allocate sessions.
 */
void
locoRequest(unsigned int address, byte long_address, byte flags)
{
  int controllerIndex = getDCCIndex(address, long_address);
#if DEBUG
  Serial.println("locoRequest");
#endif
  // only respond if working standalone
  if (cancmd_present == FALSE)
  {
#if DEBUG
    Serial.println("Standalone");
#endif
    if (controllerIndex >= 0)
    {
      // This is a DCC Address associated with our controllers
      if (controllers[controllerIndex].session != SF_INACTIVE)
      {
        // Loco is already used in a session
#if DEBUG
        Serial.print("Loco already allocated to session ");
        Serial.println(controllers[controllerIndex].session);
        Serial.print(F("Flag: "));
        Serial.println(flags);
#endif
        if (flags == 0)
          sendError(address, long_address, locoTaken);    // Send a Taken error
        else if (flags == 1)        // Steal
        {
          sendError(address, long_address, sessionCancelled);
          controllers[controllerIndex].session = SF_INACTIVE;
#if KEYPAD
          controllers[keyFSM.currentLoco].shared = false;
#endif
        }
        else if (flags == 2)        // Share
        {
          sendPLOC(controllers[controllerIndex].session);
#if KEYPAD
          controllers[keyFSM.currentLoco].shared = true;
#endif
        }
        else
          sendError(address, long_address, invalidRequest);
        return;
      }
    }
    else
    {
      // This DCC Address is not associated with any of our controllers
      sendError(address, long_address, invalidRequest);
      return;
    }
    // If we have got this far then the controller is not in use
    // set a new session number for the controller to use
   
    locoSession(controllerIndex, address, long_address, SF_FORWARDS, 0);
 #if DEBUG
        Serial.print("Session Allocated: ");
        Serial.println(controllers[controllerIndex].session);
 #endif
    sendPLOC(controllers[controllerIndex].session);
  }
  // Do nothing if there is a CANCMD present - it will assign sessions.
}

/*
* A throttle has requested access to a particular consist address
* This routine is only used if there is no CANCMD on the bus that will
* allocate sessions.
*/
void consistRequest(unsigned int address)
{
#if DEBUG
  Serial.println(F("ConsistRequest"));
#endif
  // only respond if working standalone
  if (cancmd_present == FALSE)
  {
#if DEBUG
    Serial.println(F("Standalone"));
    byte index;
#endif
    if ((address > 0) && address < 128)
    {
      //This is a DCC Address associated with our consists
      boolean found = false;

      for (index = 0; index < NUM_CONTROLLERS; index++)
      {
        if (controllers[index].consist.address == address)
        {
          found = true;
          break;
        }
      }

      if (found == true)
      {
        if (controllers[index].consist.session > 0)
        {
#if DEBUG
          Serial.print(F("Consist in use "));
          Serial.println(address);
#endif
          sendError(address, 0, locoTaken);
          return;
        }
      }
      else
      {
#if DEBUG
        Serial.print(F("Consist not found "));
        Serial.println(address);
#endif
        sendError(address, 0, consistEmpty);
        return;
      }
    }
    else
    {
      // This DCC Address is not associated with any of our consists
#if DEBUG
      Serial.print(F("Invalid consist address: "));
      Serial.println(address);
#endif
      sendError(address, 0, invalidRequest);
      return;
    }
    // If we have got this far then the consist is not in use.
    // Set a new session number for the consist - same as address with MSB to 1.
    // The session id is common across all CANCMDDC instances.
    controllers[index].consist.session = address | 0x80;
#if DEBUG
    Serial.print(F("Consist Session Allocated: "));
    Serial.println(address | 0x80);
#endif
    sendPLOCConsist(address);
  }
  // Do nothing if there is a CANCMD present - it will assign sessions.
}

/**
 * Send a PLOC message in response to a CAB requesting a session for
 * a DCC address
 */
void sendPLOC(byte session)
{
  unsigned char buf[8];
  int controllerIndex = getSessionIndex(session);
  // only send this response if working standalone
  if (cancmd_present == FALSE)
  {
   #if DEBUG
     Serial.print("Send PLOC ");
   #endif
    buf[0] = 0xE1; // OPC_PLOC
    buf[1] = session;
    buf[2] = ((controllers[controllerIndex].DCCAddress >> 8) & 0x3f) | (controllers[controllerIndex].longAddress);
    buf[3] = (controllers[controllerIndex].DCCAddress) & 0xff;
    buf[4] = controllers[controllerIndex].trainController.getSpeed() | (controllers[controllerIndex].trainController.getDirection() ? 0x80 : 0);
    buf[5] = 0;  // Zero function bytes
    buf[6] = 0;
    buf[7] = 0;
    sendMessage(8, buf);
   #if DEBUG
          Serial.print("CAN msg: ");
          for(int i = 0; i<8; i++)                // Print each byte of the data
          {
            Serial.print(" ");
            if(buf[i] < 0x10)                     // If data byte is less than 0x10, add a leading zero
            {
              Serial.print("0");
            }
            Serial.print(buf[i], HEX);
          }
          Serial.println();
   #endif
  }
}


void sendPLOCConsist(byte address)
{
  unsigned char buf[8];
  // only send this response if working standalone
  if (cancmd_present == FALSE)
  {
    // only send this response if 1st device on bus - we don't want up to 8 identical messages sent
    if (deviceAddress == 0)
    {
#if DEBUG
      Serial.print(F("Send PLOC "));
#endif
        buf[0] = 0xE1; // OPC_PLOC
        buf[1] = address | 0x80;
        buf[2] = 0;
        buf[3] = address;
        buf[4] = 0;
        buf[5] = 0;  // Zero function bytes
        buf[6] = 0;
        buf[7] = 0;
        sendMessage(8, buf);
#if DEBUG
      Serial.print(F("CAN msg: "));
      for (int i = 0; i < 8; i++)                // Print each byte of the data
      {
        Serial.print(F(" "));
        if (buf[i] < 0x10)                     // If data byte is less than 0x10, add a leading zero
        {
          Serial.print(F("0"));
        }
        Serial.print(buf[i], HEX);
      }
      Serial.println();
#endif
    }
  }
}

void addSessionConsist(byte session, byte consist)
{
#if DEBUG
  Serial.print(F("Add to consist: "));
  Serial.println(consist);
#endif

  // does the session belong to this controller?
  byte index = getSessionIndex(session);

  if (index == SF_UNHANDLED)
    return;

  // is the session already in a consist?
  removeSessionConsist(session);

  // add the consist address to the loco
  // remove for now -
  //invalid narrowing conversion from "int" to "unsigned char"

  //controllers[index].consist = { (consist & 0x7f), 0, ((consist & 0x80) == 0x80) };
}


void removeSessionConsist(byte session)
{
#if DEBUG
  Serial.print(F("Remove from consist: "));
  Serial.println(session);
#endif

  for (byte i = 0; i < NUM_CONTROLLERS; i++)
  {
    if (controllers[i].consist.address == (session & 0x7f))
    {
      controllers[i].consist.session = 0;
    }
  }
}

void setSpeedAndDirection(byte controllerIndex, byte requestedSpeed, byte reverse)
{
  if ((requestedSpeed & 0x7F) == 1)
  {
    // emergency stop
    controllers[controllerIndex].trainController.emergencyStop();
  }
  else
  {
#if DEBUG
    Serial << F("Setting speed to ") << (requestedSpeed & 0x7f) << " with reverse " << reverse << endl;
#endif
    controllers[controllerIndex].trainController.emergencyStopOff();
    controllers[controllerIndex].trainController.setSpeedAndDirection(((requestedSpeed & 0x80) ^ reverse) >> 7, requestedSpeed & 0x7f);
  }
  // update the speed display.
  displaySpeed(controllerIndex);
}

void setSpeedSteps(byte session, byte steps)
{
  // This is only relevent for DCC, so can be ignored
}

/**
 * Send an error packet labelled with the DCC address
 */
void
sendError(unsigned int address, byte long_address, errorStates code)
{
  unsigned char buf[4];
#if DEBUG
  Serial.print("Send Loco ");
  Serial.print(address);
  Serial.print(" Error ");
  Serial.println(code);
  serialPrintErrorln(code);
#endif
  buf[0] = 0x63; // OPC_ERR
  buf[1] = ((address >> 8) & 0xff) | long_address;
  buf[2] = address & 0xff;
  buf[3] = code;
  sendMessage(4, buf);
}

/**
 * Send a session error message to the CABs, labelled with the session number
 */
void
sendSessionError(byte session, errorStates code)
{
  unsigned char buf[4];
#if DEBUG
  Serial.print("Send Session ");
  Serial.print(session);
  Serial.print(" Error ");
  Serial.println(code);
#endif
  buf[0] = 0x63; // OPC_ERR
  buf[1] = session;
  buf[2] = 0;
  buf[3] = code;
  sendMessage(4, buf);
}

/**
 * Send a reset signal to all connected CABs
 */
void
sendReset()
{
  unsigned char buf[1];
  int i;
  buf[0] = 0x07; // OPC_ARST
  for (i=0; i< NUM_CONTROLLERS; i++)
  {
   sendMessage(1, buf);
  }
  buf[0] = 0x03; // OPC_BON
  sendMessage(1, buf);
}

void
emergencyStopAll()
{
  unsigned char buf[1];
  // Tell all the cabs
  buf[0] = 0x06; // ESTOP
  sendMessage(1, buf);
  beep_counter = 100; // sound buzzer 1 second
#if CBUS_EVENTS
  sendEvent(OPC_ACON,stopEvent);
  stopEventOn = true;
#endif
}

/**
* Stop all DC tracks
* Loop over every session and if it is not free set
* the speed to 0 (stop) or 1 (eStop)
*/
void stopAll(boolean emergency)
{
  for (byte controllerIndex = 0; controllerIndex < NUM_CONTROLLERS; controllerIndex++)
  {
    // stop all active controllers
    if (controllers[controllerIndex].session != SF_INACTIVE)
    {
      if (emergency)
        controllers[controllerIndex].trainController.emergencyStop();
      else
        controllers[controllerIndex].trainController.setSpeed(0);
      // update the speed display.
      displaySpeed(controllerIndex);
      sendDSPD(controllerIndex);
    }
  }
#if CBUS_EVENTS
  sendEvent(OPC_ACON,stopEvent);
  stopEventOn = true;
#endif
}

/*
* Send a DSPD message to CABs showing speed/direction
*/
void sendDSPD(byte controllerIndex)
{

  unsigned char buf[3];

#if DEBUG
  Serial.print(F("Send DSPD "));
#endif
    buf[0] = 0x47; // OPC_DSPD
    buf[1] = controllers[controllerIndex].session;
    buf[2] = controllers[controllerIndex].trainController.getSpeed() | (controllers[controllerIndex].trainController.getDirection() * 0x80);
    sendMessage(3, buf);
  //CAN0.sendMsgBuf(((unsigned long)canId.id) << 5, 3, buf);
#if DEBUG
  Serial.print(F("CAN msg: "));
  for (int i = 0; i < 3; i++)                // Print each byte of the data
  {
    Serial.print(F(" "));
    if (buf[i] < 0x10)                     // If data byte is less than 0x10, add a leading zero
    {
      Serial.print(F("0"));
    }
    Serial.print(buf[i], HEX);
  }
  Serial.println();
#endif

}

// set up fixed text on display
void
setupDisplay()
{
  // we will display controllers in 2 columns on 3 lines (max 6 controllers)
#if OLED_DISPLAY || LCD_DISPLAY
  int line; // 0-2
  int column; // 0 or 1
  int x_pos;
  int y_pos;
  float barsize;
  
  // Clear the buffer.
#if OLED_DISPLAY
  float barsize;
  display.clearDisplay();
  display.setTextSize(1);
#else
  display.clear();
#endif

  for (int controllerIndex=0; controllerIndex < NUM_CONTROLLERS; controllerIndex++)
  {
    line = controllerIndex / 2;
    column = controllerIndex - (line * 2);
#if OLED_DISPLAY
    x_pos = column*67;
    y_pos = line*12;
#else
    x_pos = column * 10;
    y_pos = line;
#endif

    display.setCursor(x_pos, y_pos);
    display.print(controllers[controllerIndex].DCCAddress); // display DCC address
 //   if (init)
//    {
//#if OLED_DISPLAY
//      display.setCursor((x_pos)+30, y_pos);
//#else
//      display.setCursor((x_pos)+5, y_pos);
//#endif
//
//      display.write("Free");
//    }
   }

    display.display();

  if (!barGraphicsLoaded)
    setupBarGraph();

  showingSpeeds = true;

#endif
} 

#if OLED_DISPLAY || LCD_DISPLAY
void showSpeeds()
{
  if (!showingSpeeds)
    setupDisplay();

  for (byte controllerIndex = 0; controllerIndex < NUM_CONTROLLERS; controllerIndex++)
  {
    displaySpeed(controllerIndex);
  }

}
#endif

// Display the current speed settings
void
displaySpeed(byte controllerIndex)
{
  // we will display controllers in 2 columns on 3 lines (max 6 controllers)
#if OLED_DISPLAY || LCD_DISPLAY
  int line = controllerIndex / 2; // 0-2
  int column = controllerIndex - (line * 2); // 0 or 1
  int x_pos;
  int y_pos;

  //float barsize;
  display.backlight();
  previousTurnon = millis();
  
#if OLED_DISPLAY
    x_pos = column*67;
    y_pos = line*12;
    if (controllers[controllerIndex].session != SF_INACTIVE)
    {
      barsize = (controllers[controllerIndex].trainController.getSpeed()*30)/128;
      display.fillRect((x_pos)+30,y_pos,30,8, BLACK);
      display.drawRect((x_pos)+30,y_pos,30,8, WHITE);
      display.fillRect((x_pos)+30,y_pos,barsize,8, WHITE);
    }
    else
    {
      display.fillRect((x_pos)+30,y_pos,30,8, BLACK);
      display.setCursor((x_pos)+30, y_pos);
      display.println("Free");
    }
  display.display();
#else
// LCD_DISPLAY
  if (showingSpeeds)
  {
    x_pos = column * 10;
    y_pos = line;
    if (controllers[controllerIndex].session == SF_INACTIVE)
    {
      display.setCursor((x_pos)+4, y_pos);
      display.write("-Free-");
    }
    else
    {
      display.setCursor((x_pos)+4, y_pos);

      uint8_t speed_ = controllers[controllerIndex].trainController.getSpeed();
      uint8_t targetSpeed = controllers[controllerIndex].trainController.getTargetSpeed();
      //display.print(speed); // in text
      bool isEstop = controllers[controllerIndex].trainController.isEmergencyStop();
#if DEBUG
     Serial << F("> target speed ") << targetSpeed << F(" speed ") << speed_ << F("  ") << isEstop << endl;
#endif
      // There are 6 sections in the bar-graph, each filled with of 1 of 6 characters (0-4 and <space>) for that part of the speed.
      // There are 31 unique bar-graphs, meaning that each graph handles 128 / 31 = 4.1 speed steps.
      // Speed 0 is Stop, and Graph 0 (nothing displayed) is otherwise unused,
      // so the first 3 graphs and the last one show more speed steps than the others, covering all remaining 127 speed steps.

#pragma region section 1

      //if (controllers[controllerIndex].trainController.eStopped == true)
      if (controllers[controllerIndex].trainController.isEmergencyStop() )
      display.print("-STOP-");
      else
      {
        switch (speed_)
        {
        case 0:
          display.print("-Stop-");
          break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
          display.print(char(0));
          break;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
          display.print(char(1));
          break;
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
          display.print(char(2));
          break;
        case 17:
        case 18:
        case 19:
        case 20:
          display.print(char(3));
          break;
        default:
          display.print(char(4));
          break;
        }
      }
#pragma endregion

#pragma region section 2
      switch (speed_)
      {
      case 0:
        break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
        display.print(" ");
        break;
      case 25:
      case 26:
      case 27:
      case 28:
        display.print(char(0));
        break;
      case 29:
      case 30:
      case 31:
      case 32:
        display.print(char(1));
        break;
      case 33:
      case 34:
      case 35:
      case 36:
        display.print(char(2));
        break;
      case 37:
      case 38:
      case 39:
      case 40:
        display.print(char(3));
        break;
      default:
        display.print(char(4));
        break;
      }
#pragma endregion


#pragma region section 3
      switch (speed_)
      {
      case 0:
        break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
      case 27:
      case 28:
      case 29:
      case 30:
      case 31:
      case 32:
      case 33:
      case 34:
      case 35:
      case 36:
      case 37:
      case 38:
      case 39:
      case 40:
      case 41:
      case 42:
      case 43:
      case 44:
        display.print(" ");
        break;
      case 45:
      case 46:
      case 47:
      case 48:
        display.print(char(0));
        break;
      case 49:
      case 50:
      case 51:
      case 52:
        display.print(char(1));
        break;
      case 53:
      case 54:
      case 55:
      case 56:
        display.print(char(2));
        break;
      case 57:
      case 58:
      case 59:
      case 60:
        display.print(char(3));
        break;
      default:
        display.print(char(4));
        break;

      }
#pragma endregion

#pragma region section 4
      switch (speed_)
      {
      case 0:
        break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
      case 27:
      case 28:
      case 29:
      case 30:
      case 31:
      case 32:
      case 33:
      case 34:
      case 35:
      case 36:
      case 37:
      case 38:
      case 39:
      case 40:
      case 41:
      case 42:
      case 43:
      case 44:
      case 45:
      case 46:
      case 47:
      case 48:
      case 49:
      case 50:
      case 51:
      case 52:
      case 53:
      case 54:
      case 55:
      case 56:
      case 57:
      case 58:
      case 59:
      case 60:
      case 61:
      case 62:
      case 63:
      case 64:
        display.print(" ");
        break;
      case 65:
      case 66:
      case 67:
      case 68:
        display.print(char(0));
        break;
      case 69:
      case 70:
      case 71:
      case 72:
        display.print(char(1));
        break;
      case 73:
      case 74:
      case 75:
      case 76:
        display.print(char(2));
        break;
      case 77:
      case 78:
      case 79:
      case 80:
        display.print(char(3));
        break;
      default:
        display.print(char(4));
        break;
      }
#pragma endregion

#pragma region section 5
      switch (speed_)
      {
      case 0:
        break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
      case 27:
      case 28:
      case 29:
      case 30:
      case 31:
      case 32:
      case 33:
      case 34:
      case 35:
      case 36:
      case 37:
      case 38:
      case 39:
      case 40:
      case 41:
      case 42:
      case 43:
      case 44:
      case 45:
      case 46:
      case 47:
      case 48:
      case 49:
      case 50:
      case 51:
      case 52:
      case 53:
      case 54:
      case 55:
      case 56:
      case 57:
      case 58:
      case 59:
      case 60:
      case 61:
      case 62:
      case 63:
      case 64:
      case 65:
      case 66:
      case 67:
      case 68:
      case 69:
      case 70:
      case 71:
      case 72:
      case 73:
      case 74:
      case 75:
      case 76:
      case 77:
      case 78:
      case 79:
      case 80:
      case 81:
      case 82:
      case 83:
      case 84:
        display.print(" ");
        break;
      case 85:
      case 86:
      case 87:
      case 88:
        display.print(char(0));
        break;
      case 89:
      case 90:
      case 91:
      case 92:
        display.print(char(1));
        break;
      case 93:
      case 94:
      case 95:
      case 96:
        display.print(char(2));
        break;
      case 97:
      case 98:
      case 99:
      case 100:
        display.print(char(3));
        break;
      default:
        display.print(char(4));
        break;
      }
#pragma endregion

#pragma region section 6
      switch (speed_)
      {
      case 0:
        break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
      case 27:
      case 28:
      case 29:
      case 30:
      case 31:
      case 32:
      case 33:
      case 34:
      case 35:
      case 36:
      case 37:
      case 38:
      case 39:
      case 40:
      case 41:
      case 42:
      case 43:
      case 44:
      case 45:
      case 46:
      case 47:
      case 48:
      case 49:
      case 50:
      case 51:
      case 52:
      case 53:
      case 54:
      case 55:
      case 56:
      case 57:
      case 58:
      case 59:
      case 60:
      case 61:
      case 62:
      case 63:
      case 64:
      case 65:
      case 66:
      case 67:
      case 68:
      case 69:
      case 70:
      case 71:
      case 72:
      case 73:
      case 74:
      case 75:
      case 76:
      case 77:
      case 78:
      case 79:
      case 80:
      case 81:
      case 82:
      case 83:
      case 84:
      case 85:
      case 86:
      case 87:
      case 88:
      case 89:
      case 90:
      case 91:
      case 92:
      case 93:
      case 94:
      case 95:
      case 96:
      case 97:
      case 98:
      case 99:
      case 100:
      case 101:
      case 102:
      case 103:
      case 104:
        display.print(" ");
        break;
      case 105:
      case 106:
      case 107:
      case 108:
        display.print(char(0));
        break;
      case 109:
      case 110:
      case 111:
      case 112:
        display.print(char(1));
        break;
      case 113:
      case 114:
      case 115:
      case 116:
        display.print(char(2));
        break;
      case 117:
      case 118:
      case 119:
      case 120:
        display.print(char(3));
        break;
      default:
        display.print(char(4));
        break;
      }
#pragma endregion

#pragma region status line
      if (NUM_CONTROLLERS < 7)
      {
        display.setCursor(6, 3);
        display.print("     "); // Set field blank
        display.setCursor(0, 3);
        display.print("Last:");
        display.print(controllers[controllerIndex].DCCAddress);

        display.setCursor(10, 3);
        display.print("Speed:");
        display.print(to3digits(controllers[controllerIndex].trainController.getSpeed()));

        display.setCursor(19, 3);
        display.print(char(5 + controllers[controllerIndex].trainController.getDirection()));
      }
#pragma endregion

    }
  }
  else
  {
    setupDisplay();
    showSpeeds();
  }
#endif

#endif
}

String to3digits(uint8_t speed)
{
  char s[4];
  dtostrf(speed, 3, 0, s);
  return s;
}

void displayImage(const uint8_t *imageBitmap)
{
#if OLED_DISPLAY
  // Clear the buffer.
  display.clearDisplay();
  // Show Merg logo on the display hardware.
  display.drawBitmap(0, 0, imageBitmap, 128, 32, 1);
  display.display();
  // leave logo on screen for a while
  delay(1500);
#endif
}

void displayVersion()
{
#if OLED_DISPLAY
  // Clear the buffer.
  display.clearDisplay();
  // display module name and version for a short time
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20,8);
  display.println("CANCMDDC");
  display.setTextSize(1);
  display.setCursor(60,24);
  display.println("v2.3");
  display.display();
#endif
#if LCD_DISPLAY
  // Clear the buffer.
  display.clear();

  display.clear();
  display.setCursor(3, 0);
  display.write("CANCMDDC v");
  display.print(VERSION);
  display.setCursor(0, 1);
  display.write(char(7));
  display.setCursor(2, 1);
  display.write("David W Radcliffe &");
  display.setCursor(0, 2);
  display.write("John Fletcher with");
  display.setCursor(0, 3);
  display.write("I.Morgan & M.Riddoch");

#endif
  delay(2000);
}

void initialiseDisplay()
{
#if OLED_DISPLAY
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // Clear the buffer.
  display.clearDisplay();
#endif
#if LCD_DISPLAY
  display.begin(20, 4); // initialize the lcd for 20 chars 4 lines, turn on backlight
  display.display();    // turn it on
  display.clear();
#endif
}


#if LCD_DISPLAY
void displayMergLogo()
{
  // Creat a set of new characters
  const uint8_t mergLogo[][8] = {
    { B00001111, B00011111, B00011111, B00011111, B00011100, B00011100, B00011100, B00011100 }, // 0
    { B00011111, B00011111, B00011111, B00011111, B00000000, B00000000, B00000000, B00000000 }, // 1
    { B00011100, B00011100, B00011100, B00011100, B00011100, B00011100, B00011100, B00011100 }, // 2
    { B00000000, B00000000, B00000000, B00000000, B00011111, B00011111, B00011111, B00011111 }, // 3
    { B00000111, B00000111, B00000111, B00000111, B00000111, B00000111, B00000111, B00000111 }, // 4
    { B00000000, B00000000, B00000000, B00000000, B00011111, B00011111, B00011111, B00011111 }, // 5
    { B00011111, B00011111, B00011111, B00011111, B00001111, B00000111, B00000111, B00000111 }, // 6
    { B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111 }  // 7
  };

//void displayMergLogo()
//{
  customChars(mergLogo);

  char chars[4][20] = {
  char(0), char(1), char(6), char(1), char(1), char(2), ' ', char(0), char(1), char(1), ' ', char(0), char(1), char(1), char(2), ' ', char(0), char(1), char(1), char(2),
  char(2), ' ',     char(4), ' ',     ' ',     char(2), ' ', char(2), ' ',     ' ',     ' ', char(2), ' ',     ' ',     char(2), ' ', char(2), ' ',     ' ',     ' ',
  char(7), ' ',     char(4), ' ',     ' ',     char(2), ' ', char(7), char(1), ' ',     ' ', char(7), char(1), char(1), char(6), ' ', char(7), ' ',     char(1), char(2),
  char(7), ' ',     char(4), ' ',     ' ',     char(2), ' ', char(7), char(3), char(3), ' ', char(7), ' ',     ' ',     char(4), ' ', char(7), char(3), char(3), char(2)
  };
  displayLogo(chars);

  delay(2000);
}

void displayStopLogo()
{
  // Creat a set of new characters
  const uint8_t stopLogo[][8] = {
    { B00000000, B00000001, B00000011, B00000111, B00001111, B00011111, B00011111, B00011111 }, // 0
    { B00000000, B00010000, B00011000, B00011100, B00011110, B00011111, B00011111, B00011111 }, // 1
    { B00011111, B00011111, B00011111, B00001111, B00000111, B00000011, B00000001, B00000000 }, // 2
    { B00011111, B00011111, B00011111, B00011110, B00011100, B00011000, B00010000, B00000000 }, // 3
    { B00001111, B00000111, B00000011, B00000001, B00000000, B00000000, B00000000, B00000000 }, // 4
    { B00000000, B00000000, B00000000, B00000000, B00010000, B00011000, B00011100, B00011110 }, // 5
    { B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111 }, // 6
    { B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111 }  // 7
  };

//void displayStopLogo()
//{

  customChars(stopLogo);

  char chars[4][20] = {
    char(0), char(7), char(7), char(1), ' ', char(7), char(7), char(7), char(7), ' ', char(0), char(7), char(7), char(7), char(1), ' ', char(7), char(7), char(7), char(1),
    char(2), char(7), char(5), char(4), ' ', ' ',     char(7), ' ',     ' ',   ' ', char(7), ' ',     ' ',     ' ',   char(7), ' ', char(7), ' ',     ' ',     char(7),
    char(5), char(4), char(7), char(1), ' ', ' ',     char(7), ' ',     ' ',   ' ', char(7), ' ',     ' ',     ' ',   char(7), ' ', char(7), char(7), char(7), char(3),
    char(2), char(7), char(7), char(3), ' ', ' ',     char(7), ' ',     ' ',   ' ', char(2), char(7), char(7), char(7), char(3), ' ', char(7), ' ',     ' ',     ' '
  };
  displayLogo(chars);

  barGraphicsLoaded = false;
  showingSpeeds     = false;
}

void setupBarGraph()
{
  // Creat a set of new characters
  const uint8_t bars[][8] = {
    { B00010000, B00010000, B00010000, B00010000, B00010000, B00010000, B00010000, B00000000 }, // 0 - 1 segment
    { B00011000, B00011000, B00011000, B00011000, B00011000, B00011000, B00011000, B00000000 }, // 1 - 2 segments
    { B00011100, B00011100, B00011100, B00011100, B00011100, B00011100, B00011100, B00000000 }, // 2 - 3 segments
    { B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B00000000 }, // 3 - 4 segments
    { B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00000000 }, // 4 - 5 segments
    { B00000001, B00000011, B00000111, B00001111, B00000111, B00000011, B00000001, B00000000 }, // 5 - < arrow rev
    { B00010000, B00011000, B00011100, B00011110, B00011100, B00011000, B00010000, B00000000 }, // 6 - > arrow fwd
    { B00001110, B00011011, B00010101, B00010111, B00010101, B00011011, B00001110, B00000000 }, // 7 - (c) symbol
  };

//void setupBarGraph()
//{

  customChars(bars);

  barGraphicsLoaded = true;
}

void customChars(const uint8_t chars[][8])
{
  for (int i = 0; i < 8; i++)
  {
    display.createChar(i, (uint8_t *)chars[i]);
  }
}

void displayLogo(const char chars[4][20])
{
#if LCD_DISPLAY
  for (int i = 0; i < 4; i++)
  {
    display.setCursor(0, i);
    displayChars(chars[i], 20);
  }
#endif
}


void displayChars(const char chars[20], int count)
{
#if LCD_DISPLAY
  for (int j = 0; j < count; j++)
  {
    display.write(chars[j]);
  }
#endif
}

#endif
//
/// command interpreter for serial console input
//

void processSerialInput(void) {

  byte uev = 0;
  char msgstr[32], dstr[32];

  if (Serial.available()) {

    char c = Serial.read();

    switch (c) {

      case 'n':

        // node config
        printConfig();

        // node identity
        Serial << F("> CBUS node configuration") << endl;
        Serial << F("> mode = ") << (config.FLiM ? "FLiM" : "SLiM") << F(", CANID = ") << config.CANID << F(", node number = ") << config.nodeNum << endl;
        Serial << endl;
        break;

      case 'e':

        // EEPROM learned event data table
        Serial << F("> stored events ") << endl;

        sprintf(msgstr, "  max events = %d, EVs per event = %d, bytes per event = %d", config.EE_MAX_EVENTS, config.EE_NUM_EVS, config.EE_BYTES_PER_EVENT);
        Serial << msgstr << endl;

        for (byte j = 0; j < config.EE_MAX_EVENTS; j++) {
          if (config.getEvTableEntry(j) != 0) {
            ++uev;
          }
        }

        Serial << F("  stored events = ") << uev << F(", free = ") << (config.EE_MAX_EVENTS - uev) << endl;
        Serial << F("  using ") << (uev * config.EE_BYTES_PER_EVENT) << F(" of ") << (config.EE_MAX_EVENTS * config.EE_BYTES_PER_EVENT) << F(" bytes") << endl << endl;

        Serial << F("  Ev#  |  NNhi |  NNlo |  ENhi |  ENlo | ");

        for (byte j = 0; j < (config.EE_NUM_EVS); j++) {
          sprintf(dstr, "EV%03d | ", j + 1);
          Serial << dstr;
        }

        Serial << F("Hash |") << endl;

        Serial << F(" --------------------------------------------------------------") << endl;

        // for each event data line
        for (byte j = 0; j < config.EE_MAX_EVENTS; j++) {

          if (config.getEvTableEntry(j) != 0) {
            sprintf(dstr, "  %03d  | ", j);
            Serial << dstr;

            // for each data byte of this event
            for (byte e = 0; e < (config.EE_NUM_EVS + 4); e++) {
              sprintf(dstr, " 0x%02hx | ", config.readEEPROM(config.EE_EVENTS_START + (j * config.EE_BYTES_PER_EVENT) + e));
              Serial << dstr;
            }

            sprintf(dstr, "%4d |", config.getEvTableEntry(j));
            Serial << dstr << endl;
          }
        }

        Serial << endl;

        break;

      // NVs
      case 'v':

        // note NVs number from 1, not 0
        Serial << "> Node variables" << endl;
        Serial << F("   NV   Val") << endl;
        Serial << F("  --------------------") << endl;

        for (byte j = 1; j <= config.EE_NUM_NVS; j++) {
          sprintf(msgstr, " - %02d : %3hd | 0x%02hx", j, config.readNV(j), config.readNV(j));
          Serial << msgstr << endl;
        }

        Serial << endl << endl;

        break;

      // CAN bus status
      case 'c':

        CBUS.printStatus();
        break;

      case 'h':
        // event hash table
        config.printEvHashTable(false);
        break;

      case 'y':
        // reset CAN bus and CBUS message processing
        CBUS.reset();
        break;

      case '*':
        // reboot
        config.reboot();
        break;

      case 'm':
        // free memory
        Serial << F("> free SRAM = ") << config.freeSRAM() << F(" bytes") << endl;
        break;

      case '\r':
      case '\n':
        Serial << endl;
        break;

      default:
        // Serial << F("> unknown command ") << c << endl;
        break;
    }
  }
}
