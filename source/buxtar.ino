#include "pumpControl.h"
#include "tensoControl.h"
#include "displayControl.h"
#include "ledsControl.h"
#include "encoderControl.h"
#include "servoControl.h"
#include <string>
#include "buttonControl.h"


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
//button
#define BUTTON_1 25
#define BUTTON_2 26
#define BUTTON_3 27
#define BUTTON_4 14

/////////FIRST LEVEL//////////
enum Modes {
	auto_mode           = 1,
	russian_roulette    = 2,
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
enum Volume {
	ml50   = 1,
	ml100  = 2,
	ml150  = 3,
	ml200  = 4,
};
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

float first_pump_vol_per_sec = 1;
float second_pump_vol_per_sec = 1;
//////////////////////////////
struct auto_one_set {
    int volume;
    int first_proportion;
};

auto_one_set auto_set[4] = {{50, 0}, {50, 0}, {50, 0}, {50, 0}};
//menu branch status
//1st lvl
bool is_mode_selected             = 0;  //
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

//auto
int which_auto_pos = 0;
bool is_first_vol_selected = 1;
bool is_second_vol_selected = 1;
bool is_third_vol_selected = 1;
bool is_fourth_vol_selected = 1;

bool is_first_prop_selected = 1;
bool is_second_prop_selected = 1;
bool is_third_prop_selected = 1;
bool is_fourth_prop_selected = 1;

bool is_volume_submenu_selected = 1;
bool is_prop_menu_selected = 1;
bool auto_mode_settings = 1;

int is_vol_selected = 1;         //
int is_prop_selected = 1;        //

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

ButtonControl  buttons[4] = {
  ButtonControl(BUTTON_1),
  ButtonControl(BUTTON_2),
  ButtonControl(BUTTON_3),
  ButtonControl(BUTTON_4),
};

void dumpAutoSet(const auto_one_set* arr, size_t n,
                 uint8_t volMask, uint8_t propMask);

void scaleSensitivity(void *param)  {
    while (true) {
      if ((is_vol_selected == 15) && (is_prop_selected == 15)) {
        float cur_weight = 0;
        for (int i = 0; i < 4; ++i) {
          cur_weight = scale[i].current_weight();
          if (abs(cur_weight) > 15.0) {
            leds.circleUnoInstant(4 - i, leds.strip.Color(50, 0, 50));
            if(buttons[i].wasPressed()) {
                servo.turn(servo.angles[i + 1]);

                float v1 = auto_set[i].volume * (auto_set[i].first_proportion / 100.0f);
                float v2 = auto_set[i].volume * ((100.0f - auto_set[i].first_proportion) / 100.0f);
                float tare_weight = scale[i].current_weight();
                uint32_t t0 = millis();

                pump[0].setPumpForward();
                while(v1 > (scale[i].current_weight() - tare_weight)) {
                  if (millis() - t0 > 20000) break;
                  vTaskDelay(10);
                }
                pump[0].stopPump();

                vTaskDelay(100);

                tare_weight = scale[i].current_weight();
                t0 = millis();
                pump[1].setPumpForward();
                while(v2 > (scale[i].current_weight() - tare_weight)) {
                  if (millis() - t0 > 20000) break;
                    vTaskDelay(10);
                }
                pump[1].stopPump();

                servo.turn(0);
            }
          } else {
            leds.circleUnoOffInstant(4 - i);
          }
        }
    }

    // Yield control to FreeRTOS so other tasks can run
    vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay for 100ms
  }
}

int spot = 0, semnum = 0;
void spotLights(void* param) {
    while (true) {
      if (is_scaling_submenu_selected || is_servo_submenu_selected) {
        if (semnum) {
          --semnum;
          LEDTransition(spot);
        }
      }

      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void LEDTransition(int position) {
  if(position != 0) leds.circleUno(position, leds.strip.Color(50, 0, 50));
  for (int i = 4; i > 0; --i) {
    if(i != position) {
      leds.circleUnoOff(i);
    }
  }
}


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

  for(int i = 0; i < 4; i++) {
      scale[i].init();
  }

  if (xTaskCreate(scaleSensitivity, "Weight - LED", 2048, NULL, 1, NULL) != pdPASS) {
    Serial.println("Failed to create RTOS");
  }
  if (xTaskCreate(spotLights, "Animated LED", 2048, NULL, 1, NULL) != pdPASS) {
    Serial.println("Failed to create RTOS");
  }

  display.lcd.clear();
  display.printL2("via Encoder");
  display.printL1("Select mode");

}

void loop() {

  if(!is_mode_selected) {
      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 3);

      if(option != encoder.getLastChoice()) {
	      display.lcd.clear();
          switch(option) {
              case auto_mode:
                display.printL1("Preset mode");
                break;
              //case russian_roulette:
                //display.printL1("Russian Roulette");
                //break;
              case manual_mode - 1:
                display.printL1("Manual control");
                break;
              case captcha - 1:
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
              //case russian_roulette:
                //display.printL1("Russian Roulette");
                //is_russian_roulette_selected = 0;
                //break;
              case manual_mode - 1:
                display.printL1("Select & Control");
                display.printL2("control");
                is_manual_selected = 0;
                break;
              case captcha - 1:
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

  if((is_vol_selected == 15) && (is_prop_selected == 15)) {
      //Serial.println("qwertyui");

      //display.printL1("Positions");
      //display.printL2("are set");
      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 5);
      if(option != encoder.getLastChoice()) {
          display.lcd.clear();
          switch(option) {
              case 1:
                display.printL1("Position 1");
                display.lcd.setCursor(0, 1);
                display.lcd.print(auto_set[0].volume);
                display.lcd.print("  ");
                display.lcd.print("ml");
                display.lcd.print("    ");
                display.lcd.print(auto_set[0].first_proportion);
                display.lcd.print(":");
                display.lcd.print(100 - auto_set[0].first_proportion);
                break;
              case 2:
                display.printL1("Position 2");
                display.lcd.setCursor(0, 1);
                display.lcd.print(auto_set[1].volume);
                display.lcd.print("  ");
                display.lcd.print("ml");
                display.lcd.print("    ");
                display.lcd.print(auto_set[1].first_proportion);
                display.lcd.print(":");
                display.lcd.print(100 - auto_set[1].first_proportion);
                break;
              case 3:
                display.printL1("Position 3");
                display.lcd.setCursor(0, 1);
                display.lcd.print(auto_set[2].volume);
                display.lcd.print("  ");
                display.lcd.print("ml");
                display.lcd.print("    ");
                display.lcd.print(auto_set[2].first_proportion);
                display.lcd.print(":");
                display.lcd.print(100 - auto_set[2].first_proportion);
                break;
              case 4:
                display.printL1("Position 4");
                display.lcd.setCursor(0, 1);
                display.lcd.print(auto_set[3].volume);
                display.lcd.print("  ");
                display.lcd.print("ml");
                display.lcd.print("    ");
                display.lcd.print(auto_set[3].first_proportion);
                display.lcd.print(":");
                display.lcd.print(100 - auto_set[3].first_proportion);
                break;
              case 5:
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
          display.lcd.clear();
          encoder.resetChoice();

          switch(option) {
                case 1:
                  which_auto_pos = 1;
                  is_vol_selected = 0b1110;
                  is_prop_selected = 0b1110;
                  is_volume_submenu_selected = 0;
                  break;
                case 2:
                  which_auto_pos = 2;
                  is_vol_selected = 0b1101;
                  is_prop_selected = 0b1101;
                  is_volume_submenu_selected = 0;
                  break;
                case 3:
                  which_auto_pos = 3;
                  is_vol_selected = 0b1011;
                  is_prop_selected = 0b1011;
                  is_volume_submenu_selected = 0;
                  break;
                case 4:
                  which_auto_pos = 4;
                  is_vol_selected = 0b0111;
                  is_prop_selected = 0b0111;
                  is_volume_submenu_selected = 0;
                  break;
                case 5:
                  display.printL1("Back to");
                  display.printL2("main menu");
                  is_mode_selected = 0;
                  is_vol_selected = 0;
                  is_prop_selected = 0;
                   break;
                default: Serial.println("mode error");
                        Serial.println(option);
                        break;
          }
        }
  } else if(!is_automode_selected) {
      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 3);

      if(option != encoder.getLastChoice()) {
          display.lcd.clear();
          switch(option) {
              case 1:
                display.printL1("Start with");
                display.printL2("standart set");
                break;
              case 2:
                display.printL1("Choose your");
                display.printL2("preset");
                break;
              case 3: 
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
            display.lcd.clear();
            encoder.resetChoice();
            is_automode_selected = 1;
            switch(option) {
              case 1:
                display.printL1("Default presets");
                display.printL2("");
                is_vol_selected = 15;
                is_prop_selected = 15;
                break;
              case 2:
		            display.printL1("Position 1");
                auto_mode_settings = 0;
                break;
              case 3:
                display.printL1("Back to");
                display.printL2("main menu");
                is_mode_selected = 0;
                break;
                  default: Serial.println("mode error");
                          Serial.println(option);
                          break;
                }
          }
  } else if(!auto_mode_settings) {

      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 5);

      if(option != encoder.getLastChoice()) {
          display.lcd.clear();
          switch(option) {
              case autoPos1:
                display.printL1("Position 1");
                break;
              case autoPos2:
                display.printL1("Position 2");
                break;
              case autoPos3:
                display.printL1("Position 3");
                break;
              case autoPos4:
                display.printL1("Position 4");
                break;
              case quitAuto:
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
            display.lcd.clear();
            encoder.resetChoice();
            switch(option) {
              case autoPos1:
                display.printL1("Position 1");
                display.printL2("Select volume");
                which_auto_pos = 1;
                is_automode_selected = 1;
                is_volume_submenu_selected = 0;
                break;
              case autoPos2:
                display.printL1("Position 2");
                display.printL2("Select volume");
                which_auto_pos = 2;
                is_automode_selected = 1;
                is_volume_submenu_selected = 0;
                break;
              case autoPos3:
                display.printL1("Position 3");
                display.printL2("Select volume");
                which_auto_pos = 3;
                is_automode_selected = 1;
                is_volume_submenu_selected = 0;
                break;
              case autoPos4 :
                display.printL1("Position 4");
                display.printL2("Select volume");
                which_auto_pos = 4;
                is_automode_selected = 1;
                is_volume_submenu_selected = 0;
                break;
              case 5:
                display.printL1("Start with");
                display.printL2("standart set");
                auto_mode_settings = 1;
                is_automode_selected = 0;
                break;
                  default: Serial.println("mode error");
                          Serial.println(option);
                          break;
                }
          }
  } else if(!is_volume_submenu_selected) {
      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 5);
      if(option != encoder.getLastChoice()) {
          display.lcd.clear();
          switch(option) {
              case ml50:
                display.printL1("Select volume");
                display.printL2("       50ml");
                break;
              case ml100:
                display.printL1("Select volume");
                display.printL2("       100ml");
                break;
              case ml150:
                display.printL1("Select volume");
                display.printL2("       150ml");
                break;
              case ml200:
                display.printL1("Select volume");
                display.printL2("       200ml");
                break;
              case 5:
                display.printL1("Back to choose");
                display.printL2("position");
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
          display.lcd.clear();
          encoder.resetChoice();

          switch(option) {
                case ml50:
                  display.printL1("Volume selected");
                  auto_set[which_auto_pos - 1].volume = 50;
                  is_vol_selected  |= (1u << (which_auto_pos - 1));
                  is_volume_submenu_selected = 1;
                  is_prop_menu_selected = 0;
                  break;
                case ml100:
                  display.printL1("Volume selected");
                  auto_set[which_auto_pos - 1].volume = 100;
                  is_vol_selected  |= (1u << (which_auto_pos - 1));
                  is_volume_submenu_selected = 1;
                  is_prop_menu_selected = 0;
                  break;
                case ml150:
                  display.printL1("Volume selected");
                  auto_set[which_auto_pos - 1].volume = 150;
                  is_vol_selected  |= (1u << (which_auto_pos - 1));
                  is_volume_submenu_selected = 1;
                  is_prop_menu_selected = 0;
                  break;
                case ml200:
                  display.printL1("Volume selected");
                  auto_set[which_auto_pos - 1].volume = 200;
                  is_vol_selected  |= (1u << (which_auto_pos - 1));
                  is_volume_submenu_selected = 1;
                  is_prop_menu_selected = 0;
                  break;
                case 5:
                  display.printL1("Select position");
                  is_volume_submenu_selected = 1;
                  is_automode_selected = 0;
                  break;
                default: Serial.println("mode error");
                        Serial.println(option);
                        break;
          }
        }
  } else if(!is_prop_menu_selected) {
      currentStateCLK = digitalRead(CLK_PIN);
      option = encoder.SetChoice(currentStateCLK, 7);
      if(option != encoder.getLastChoice()) {
          display.lcd.clear();
          switch(option) {
              case 1:
                display.printL1("Select ratio");
                display.printL2("       0% | 100%");
                break;
              case 2:
                display.printL1("Select ratio");
                display.printL2("       20% | 80%");
                break;
              case 3:
                display.printL1("Select ratio");
                display.printL2("       40% | 60%");
                break;
              case 4:
                display.printL1("Select ratio");
                display.printL2("       60% | 40%");
                break;
              case 5:
                display.printL1("Select ratio");
                display.printL2("       80% | 100%");
                break;
              case 6:
                display.printL1("Select ratio");
                display.printL2("       100% | 0%");
                break;
              case 7:
                display.printL1("Back to select");
                display.printL2("volume");
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
          display.lcd.clear();
          encoder.resetChoice();

          switch(option) {
                case 1:
                  display.printL1("Proportion");
                  display.printL2("selected");
                  auto_set[which_auto_pos - 1].first_proportion = 0;
                  is_prop_selected |= (1u << (which_auto_pos - 1));
                  is_prop_menu_selected = 1;
                  is_automode_selected = 0;
                  break;
                case 2:
                  display.printL1("Proportion");
                  display.printL2("selected");
                  auto_set[which_auto_pos - 1].first_proportion = 20;
                  is_prop_selected |= (1u << (which_auto_pos - 1));
                  is_prop_menu_selected = 1;
                   is_automode_selected = 0;
                  break;
                case 3:
                  display.printL1("Proportion");
                  display.printL2("selected");
                  auto_set[which_auto_pos - 1].first_proportion = 40;
                  is_prop_selected |= (1u << (which_auto_pos - 1));
                  is_prop_menu_selected = 1;
                  is_automode_selected = 0;
                  break;
                case 4:
                  display.printL1("Proportion");
                  display.printL2("selected");
                  auto_set[which_auto_pos - 1].first_proportion = 60;
                  is_prop_selected |= (1u << (which_auto_pos - 1));
                  is_prop_menu_selected = 1;
                  is_automode_selected = 0;
                  break;
                case 5:
                  display.printL1("Proportion");
                  display.printL2("selected");
                  auto_set[which_auto_pos - 1].first_proportion = 80;
                   is_prop_selected |= (1u << (which_auto_pos - 1));
                   is_prop_menu_selected = 1;
                   is_automode_selected = 0;
                   break;
                case 6:
                  display.printL1("Proportion");
                  display.printL2("selected");
                  auto_set[which_auto_pos - 1].first_proportion = 100;
                  is_prop_selected |= (1u << (which_auto_pos - 1));
                  is_prop_menu_selected = 1;
                  is_automode_selected = 0;
                  break;
                case 7:
                  display.printL1("Select volume");
                  is_volume_submenu_selected = 0;
                  is_prop_menu_selected = 1;
                  break;
                default: Serial.println("mode error");
                        Serial.println(option);
                        break;
          }
        }
  }



  if(!is_captcha_selected) {
      for(int i = 1; i < 5; i++) {
          leds.circleUno(i, leds.strip.Color(50, 0, 50));
      }

      delay(1000);

      for(int i = 1; i < 5; i++) {
          leds.circleUnoOff(i);
      }

      bool are_you_pass_the_test = true;

      for(int i = 0; i < 7; i++) {
          int random_number = random(1, 5);

          leds.circleUnoInstant(5 - random_number, leds.strip.Color(50, 0, 50));

          unsigned long startTime = millis();
          bool hit = false;

          while(millis() - startTime < 1500) {
            if(buttons[random_number - 1].getButtonState()) {

                Serial.print("Button ");
                Serial.print(random_number);
                Serial.println(" pressed!");

                hit = true;
                break;
            }

            for(int b = 0; b < 4; b++){
              if(b != (random_number-1) && buttons[b].getButtonState()){
                hit = false;
                are_you_pass_the_test = false;
                break;
              }
            }
            if(!are_you_pass_the_test) break;

            delay(10);
          }
          are_you_pass_the_test = 1;

          if(!are_you_pass_the_test) {break;}

          leds.circleUnoOffInstant(5 - random_number);

          if (!hit) {
            are_you_pass_the_test = false;
            break;
          }
          int rand_delay = random(200, 1000);

          delay(rand_delay);
      }

      for(int i = 1; i < 5; i++) {
          leds.circleUnoOffInstant(i);
      }


      if(!are_you_pass_the_test) {
          for(int i = 1; i < 5; i++) {
            leds.circleUnoInstant(i, leds.strip.Color(50, 0, 0));
          }
          display.lcd.clear();
          display.printL1("its time to");
          display.printL2("polysorb");
          delay(5000);
      } else {
          for(int i = 1; i < 5; i++) {
            leds.circleUnoInstant(i, leds.strip.Color(0, 50, 0));
          }
          display.lcd.clear();
          display.printL1("you passed");
          display.printL2("the test");
          delay(5000);
      }
      for(int i = 1; i < 5; i++) {
          leds.circleUnoOffInstant(i);
      }


      is_captcha_selected = 1;
      is_mode_selected = 0;
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
                spot = 4;
                ++semnum;
                break;
              case secondS:
                display.printL1("Scale 2");
                spot = 3;
                ++semnum;
                break;
              case thirdS:
                display.printL1("Scale 3");
                spot = 2;
                ++semnum;
                break;
              case fourthS:
                display.printL1("Scale 4");
                spot = 1;
                ++semnum;
                break;
              case quitCOS:
                display.printL1("Back");
                spot = 0;
                ++semnum;
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
                display.lcd.print(scale[which_scale - 1].current_weight());
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
                spot = 4;
                ++semnum;
                break;
              case secondPos:
                display.printL1("Turn until ");
                display.printL2("second position");
                spot = 3;
                ++semnum;
                break;
              case thirdPos:
                display.printL1("Turn until");
                display.printL2("third position");
                spot = 2;
                ++semnum;
                break;
              case fourthPos:
                display.printL1("Turn until");
                display.printL2("fourth position");
                spot = 1;
                ++semnum;
                break;
              case home:
                display.printL1("Get servo back");
                spot = 0;
                ++semnum;
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

void dumpAutoSet(const auto_one_set* arr, size_t n,
                 uint8_t volMask, uint8_t propMask) {
  Serial.println(F("---- AUTO SETTINGS DUMP ----"));
  Serial.print(F("volMask = 0b"));
  for (int i = 7; i >= 0; --i) Serial.print((volMask >> i) & 1);
  Serial.print(F(" (")); Serial.print(volMask); Serial.println(F(")"));

  Serial.print(F("propMask= 0b"));
  for (int i = 7; i >= 0; --i) Serial.print((propMask >> i) & 1);
  Serial.print(F(" (")); Serial.print(propMask); Serial.println(F(")"));

  for (size_t i = 0; i < n; ++i) {
    Serial.print(F("Pos "));
    Serial.print(i + 1);
    Serial.print(F(": volume="));
    Serial.print(arr[i].volume);
    Serial.print(F(" ml, first_proportion="));
    Serial.print(arr[i].first_proportion);
    Serial.println(F("%"));
  }
  Serial.println(F("----------------------------"));
}
