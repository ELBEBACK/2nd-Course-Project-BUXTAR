#ifndef PUMP_CONTROL_H
#define PUMP_CONTROL_H

#include <Arduino.h>

class PumpControl{
    int IN1_PIN;

public: 

    PumpControl(int pin1) : IN1_PIN(pin1) {
        pinMode(IN1_PIN, OUTPUT);
        stopPump();
        //Serial.println("Pump is ready!");
    }

    void menu() {

        Serial.println("To start the flow:  f");
        Serial.println("To stop the flow:   s");
        Serial.println("To quit:            q");
        
        while(1) {
            if(Serial.available()) {
                char cmd = Serial.read();
    
                if(cmd == 's') {
                    stopPump();
                } else if(cmd == 'f') {
                    setPumpForward();
                }
            }
  
            delay(1000);
            
        }
    }

    void setPumpForward() {
        digitalWrite(IN1_PIN, HIGH);
    }

    void stopPump() {
        digitalWrite(IN1_PIN, LOW);
    }
};


#endif