#include "pumpControl.h"
#include "tensoControl.h"
#include "displayControl.h"
#include "ledsControl.h"
#include "encoderControl.h"
#include "servoControl.h"
#include <string>

//scales
#define HX711_DT1 34
#define HX711_SCK1 13
#define HX711_DT2 15
#define HX711_SCK2 2
#define HX711_DT3 0
#define HX711_SCK3 4
#define HX711_DT4 5
#define HX711_SCK4 17
// pump
#define IN1_PIN 12
#define IN2_PIN 19
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

/////////FIRST LEVEL//////////
enum Modes {
    auto_mode           = 1,
    russian_roulette  = 2,
    manual_mode         = 3,
    captcha             = 4,
};
//////////////////////////////

////////SECOND LEVEL//////////
enum Auto {
    autoPos1               = 1,
    autoPos2               = 2,
    autoPos3               = 3,
    autoPos4               = 4,
    quitAuto               = 5,
};

enum ManualModes {
    scaling             = 1,
    servomotor          = 2,
    pumping             = 3,
    reset               = 4,
    reboot              = 5,
    quitMM              = 6,
};

/////////THIRD LEVEL//////////
//////////editing/////////////
enum AutoEditing {
    proportions         = 1,
    volume              = 2,
    quitAE              = 3,
};

//////////scaling/////////////
enum ChoiceOfScales {
    firstS              = 1,
    secondS             = 2,
    thirdS              = 3,
    fourthS             = 4,
    quitCOS             = 5,
};
int which_scale = 0;

enum ScalingOptions {
    weight              = 1,
    taring              = 2,
    calibfac_calc       = 3,
    quitSO              = 4,
};
//////////////////////////////

////////////servo/////////////
enum ServoPositioning {
    firstPos            = 1,
    secondPos           = 2,
    thirdPos            = 3,
    fourthPos           = 4,
    home                = 5,
    quitSP              = 6,
};
//////////////////////////////

////////////pump//////////////
enum ChoiceOfPumps {
    firstP              = 1,
    secondP             = 2,
    bothP               = 3,
    quitCOP             = 4,
};
int which_pumps = 0;
int duration = 5;

enum PumpingOptions {
    start               = 1,
    stop                = 2,
    timePO              = 3,
    quitPO              = 4,
};
//////////////////////////////


//menu branch status
//1st lvl
bool is_mode_selected             = 0;
//2nd lvl
bool is_automode_selected         = 1;
bool is_russian_roulette_selected = 1;
bool is_manual_selected           = 1;
bool is_captcha_selected          = 1;
//3rd lvl 
bool is_scaling_submenu_selected  = 1;
bool is_servo_submenu_selected    = 1;
bool is_pump_submenu_selected     = 1;
//4th lvl
bool is_scale_selected            = 1;
bool is_pump_selected             = 1;
//5th lvl
bool is_time_set                  = 1;


int option;
int currentStateCLK;                 


EncoderControl encoder (SW_PIN, DT_PIN, CLK_PIN);
LedsControl    leds    (LED_PIN, COUNT_LED);
DisplayControl display (SCL_PIN, SDA_PIN);
ServoControl   servo   (SERVO_PIN, MOTOR_PIN);
PumpControl    pump[2]  = {
  PumpControl(IN1_PIN),
  PumpControl(IN2_PIN),
};

TensoControl   scale[4] = {
  TensoControl(HX711_DT1, HX711_SCK1),
  TensoControl(HX711_DT2, HX711_SCK2),
  TensoControl(HX711_DT3, HX711_SCK3),
  TensoControl(HX711_DT4, HX711_SCK4)
};

void setup() {
  Serial.begin(115200);
  
  display.lcd.init();                     
  display.lcd.backlight();
  //-----------------------------------
  leds.strip.begin();
  leds.strip.show();
  leds.strip.setBrightness(50);
  //-----------------------------------
  encoder.setLastCLK(digitalRead(CLK_PIN));
  //-----------------------------------
  Serial.print(" | Setup  ");
  

  display.lcd.clear();
  display.printL2("via Encoder");
  display.printL1("Select mode");

}

void loop() {

  if(!is_mode_selected) {
      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 4);


      if(option != encoder.getLastChoice()) {
          display.lcd.clear();
          switch(option) {
              case auto_mode:
                display.printL1("Preset mode");
                break;
              case russian_roulette: 
                display.printL1("Russian Roulette");
                break;
              case manual_mode: 
                display.printL1("Manual control");
                break;
              case captcha: 
                display.printL1("Soberity captcha");
                break;
              default: 
                      Serial.println("mode error");
                      Serial.println(option);
                      break;
          }
          encoder.setLastChoice(option);
      }
      encoder.setLastCLK(digitalRead(CLK_PIN));

          if (encoder.buttonHandler(digitalRead(SW_PIN))) {
            is_mode_selected = 1;
            display.lcd.clear();
            encoder.resetChoice();
            switch(option) {
              case auto_mode:
                display.printL1("Choose preset");
                display.printL2("to edit");
                is_automode_selected = 0;
                break;
              case russian_roulette: 
                display.printL1("Russian Roulette");
                is_russian_roulette_selected = 0;
                break;
              case manual_mode: 
                display.printL1("Select & Control"); 
                display.printL2("control");
                is_manual_selected = 0;
                break;
              case captcha: 
                display.printL1("Prove that you");
                display.printL2("are sober enough");
                is_captcha_selected = 0;
                break;
                  default: Serial.println("mode error");
                          Serial.println(option);
                          break;
            }
          }
  }

  if(!is_manual_selected) {
      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 6);


      if(option != encoder.getLastChoice()) {
          display.lcd.clear();
          switch(option) {
              case scaling:
                display.printL1("Scaling   manual"); 
                display.printL2("            mode");
                break;
              case servomotor: 
                display.printL1("Servo     manual");
                display.printL2("            mode");
                break;
              case pumping: 
                display.printL1("Pumping   manual");
                display.printL2("            mode");
                break;
              case reset: 
                display.printL1("To perform ");
                display.printL2("software reset");
                break;
              case reboot: 
                display.printL1("To perform ");
                display.printL2("chip reboot");
                break;
              case quitMM: 
                display.printL1("Back");
                break;
              default: 
                      Serial.println("mode error");
                      Serial.println(option);
                      break;
          }
          encoder.setLastChoice(option);
      }
      encoder.setLastCLK(digitalRead(CLK_PIN));

          if (encoder.buttonHandler(digitalRead(SW_PIN))) {
            is_manual_selected = 1;
            display.lcd.clear();
            encoder.resetChoice();
            switch(option) {
              case scaling:
                display.printL1("Manual scaling");
                display.printL2("Choose from 1-4");
                is_scaling_submenu_selected = 0;
                break;
              case servomotor: 
                display.printL1("Manual rotation");
                display.printL2("Choose action");
                is_servo_submenu_selected = 0;
                break;
              case pumping: 
                display.printL1("Manual pumping");
                display.printL2("Choose from 1-2");
                is_pump_submenu_selected = 0;
                break;
              case reset: 
                display.printL1("Software"); 
                display.printL2("restart...");
                delay(100);
                display.lcd.clear();
                ESP.restart();
                break;
              case reboot: 
                display.printL1("Full chip"); 
                display.printL2("restart...");
                delay(100);
                display.lcd.clear();
                esp_restart();
                break;
              case quitMM: 
                display.printL1("Back to"); 
                display.printL2("main menu");
                is_mode_selected = 0;
                break;
                  default: Serial.println("mode error");
                          Serial.println(option);
                          break;
            }
          }
  }


  else if (!is_scaling_submenu_selected) {
      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 5);


      if(option != encoder.getLastChoice()) {
          display.lcd.clear();
          switch(option) {
              case firstS:
                display.printL1("Scale 1");
                leds.circleUno(4, leds.strip.Color(50, 0, 50));
                for (int i = 4; i > 0; --i) {
                  if(i != 4) {
                    leds.circleUnoOff(i);
                  }
                }
                break;
              case secondS: 
                display.printL1("Scale 2");
                leds.circleUno(3, leds.strip.Color(50, 0, 50));
                for (int i = 4; i > 0; --i) {
                  if(i != 3) {
                    leds.circleUnoOff(i);
                  }
                }
                break;
              case thirdS: 
                display.printL1("Scale 3");
                leds.circleUno(2, leds.strip.Color(50, 0, 50));
                for (int i = 4; i > 0; --i) {
                  if(i != 2) {
                    leds.circleUnoOff(i);
                  }
                }
                break;
              case fourthS: 
                display.printL1("Scale 4");
                leds.circleUno(1, leds.strip.Color(50, 0, 50));
                for (int i = 4; i > 0; --i) {
                  if(i != 1) {
                    leds.circleUnoOff(i);
                  }
                }
                break;
              case quitCOS: 
                display.printL1("Back");
                for (int i = 4; i > 0; --i) {
                    leds.circleUnoOff(i);
                }
                break;
              default: Serial.println("mode error");
                      Serial.println(option);
                      break;
          }
          encoder.setLastChoice(option);
      }
      encoder.setLastCLK(digitalRead(CLK_PIN));

      if (encoder.buttonHandler(digitalRead(SW_PIN))) {
        is_scaling_submenu_selected = 1;
        is_scale_selected = 0;
        display.lcd.clear();
        encoder.resetChoice();
        switch(option) {
            case firstS:
                display.printL1("Scale 1");
                display.printL2("Choose action");
                which_scale = 1;
                break;
            case secondS: 
                display.printL1("Scale 2");
                display.printL2("Choose action");
                which_scale = 2;
                break;
            case thirdS: 
                display.printL1("Scale 3");
                display.printL2("Choose action");
                which_scale = 3;
                break;
            case fourthS: 
                display.printL1("Scale 4");
                display.printL2("Choose action");
                which_scale = 4;
                break;
            case quitCOS:
                display.printL1("Back to"); 
                display.printL2("manual menu");
                is_manual_selected = 0;
                is_scale_selected = 1;
                break;
            default: Serial.println("mode error");
                      Serial.println(option);
                      break;
        }
      }
  }


  else if (!is_scale_selected) {
      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 4);


      if(option != encoder.getLastChoice()) {
          display.lcd.clear();
          switch(option) {
              case weight:
                display.printL1("To get weight");
                break;
              case taring:
                display.printL1("(!)To tare");
                break;
              case calibfac_calc: 
                display.printL1("(!)To calculate ");
                display.printL2("calib. factor");
                break;
              case quitSO: 
                display.printL1("Back");
                break;
              default: Serial.println("mode error");
                      Serial.println(option);
                      break;
          }
          encoder.setLastChoice(option);
      }
      encoder.setLastCLK(digitalRead(CLK_PIN));

      if (encoder.buttonHandler(digitalRead(SW_PIN))) {
        display.lcd.clear();
        encoder.resetChoice();
        switch(option) {
              case weight:
                display.printL1("Weight:");

                display.lcd.setCursor(0, 1);
                display.lcd.print(scale[which_scale].current_weight());
                break;
              case taring:
                display.printL1("Zero factor:");
                display.lcd.setCursor(0, 1);
                display.lcd.print(scale[which_scale - 1].tare());
                break;
              case calibfac_calc: 
                display.printL1("Calib. factor:");
                display.lcd.setCursor(0, 1);
                display.lcd.print(scale[which_scale - 1].calc_calibf());
                break;
              case quitSO: 
                display.printL1("Scales");
                is_scaling_submenu_selected = 0;
                is_scale_selected = 1;
                break;
              default: Serial.println("mode error");
                      Serial.println(option);
                      break;
        }
      }
  }

  else if (!is_servo_submenu_selected) {
      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 6);


      if(option != encoder.getLastChoice()) {
          display.lcd.clear();
          switch(option) {
              case firstPos:
                display.printL1("Turn until ");
                display.printL2("first position");

                leds.circleUno(4, leds.strip.Color(50, 0, 50));
                for (int i = 4; i > 0; --i) {
                  if(i != 4) {
                    leds.circleUnoOff(i);
                  }
                }
                break;
              case secondPos:
                display.printL1("Turn until ");
                display.printL2("second position");

                leds.circleUno(3, leds.strip.Color(50, 0, 50));
                for (int i = 4; i > 0; --i) {
                  if(i != 3) {
                    leds.circleUnoOff(i);
                  }
                }
                break;
              case thirdPos: 
                display.printL1("Turn until");
                display.printL2("third position");

                leds.circleUno(2, leds.strip.Color(50, 0, 50));
                for (int i = 4; i > 0; --i) {
                  if(i != 2) {
                    leds.circleUnoOff(i);
                  }
                }
                break;
              case fourthPos: 
                display.printL1("Turn until");
                display.printL2("fourth position");

                leds.circleUno(1, leds.strip.Color(50, 0, 50));
                for (int i = 4; i > 0; --i) {
                  if(i != 1) {
                    leds.circleUnoOff(i);
                  }
                }
                break;
              case home: 
                display.printL1("Get back home");

                for (int i = 4; i > 0; --i) {
                    leds.circleUnoOff(i);
                }
                break;
              case quitSP:
                display.printL1("Back");
                break;
              default: Serial.println("mode error");
                      Serial.println(option);
                      break;
          }
          encoder.setLastChoice(option);
      }
      encoder.setLastCLK(digitalRead(CLK_PIN));

      if (encoder.buttonHandler(digitalRead(SW_PIN))) {
        display.lcd.clear();
        encoder.resetChoice();
        switch(option) {
              case firstPos:
                display.printL1("Position 1");
                servo.turn(servo.angles[1]);
                break;
              case secondPos:
                display.printL1("Position 2");
                servo.turn(servo.angles[2]);
                break;
              case thirdPos: 
                display.printL1("Position 3");
                servo.turn(servo.angles[3]);
                break;
              case fourthPos: 
                display.printL1("Position 4");
                servo.turn(servo.angles[4]);
                break;
              case home: 
                display.printL1("Position 0");
                servo.turn(servo.angles[0]);
                break;
              case quitSP:
                display.printL1("Back to"); 
                display.printL2("manual menu");
                is_manual_selected = 0;
                is_servo_submenu_selected = 1;
                break;
              default: Serial.println("mode error");
                      Serial.println(option);
                      break;
        }
      }
  }

  else if (!is_pump_submenu_selected ) {
      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 4);


      if(option != encoder.getLastChoice()) {
          display.lcd.clear();
          switch(option) {
              case firstP:
                display.printL1("Pump 1");
                break;
              case secondP:
                display.printL1("Pump 2");
                break;
              case bothP: 
                display.printL1("Pump 1 & 2 ");
                break;
              case quitCOP: 
                display.printL1("Back");
                break;
              default: Serial.println("mode error");
                      Serial.println(option);
                      break;
          }
          encoder.setLastChoice(option);
      }
      encoder.setLastCLK(digitalRead(CLK_PIN));

      if (encoder.buttonHandler(digitalRead(SW_PIN))) {
        display.lcd.clear();
        encoder.resetChoice();
        is_pump_submenu_selected = 1;
        is_pump_selected = 0;
        
        switch(option) {
              case firstP:
                display.printL1("Pump 1");
                display.printL1("Choose action");
                which_pumps = 1;
                break;
              case secondP:
                display.printL1("Pump 2");
                display.printL1("Choose action");
                which_pumps = 2;
                break;
              case bothP: 
                display.printL1("Pump 1 & 2 ");
                display.printL1("Choose action");
                which_pumps = 3;
                break;
              case quitCOP: 
                display.printL1("Main menu");
                is_manual_selected = 0;
                is_pump_selected = 1;
                break;
              default: Serial.println("mode error");
                      Serial.println(option);
                      break;
        }
      }
  }

  
  else if (!is_pump_selected) {
      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 4);


      if(option != encoder.getLastChoice()) {
          display.lcd.clear();
          switch(option) {
              case start:
                display.printL1("(!)To start flow");
                display.printL2("Duration:");

                display.lcd.setCursor(11, 1);
                display.lcd.print(duration);
                break;
              case stop:
                display.printL1("To stop the flow");
                display.printL2("in emergency");
                break;
              case timePO: 
                display.printL1("To set duration");
                break;
              case quitPO: 
                display.printL1("Back");
                break;
              default: Serial.println("mode error");
                      Serial.println(option);
                      break;
          }
          encoder.setLastChoice(option);
      }
      encoder.setLastCLK(digitalRead(CLK_PIN));

      if (encoder.buttonHandler(digitalRead(SW_PIN))) {
        display.lcd.clear();
        encoder.resetChoice();
        switch(option) {
              case start:
                display.printL1("Flow has started"); 
                if (which_pumps < 1 || which_pumps > 3) break;
                if (which_pumps == 3) {
                  pump[0].setPumpForward();
                  pump[1].setPumpForward();
                  delay(duration * 1000);
                  pump[0].stopPump();
                  pump[1].stopPump();
                }
                pump[which_pumps - 1].setPumpForward();
                delay(duration * 1000);
                pump[which_pumps - 1].stopPump();

                break;
              case stop:
                display.printL1("Stopping"); 
                display.printL2("the flow..");
                
                if (which_pumps < 1 || which_pumps > 3) break;
                if (which_pumps == 3) {
                  pump[0].stopPump();
                  pump[1].stopPump();
                }
                pump[which_pumps - 1].stopPump();
                break;
              case timePO: 
                display.printL1("Duration settings");
                is_time_set = 0;
                is_pump_selected = 1;
                break;
              case quitPO: 
                display.printL1("Pumps");
                is_pump_submenu_selected = 0;
                is_pump_selected = 1;
                break;
              default: Serial.println("mode error");
                      Serial.println(option);
                      break;
        }
      }
  }


  else if (!is_time_set) {
      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 5);


      if(option != encoder.getLastChoice()) {
          display.lcd.clear();
          switch(option) {
              case 1:
                display.printL1("Set duration to");
                display.printL2("               5");
                break;
              case 2:
                display.printL1("Set duration to");
                display.printL2("              10");
                break;
              case 3: 
                display.printL1("Set duration to");
                display.printL2("              15");
                break;
              case 4: 
                display.printL1("Set duration to");
                display.printL2("              20");
                break;
              case 5: 
                display.printL1("Back");
                break;
              default: Serial.println("mode error");
                      Serial.println(option);
                      break;
          }
          encoder.setLastChoice(option);
      }
      encoder.setLastCLK(digitalRead(CLK_PIN));

      if (encoder.buttonHandler(digitalRead(SW_PIN))) {
        display.lcd.clear();
        encoder.resetChoice();
        is_time_set = 1;
        is_pump_selected = 0;
        switch(option) {
              case 1:
                display.printL1("Duration is set!");
                duration = 5;
                break;
              case 2:
                display.printL1("Duration is set!");
                duration = 10;
                break;
              case 3: 
                display.printL1("Duration is set!");
                duration = 15;
                break;
              case 4: 
                display.printL1("Duration is set!");
                duration = 20;
                break;
              case 5: 
                display.printL1("Pumping options");
                break;
              default: Serial.println("mode error");
                      Serial.println(option);
                      break;
        }
      }
  }
  
 
  delay(1);
}