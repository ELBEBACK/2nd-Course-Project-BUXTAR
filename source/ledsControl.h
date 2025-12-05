#ifndef LEDS_H
#define LEDS_H

#include <Adafruit_NeoPixel.h>

class LedsControl {

    int LED_PIN;
    int NUM_LEDS;
    int BRIGHTNESS = 50;

public:
    Adafruit_NeoPixel strip;

    LedsControl(int led_pin, int count_led) : LED_PIN(led_pin),
                                              NUM_LEDS(count_led),
                                              strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800)
    {
      //strip.begin();
      //strip.show();
      //strip.setBrightness(BRIGHTNESS);
    };

    void allOff() {
      strip.clear();
      strip.show();
    }

    //ne method
    bool isLEDOn(int ledIndex) {

      uint32_t color = strip.getPixelColor(ledIndex);
  
      uint8_t r = (color >> 16) & 0xFF;
      uint8_t g = (color >> 8) & 0xFF;
      uint8_t b = color & 0xFF;
  
      return (r > 0 || g > 0 || b > 0);
    }

    void turnOffLED(int ledIndex) {
      strip.setPixelColor(ledIndex, 0);
      strip.show();
    }


    void AnimationUno(uint32_t colour) {
      for(int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, colour);
        strip.show();
        delay(50);
      }
    }

    void circleUno(int circnum, uint32_t colour) {
      for (int i = 24 * (circnum - 1); i < 24 * circnum; ++i) {
        strip.setPixelColor(i, colour);
        strip.show();
        delay(5);
      }
    }
    void circleUnoInstant(int circnum, uint32_t colour) {
      for (int i = 24 * (circnum - 1); i < 24 * circnum; ++i) {
        strip.setPixelColor(i, colour);
        strip.show();
      }
    }

    void circleUnoOff(int circnum) {
      for (int i = 24 * (circnum - 1); i < 24 * circnum; ++i) {
        turnOffLED(i);
        delay(5);
      }
    }

    void circleUnoOffInstant(int circnum) {
      for (int i = 24 * (circnum - 1); i < 24 * circnum; ++i) {
        turnOffLED(i);
      }
    }

};
#endif