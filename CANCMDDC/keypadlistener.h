// keypadlistener.h

#ifndef KEYPAD_LISTENER_H
#define KEYPAD_LISTENER_H

// From Key.h
/// Adapted from Key.h and made into a scoped enum.
enum class KeyState : byte { IDLE, PRESSED, HOLD, RELEASED };

void keypadEvent(char,KeyState);

class MyKeyboardListener : public KeyboardListener {
public:
    void keyPressed(char key, bool hold) override {
        Serial.print("Key ");
        Serial.print(key);
        Serial.print(" is pressed, hold = ");
        Serial.println(hold);
// For now, will deal with KEYPAD44 later
#if KEYPAD
        if (hold) keypadEvent(key,KeyState::HOLD);
        else keypadEvent(key,KeyState::PRESSED);
#endif
        key_pressed = key;
    }

    void keyReleased(char key) override {
        Serial.print("Released ");
        Serial.println(key);
#if KEYPAD
        keypadEvent(key,KeyState::RELEASED);
#endif
        key_pressed = 'X';
    }
};

#endif
