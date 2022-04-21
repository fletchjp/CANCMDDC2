
/**
 * Copyright (c) 2020 David Radcliffe
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License
 */

// Adapted to use Martin Da Costa's encoder library

#ifndef ENCODERCONTROLLERMD_H
#define ENCODERCONTROLLERMD_H 


// Analogue (PWM) Train Controller.
//
// Class: encoderControllerClass
//
// Methods:
//                       encoderControllerClass(int setPinA, int setPinB, int setPinS)            Class Constructor

#include "EncoderMD.h"
#include <IoAbstraction.h>
#include <TaskManagerIO.h>

/// @brief Declaration of Arduino abstraction for Input/Output.
/// This works for pins connected directly to an Arduino
IoAbstractionRef arduinoIo = ioUsingArduino();


/// Swap the pins to get the opposite action
/// This is equivalent to changing over the wires.
#define SWAP_PINS 1

/// an LED that flashes as the encoder changes
const int ledOutputPin = 13;

volatile byte lastPins = 0;

/// The pins onto which the rotary encoders switches are connected
const int spinwheelClickPin1 = 38; /// SW on encoder1
const int spinwheelClickPin2 = 40; /// SW on encoder2
/// In this case the pins are passed to Martin's code instead.
const int encoderAPin1 = A8;  /// CLK on encoder1 
const int encoderBPin1 = A9;  /// DT  on encoder1
const int encoderAPin2 = A10; /// CLK on encoder2 
const int encoderBPin2 = A11; /// DT  on encoder2
/// Encoder names to be consistent
const char encoder_name1 = '1';
const char encoder_name2 = '2';

/// the maximum (0 based) value that we want the encoder to represent.
const int maximumEncoderValue = 128;

#if SWAP_PINS
/// Use thess settings for right hand turn to increase the value.
EncoderMD encoder1(encoderBPin1,encoderAPin1);
EncoderMD encoder2(encoderBPin2,encoderAPin2);
#else
/// Use these settings for left hand turn to increase the value.
EncoderMD encoder1(encoderAPin1,encoderBPin1);
EncoderMD encoder2(encoderAPin2,encoderBPin2);
#endif

/// @brief The PCI setup is specific to the pins being used, here A8 to A11.
/// PCI does not work on the MEGA pins A0 to A7.
void setupPCI()
{
  cli();
  PCICR  |= 0b00000100;  //Set Pin Change Interrupt on Register K
  PCMSK2 |= 0b00001111;  //Set A8, A9, A10 & A11 for interrupt
  sei();
}

/// @brief EncoderEvent - now uses class variable to support multiple encoders.
///
/// char encoderName variable is provided so that outputs can be distinguished.
class EncoderEvent : public BaseEvent {
private:
/// Used to note when the encoder position has changed.
    char encoderName;
    EncoderMD &encoder;
    static const uint32_t NEXT_CHECK_INTERVAL = 60UL * 1000000UL; // 60 seconds away, maximum is about 1 hour.
public:
    boolean TurnDetected;
    int RotaryPosition;
    int PrevPosition;
    EncoderEvent(EncoderMD &encoder_, char name_= '0') : encoderName(name_), encoder(encoder_)  {
      RotaryPosition = 0; PrevPosition = 0;
    }
    /// @brief timeOfNextCheck now replaced by call from ISR calling markTriggeredAndNotify().
    uint32_t timeOfNextCheck() override {
        return 250UL * 1000UL; // every 100 milliseconds we roll the dice
    }
    void exec() override {
         //Serial.print("exec called with ");
         RotaryPosition = encoder.getPosition();
         //Serial.println(RotaryPosition);
         TurnDetected = (RotaryPosition != PrevPosition);
         if (TurnDetected)  {         
           PrevPosition = RotaryPosition; // Save previous position in variable
           Serial.print(encoderName);
           Serial.print(" ");
           Serial.println(RotaryPosition);
           // here we turn the led on and off as the encoder moves.
           ioDeviceDigitalWriteS(arduinoIo, ledOutputPin, RotaryPosition % 2);
         }
    }
    /**
     * We should always provide a destructor.
     */
    ~EncoderEvent() override = default;
};

/// Declare encoderEvent instances for each encoder.
EncoderEvent encoderEvent1(encoder1,encoder_name1);
EncoderEvent encoderEvent2(encoder2,encoder_name2);

/// @brief When the spinwheel1 is clicked, this function will be run as we registered it as a callback
///
/// There have to be separate routines for each encoder to distinguish the variables.
void onSpinwheelClicked1(pinid_t pin, bool heldDown) { //, MyEncoder &encoder, EncoderEvent &encoderEvent) {
  Serial.print("Button 1 pressed ");
  Serial.println(heldDown ? "Held" : "Pressed");
    if (encoderEvent1.RotaryPosition == 0) {  // check if button was already pressed
       } else {
        encoderEvent1.RotaryPosition=0; // Reset position to ZERO
        encoder1.setPosition(encoderEvent1.RotaryPosition);
        // This is not available here as the encoders have not yet been declared.
        //Serial.print(encoders[0].encoderController.get_name()); // Index offset
        Serial.print("1 ");
        Serial.println(encoderEvent1.RotaryPosition);
        encoderEvent1.PrevPosition = encoderEvent1.RotaryPosition;
      }
}

/// @brief When the spinwheel2 is clicked, this function will be run as we registered it as a callback
void onSpinwheelClicked2(pinid_t pin, bool heldDown) { //, MyEncoder &encoder, EncoderEvent &encoderEvent) {
  Serial.print("Button 2 pressed ");
  Serial.println(heldDown ? "Held" : "Pressed");
    if (encoderEvent2.RotaryPosition == 0) {  // check if button was already pressed
       } else {
        encoderEvent2.RotaryPosition=0; // Reset position to ZERO
        encoder2.setPosition(encoderEvent2.RotaryPosition);
        // This is not available here as the encoders have not yet been declared.
        //Serial.print(encoders[1].encoderController.get_name()); // Index offset
        Serial.print("2 ");
        Serial.println(encoderEvent2.RotaryPosition);
        encoderEvent2.PrevPosition = encoderEvent2.RotaryPosition;
      }
}

/// This needs to be called from within the main setup routine.
void setup_encoders() {
  setupPCI();
  encoder1.setLimits(0,maximumEncoderValue);
  encoder2.setLimits(0,maximumEncoderValue);
  encoder1.setPosition (0);
  encoder1.setWrap (0);
  encoder2.setPosition (0);
  encoder2.setWrap (0);
  
  // here we initialise as output the output pin we'll use
  ioDevicePinMode(arduinoIo, ledOutputPin, OUTPUT);

  // First we set up the switches library, giving it the task manager and tell it to use arduino pins
  // We could also have chosen IO through an i2c device that supports interrupts.
  // If you want to use PULL DOWN instead of PULL UP logic, change the true to false below.
  switches.initialise(arduinoIo, true);

  // now we add the switches, we dont want the spinwheel button to repeat, so leave off the last parameter
  // which is the repeat interval (millis / 20 basically) Repeat button does repeat as we can see.
  switches.addSwitch(spinwheelClickPin1, onSpinwheelClicked1); //, encoder1, encoderEvent1);
  switches.addSwitch(spinwheelClickPin2, onSpinwheelClicked2); //, encoder1, encoderEvent1);

  taskManager.registerEvent(&encoderEvent1);
  taskManager.registerEvent(&encoderEvent2);
    
  Serial.println("2 encoders and encoderEvents are initialised!");
  
}

// @brief ISR routine now calls the encoder and also the encoderEvent as well.
ISR(PCINT2_vect)  /// Pin A8 to A11 interrupt vector
{
byte pins = PINK & 0b00001111;
byte change = pins ^ lastPins;
lastPins = pins;

 if ( change & 0b000011)
 {
    encoder1.encoderISR();
    encoderEvent1.markTriggeredAndNotify();
 }
 if ( change & 0b00001100)
 {
    encoder2.encoderISR();
    encoderEvent2.markTriggeredAndNotify();
 }
}

// Wrapper class to provide the interface.
// It may be that I don't do it this way.
class encoderControllerClass
{
  private:
      EncoderMD &enc;
      EncoderEvent &enc_event;
      bool    ignorePush = false;
      int spinwheelClickPin;
      char encoder_name;
  // -------------------------------------------------

  public:
      uint8_t       newPos = 0;
      uint8_t       lastPos = 0;
      unsigned long push = 0;
  // -------------------------------------------------
  // previous constructor removed.
  // -------------------------------------------------
  /// Constructor from encoder which already exists
  encoderControllerClass (EncoderMD &encoder,uint8_t setPinS,char enc_name = 'x') : enc(encoder) {
      spinwheelClickPin = setPinS;
      encoder_name = enc_name;
  }
  /// Added for use in onSpinWheelClicked routines
  char get_name() const { return encoder_name; }
  // -------------------------------------------------
  bool read()
  {
      uint8_t val = enc.getPosition();

      if (val == 255) // button pressed (This needs changing)
      {
          if (ignorePush) return false;

          if (push == 0) push = millis();
          enc.setPosition(0);
          newPos = 0;
          lastPos = 0;
          return true;
      }
      else // button released
      {
          ignorePush = false;
          push = 0;
      }

      if (val == 1) // EStop value - not needed
      {
          if (lastPos == 0)
          {
              enc.setPosition(2);
              val = 2;
          }
          else // == 2
          {
              enc.setPosition(0);
              val = 0;
          }
      }

      newPos = val;
      return false;
  }

  void write(uint8_t value)
  {
      enc.setPosition(value);
      newPos = value;
      lastPos = value;
      push = 0;
      ignorePush = true;
  }

  // -------------------------------------------------

  private:

  // -------------------------------------------------
  // -------------------------------------------------
};


#endif
