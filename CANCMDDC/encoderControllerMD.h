
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

// Wrapper class to provide the interface.
// It may be that I don't do it this way.
class encoderControllerClass
{
  private:
      EncoderMD &enc;
      EncoderEvent &enc_event;
      bool    ignorePush = false;
      int spinwheelClickPin;
  // -------------------------------------------------

  public:
      uint8_t       newPos = 0;
      uint8_t       lastPos = 0;
      unsigned long push = 0;
  // -------------------------------------------------

  // Constructor - initializes the member variables and state
  encoderControllerClass (uint8_t setPinA,
                          uint8_t setPinB,
                          uint8_t setPinS)
  {
#if SWAP_PINS
      enc = EncoderMD(setPinB, setPinA);
#else
      enc = EncoderMD(setPinA, setPinB);
#endif
// This action is not permitted - take reference of the event.
//      enc_event = EncoderEvent(enc);
      spinwheelClickPin = setPinS;
  }

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
