#ifndef ENCODER_H
#define ENCODER_H


class EncoderControl {
    int SW_PIN;
    int DT_PIN;
    int CLK_PIN;

    int currentChoice {0};
    int lastChoice {0};
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
    int getLastChoice() const {return lastChoice;}
    void setLastChoice(int mode) {lastChoice = mode;}

    void resetChoice() {
        currentChoice = 0;
        lastChoice = 0;
    }

    int SetChoice(int curr_clk, int option_num) {
        if(curr_clk != lastStateCLK && curr_clk == 1) {
            if(digitalRead(DT_PIN) != curr_clk) {
                currentChoice++;                        
                if(currentChoice > option_num) {
                  currentChoice = 1;
                }
            } else {
                currentChoice--;
                if(currentChoice <= 0) {
                  currentChoice = option_num;
                }
            }
            Serial.println(" | Mode: ");
            Serial.println(currentChoice);
        }
        
        return currentChoice;
    }

    bool buttonHandler(int buttonState) {
        if(buttonState == LOW) {
            if(millis() - lastButtonPress > 50) {
                lastButtonPress = millis();
                delay(200);
                return true;
            }
            lastButtonPress = millis();
        }
        return false;
    }
};

#endif