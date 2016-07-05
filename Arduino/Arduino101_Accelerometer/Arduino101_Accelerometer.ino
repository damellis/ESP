#include "CurieIMU.h"

int ax, ay, az;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  CurieIMU.begin();

  if (!CurieIMU.testConnection()) {
    Serial.println("CurieImu connection failed");
  }

  CurieIMU.setAccelerometerRange(8);
}

void loop() {
  CurieIMU.readAccelerometer(ax, ay, az);
  Serial.print(ax);
  Serial.print("\t");
  Serial.print(ay);
  Serial.print("\t");
  Serial.print(az);
  Serial.println();
  delay(10);
}
