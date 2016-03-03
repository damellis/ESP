#include "CurieImu.h"

int16_t ax, ay, az;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  CurieImu.initialize();

  if (!CurieImu.testConnection()) {
    Serial.println("CurieImu connection failed");
  }

  CurieImu.setFullScaleAccelRange(BMI160_ACCEL_RANGE_8G);
}

void loop() {
  CurieImu.getAcceleration(&ax, &ay, &az);
  Serial.print(ax);
  Serial.print("\t");
  Serial.print(ay);
  Serial.print("\t");
  Serial.print(az);
  Serial.println();
}
