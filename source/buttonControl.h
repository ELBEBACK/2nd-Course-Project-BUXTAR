#include "esp32-hal-gpio.h"
#ifndef BUTTON_H
#define BUTTON_H

class ButtonControl {

    int IN_PIN;

    bool stableState = HIGH;
    bool lastReading = HIGH;
    bool prevStableState = HIGH;
    unsigned long lastChangeMs = 0;
    const unsigned long debounceMs = 40;

public: 
    ButtonControl(int pin) : IN_PIN(pin) {
        pinMode(pin, INPUT_PULLUP);
    }

    void begin() {
        stableState = digitalRead(IN_PIN);
        lastReading = stableState;
        prevStableState = stableState;
        lastChangeMs = millis();
    }

    void update() {
        bool r = digitalRead(IN_PIN);

        if (r != lastReading) {
        lastReading = r;
        lastChangeMs = millis();
        }

        if (millis() - lastChangeMs >= debounceMs) {
        stableState = lastReading;
        }
    }    

    bool isPressed() const {
        return stableState == LOW;
    }

    bool wasPressed() {
        update(); 

        bool pressedEdge = (prevStableState == HIGH && stableState == LOW);
        prevStableState = stableState;
        return pressedEdge;
    }

    bool getButtonState() {
        return !digitalRead(IN_PIN);
    }
};

#endif