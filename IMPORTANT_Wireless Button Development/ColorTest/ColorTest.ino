#define led 27
#define led_count 24

#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(led_count, led, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  pixels.begin();
  pixels.setBrightness(80);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < led_count; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
  }
  pixels.show();
}
