#ifndef ENCODER_H
#define ENCODER_H

#define MODES 4

class EncoderControl {
    int SW_PIN;
    int DT_PIN;
    int CLK_PIN;

    int counter = 0;
    int currentStateCLK = 0;
    int lastStateCLK = 0;

public:
    EncoderControl(int sw, int dt, int clk) : SW_PIN(sw), DT_PIN(dt), CLK_PIN(clk) {
        pinMode(clk, INPUT);
        pinMode(dt, INPUT);
        pinMode(sw, INPUT_PULLUP);
    };

    int setLastCLK(int curr_clk){lastStateCLK = curr_clk;}
    int getLastCLK() {return lastStateCLK;}

    int SetMode(int curr_clk) {
        if(curr_clk != lastStateCLK && curr_clk == 1) {
            if(digitalRead(DT_PIN) != curr_clk) {
                counter--;                        //mods -- 
            } else {
                counter++;
            }
            Serial.println(" | Counter: ");
            Serial.println(counter);
        }
        lastStateCLK = curr_clk;
        int mode = counter - MODES * (counter / MODES);
        Serial.println("mode in set");
        Serial.println(mode);
        return mode;
    }

};

#endif