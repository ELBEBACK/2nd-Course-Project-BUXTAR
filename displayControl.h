#ifndef DISPLAY_H
#define DISPLAY_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

class DisplayControl {
    int SCL_PIN;
    int SDA_PIN;

public:
    LiquidCrystal_I2C lcd;  

    DisplayControl(int scl, int sda) : 
        SCL_PIN(scl), 
        SDA_PIN(sda),
        lcd(0x27, 16, 2)  
    {
        //Wire.begin(SDA_PIN, SCL_PIN);
        //lcd.init();
        //lcd.backlight();
        //Serial.println("Display is ready!");
    }

    void printL1(const char* line) {
        lcd.setCursor(0, 0);
        lcd.print(line);
    }
    
    void printL2(const char* line) {
        lcd.setCursor(0, 1);
        lcd.print(line);
    }
};
#endif