#ifndef BUTTON_H
#define BUTTON_H

class ButtonControl {
    int IN_PIN;
    volatile bool stableState = HIGH;
    bool prevStableState = HIGH;
    
    static ButtonControl* instance;

public:
    ButtonControl(int pin) : IN_PIN(pin) {
        pinMode(pin, INPUT_PULLUP);
        instance = this; 
    }

    static void IRAM_ATTR buttonInterrupt() {
        if (instance) {
            instance->stableState = digitalRead(instance->IN_PIN);
        }
    }

    void begin() {
        stableState = digitalRead(IN_PIN);
        prevStableState = stableState;
        attachInterrupt(digitalPinToInterrupt(IN_PIN), buttonInterrupt, CHANGE);
    }

    bool wasPressed() {
        bool pressedEdge = (prevStableState == HIGH && stableState == LOW);
        prevStableState = stableState;
        return pressedEdge;
    }

    bool isPressed() const {
        return stableState == LOW;
    }

    bool getButtonState() {
        return !digitalRead(IN_PIN);
    }
};

ButtonControl* ButtonControl::instance = nullptr;

#endif