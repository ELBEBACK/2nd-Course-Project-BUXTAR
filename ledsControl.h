#ifndef LEDS_H
#define LEDS_H

#include <Adafruit_NeoPixel.h>

class LedsControl {

    int LED_PIN;

public:
    Adafruit_NeoPixel pixels;

    LedsControl(int led_pin, int count_led) : LED_PIN(led_pin) {
      pixels = Adafruit_NeoPixel(count_led, led_pin, NEO_GRB + NEO_KHZ800);
    };

};
#endif