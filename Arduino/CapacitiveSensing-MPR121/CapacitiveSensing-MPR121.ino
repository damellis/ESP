#include <Wire.h>
#include "Adafruit_MPR121.h"

Adafruit_MPR121 cap = Adafruit_MPR121();

void setup() {
  Serial.begin(9600);
  
  if (!cap.begin(0x5A)) {
    Serial.println("Error: couldn't connect to MPR121.");
    while (1);
  }
}

void loop() {
  for (int i = 0; i < 12; i++) {
    Serial.print(cap.filteredData(i)); Serial.print("\t");
    //Serial.print(cap.baselineData(i)); Serial.print("\t");
  }
  Serial.println();
  delay(10);
}
