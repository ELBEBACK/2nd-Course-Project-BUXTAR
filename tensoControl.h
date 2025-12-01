#ifndef TENSO_CONTROL_H
#define TENSO_CONTROL_H

#include <Arduino.h>
#include <HX711.h>
#include <cmath>

#define ReferenceWeight 55

class TensoControl{

    int HX711_DT;
    int HX711_SCK;

    float calibration_factor = 480.0;

public:

    long zero_factor;

    TensoControl(int dt, int sck) : HX711_DT(dt),
                                    HX711_SCK(sck)
    {
        pinMode(HX711_SCK, OUTPUT);
        pinMode(HX711_DT, INPUT);
        zero_factor = readHX711();
        //Serial.println("Scale is ready!");
    }

    void menu() {

        Serial.println("To calibrate the scale, firstly tare, then calculate the factor.");
        Serial.println("To tare:                            t");
        Serial.println("To calculate calibration_factor:    c");
        Serial.println("To quit:                            q");

        while(1) {
            if(Serial.available()) {
                char cmd = Serial.read();
    
                if(cmd == 't') {
                    zero_factor = readHX711();
                    for (int i = 0; i < 9; ++i) {
                        zero_factor += readHX711();
                    }
                    zero_factor = zero_factor/10;
                    Serial.print("Taring is done. Zero factor: ");
                    Serial.println(zero_factor);
                } else if(cmd == 'c') {
                    long tmp = readHX711();
                    calibration_factor = (tmp - zero_factor)/ReferenceWeight;
                } else if(cmd == 'q') {
                    return;
                }
            }

            long raw_value = readHX711();
            float weight = (raw_value - zero_factor) / calibration_factor;
  
            Serial.print("Raw: ");
            Serial.print(raw_value);
            Serial.print(" | Weight: ");
            Serial.print(weight, 2);
            Serial.println(" g");
  
            delay(1000);
            
        }
    }

    float calc_calibf() {
        long tmp = readHX711();
        calibration_factor = (tmp - zero_factor)/ReferenceWeight;
        return calibration_factor;
    }

    float current_weight() {
        long raw_value = readHX711();
        float weight = (raw_value - zero_factor) / calibration_factor;
        return weight;
    }

    float tare() {
        zero_factor = readHX711();
            for (int i = 0; i < 9; ++i) {
                zero_factor += readHX711();
        }
        zero_factor = zero_factor   / 10.0;
        return zero_factor;
    }

    long readHX711() {
        long count = 0;
  
        while(digitalRead(HX711_DT) == HIGH) {
            yield();
        }
  
        // Читаем 24 бита данных
        for(int i = 0; i < 24; i++) {
            digitalWrite(HX711_SCK, HIGH);
            delayMicroseconds(1);
            count = count << 1;
            digitalWrite(HX711_SCK, LOW);
            delayMicroseconds(1);
    
            if(digitalRead(HX711_DT) == HIGH) {
                count++;
            }
        }
  
        // Устанавливаем канал и усиление для следующего чтения
        digitalWrite(HX711_SCK, HIGH);
        delayMicroseconds(1);
        digitalWrite(HX711_SCK, LOW);
        delayMicroseconds(1);
  
        // Преобразуем в signed 32-bit
        if(count & 0x800000) {
            count |= 0xFF000000;
        }
  
        return count;
    }

};


#endif