#include <Wire.h>
#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(9600);

  if (!tcs.begin()) {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
}

void loop() {
  uint16_t clear, red, green, blue;

  tcs.setInterrupt(false);      // turn on LED

  delay(60);  // takes 50ms to read 
  
  tcs.getRawData(&red, &green, &blue, &clear);

  tcs.setInterrupt(true);  // turn off LED
  
  Serial.print(red); Serial.print("\t");
  Serial.print(green); Serial.print("\t");
  Serial.print(blue); // Serial.print("\t");
  // Serial.print(clear);
  Serial.println();

//  uint32_t sum = clear;
//  float r, g, b;
//  r = red; r /= sum;
//  g = green; g /= sum;
//  b = blue; b /= sum;
//  r *= 256; g *= 256; b *= 256;
//  Serial.print((int)r ); Serial.print("\t"); Serial.print((int)g);Serial.print("\t");  Serial.println((int)b );
}

