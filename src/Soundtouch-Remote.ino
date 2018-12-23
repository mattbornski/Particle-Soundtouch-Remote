/*
 * Project Soundtouch-Remote
 * Description:
 * Author:
 * Date:
 */

#include "Particle.h"
#include "neopixel.h"
#include <Encoder.h>

#define PIXEL_PIN A5
#define PIXEL_COUNT 24
#define PIXEL_TYPE WS2812B

#define BUTTON_PIN A4


Adafruit_NeoPixel ring(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
Encoder volumeKnob(D4, D3);
static volatile uint8_t volumePercentage = 0;
static volatile bool recentlyPressed = false;

void setup() {
    ring.begin();
    // This thing is bright.  255 = max, 0 = off
    ring.setBrightness(16);
    ring.show();

    volumeKnob.write(0);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(BUTTON_PIN, press, FALLING);
}

void loop() {
    if (recentlyPressed) {
        Serial.println("Button recently pressed");
        recentlyPressed = false;
    }

    setArc(volumePercentage, 0xFFFFFFFF);

    static volatile long oldPosition = -999;
    long newPosition = volumeKnob.read();
    if (newPosition != oldPosition) {
        oldPosition = newPosition;
        Serial.println(newPosition);
        volumePercentage = newPosition;
    }
}

void setArc(uint8_t percentage, uint32_t color) {
    uint8_t illuminatedPixels = (uint8_t) (ring.numPixels() * ((float)percentage / 100.0f));
    Serial.print("illuminate ");
    Serial.print(illuminatedPixels);
    Serial.println(" / 24 pixels");
    for (uint8_t i = 0; i < ring.numPixels(); i++) {
        if (i < illuminatedPixels) {
            ring.setPixelColor(i, color);
        } else {
            ring.setPixelColor(i, (uint32_t)0);
        }
    }
    ring.show();
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<ring.numPixels(); i++) {
      ring.setPixelColor(i, Wheel((i+j) & 255));
    }
    ring.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return ring.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return ring.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return ring.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void press() {
    static volatile unsigned long lastInterruptTime = 0;
    unsigned long interrupt_time = millis();
    // debounce time = 50milliseconds
    if (interrupt_time - lastInterruptTime > 50) {
        recentlyPressed = true;
    }
    lastInterruptTime = interrupt_time;
}