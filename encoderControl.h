#ifndef ENCODER_H
#define ENCODER_H

#define NumModes 4

class EncoderControl {
    int SW_PIN;
    int DT_PIN;
    int CLK_PIN;

    int currentMode {0};
    int lastMode {0};
    int currentStateCLK {0};
    int lastStateCLK {0};

    unsigned long lastButtonPress = 0;

public:
    EncoderControl(int sw, int dt, int clk) : SW_PIN(sw), DT_PIN(dt), CLK_PIN(clk) {
        pinMode(clk, INPUT);
        pinMode(dt, INPUT);
        pinMode(sw, INPUT_PULLUP);
    };

    void setLastCLK(int curr_clk) {lastStateCLK = curr_clk;}
    int getLastCLK () const {return lastStateCLK;}
    int getLastMode() const {return lastMode;}
    void setLastMode(int mode) {lastMode = mode;}

    int SetMode(int curr_clk) {
        if(curr_clk != lastStateCLK && curr_clk == 1) {
            if(digitalRead(DT_PIN) != curr_clk) {
                currentMode++;                        
                if(currentMode > NumModes) {
                  currentMode = 1;
                }
            } else {
                currentMode--;
                if(currentMode <= 0) {
                  currentMode = NumModes;
                }
            }
            Serial.println(" | Mode: ");
            Serial.println(currentMode);
        }
        
        return currentMode;
    }

    void buttonHandler(int buttonState, bool* flag) {
        if(buttonState == LOW) {
            if(millis() - lastButtonPress > 50) {
                *flag = !(*flag);
            }
            lastButtonPress = millis();
        }
    }
};

#endif