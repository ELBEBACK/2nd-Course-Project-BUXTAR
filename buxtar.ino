#include "pumpControl.h"
#include "tensoControl.h"
#include "displayControl.h"
#include "ledsControl.h"
#include "encoderControl.h"
#include "servoControl.h"

//scales
#define HX711_DT1 34
#define HX711_SCK2 13
#define HX711_DT1 34
#define HX711_SCK2 13
#define HX711_DT1 34
#define HX711_SCK2 13
#define HX711_DT1 34
#define HX711_SCK2 13
// pump
#define IN1_PIN 12
// display
#define SCL_PIN 22
#define SDA_PIN 21
//leds
#define LED_PIN 16
#define COUNT_LED 96
//encoder
#define SW_PIN 32
#define DT_PIN 33
#define CLK_PIN 35
//servo
#define SERVO_PIN 23
#define MOTOR_PIN 18 

enum Modes {
    auto_mode         = 1,
    auto_same         = 2,
    russian_roulete_1 = 3,
    russian_roulete_2 = 4
};

bool is_mode_selected = 0;
int mode;
int currentStateCLK;                 

EncoderControl encoder (SW_PIN, DT_PIN, CLK_PIN);
LedsControl    leds    (LED_PIN, COUNT_LED);
DisplayControl display (SCL_PIN, SDA_PIN);
PumpControl    pump1   (IN1_PIN);
TensoControl   scale1  (HX711_DT, HX711_SCK);
ServoControl   servo   (SERVO_PIN, MOTOR_PIN);

void setup() {
  Serial.begin(115200);
  
  //эта хуета только здесь не трогать!!!!!
  display.lcd.init();                     
  //display.lcd.backlight();
  display.lcd.setCursor(10, 2);
  display.lcd.print("setup");
  //-----------------------------------
  leds.pixels.begin();
  leds.pixels.show();
  //-----------------------------------
  encoder.setLastCLK(digitalRead(CLK_PIN));
  //lastStateCLK = digitalRead(CLK_PIN);
  //-----------------------------------
  Serial.print("Zero factor: ");
  Serial.println(scale1.zero_factor);
  Serial.println("Menu:");
  Serial.println("p - PumpControl");
  Serial.println("s - ScaleControl");
  Serial.println("q - to return to the menu");
  Serial.println("t - servo menu to turn servo");

  display.lcd.clear();
  display.lcd.setCursor(1, 0);
  display.lcd.print("Select mode");
  display.lcd.setCursor(1, 1);
  display.lcd.print("via encoder");

}

void loop() {
  /*for(int i = 0; i < COUNT_LED; i++) {
    leds.pixels.setPixelColor(i, leds.pixels.Color(0,50,0)); // Назначаем для первого светодиода цвет "Зеленый"
  }
  leds.pixels.show();*/

  //-------------------------------------------------------------------

  if(!is_mode_selected) {
      currentStateCLK = digitalRead(CLK_PIN);
      mode = encoder.SetMode(currentStateCLK);

      //Serial.println(mode);

      if(mode != encoder.getLastMode()) {
          switch(mode) {
              case auto_mode:
                display.lcd.clear();
                display.lcd.print("mode 1");
                break;
              case auto_same: 
                display.lcd.clear();
                display.lcd.print("mode 2");
                break;
              case russian_roulete_1: 
                display.lcd.clear();
                display.lcd.print("mode 3");
                break;
              case russian_roulete_2: 
                display.lcd.clear();
                display.lcd.print("mode 4");
                break;
              default: Serial.println("mode error");
                      Serial.println(mode);
                      break;
          }
          encoder.setLastMode(mode);
      }
      encoder.setLastCLK(digitalRead(CLK_PIN));

      encoder.buttonHandler(digitalRead(SW_PIN), &is_mode_selected);
  }
  //Serial.println(" | is mode selected ");
  //Serial.println(is_mode_selected);
  //Serial.println(" | mode ");
  //Serial.println(mode);

  //-------------------------------------------------------------------
  if(Serial.available()) {
    char cmd = Serial.read();
    
    if(cmd == 'p') {
      pump1.menu();
    }
    else if(cmd == 's') {
      scale1.menu();  
    }
    else if (cmd == 'r') {
      Serial.println("> Performing software restart...");
      delay(100);
      Serial.flush();
      ESP.restart();
    } 
    else if (cmd == 'b') {
      Serial.println("> Performing full chip restart...");
      delay(100);
      Serial.flush();
      esp_restart();
    }
    else if (cmd == 'c') {
      display.lcd.clear();
    } 
    else if (cmd == 't')
    {
      servo.menu();
    }

  }
 
  delay(1000);
}
