#ifndef SERVO_H
#define SERVO_H

#include <ESP32Servo.h>

class ServoControl {

    Servo servo_;

    int SERVO_PIN;
    int MOTOR_PIN;

public:
    ServoControl(int Spin, int Mpin) : SERVO_PIN(Spin),
                                MOTOR_PIN(Mpin),
                                servo_()
    {
        pinMode(MOTOR_PIN, OUTPUT);
        servo_.attach(SERVO_PIN);
        analogWrite(MOTOR_PIN, 255);
    }

    const int angles[5] = {0, 30, 75, 115, 155};
    
    
    void menu() {
        
        Serial.println("The servo only has 5 fixed positions");
        Serial.println("To get the servo back:              h   ");
        Serial.println("To turn to the position from 1-4:   1-4 ");
        Serial.println("To quit:                            q   ");
        
        while(1) {
            if(Serial.available()) {
                char cmd = Serial.read();
                
                switch(cmd) {
                    case 'h':
                        turn(angles[0]);
                        break;
                    case '1':
                        turn(angles[1]);
                        break;
                    case '2':
                        turn(angles[2]);
                        break;
                    case '3':
                        turn(angles[3]);
                        break;
                    case '4':
                        turn(angles[4]);
                    case 'q':
                        return;
                    default:
                        break;
                }
            }
  
            delay(1000);
            
        }

    }


    void turn(int angle) {
        delay(2000);
        servo_.write(angle);
        delay(2000);
    }

};

#endif