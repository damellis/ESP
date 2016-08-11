// Arduino example that streams accelerometer data from an ADXL335
// (or other three-axis analog accelerometer) to the ESP system and
// lights different LEDs depending on the predictions made by the
// ESP system. Use with the user_accelerometer_gestures.cpp ESP example.

// the accelerometer pins
int zpin = A2;
int ypin = A1;
int xpin = A0;

// the LED pins
int redpin = 9;
int greenpin = 10;
int bluepin = 11;

// These are only used if you're plugging the ADXL335 (on the
// Adafruit breakout board) directly into the analog input pins
// of your Arduino. See comment below.
int vinpin = A5;
int voutpin = A4;
int gndpin = A3;

void setup() {
  Serial.begin(115200);

  // Lower the serial timeout (from its default value of 1000 ms)
  // so that the call to Serial.parseInt() below doesn't pause for
  // too long and disrupt the sending of accelerometer data.
  Serial.setTimeout(2);

  // Uncomment the following lines if you're using an ADXL335 on an
  // Adafruit breakout board (https://www.adafruit.com/products/163)
  // and want to plug it directly into (and power it from) the analog
  // input pins of your Arduino board.
//  pinMode(vinpin, OUTPUT); digitalWrite(vinpin, HIGH);
//  pinMode(gndpin, OUTPUT); digitalWrite(gndpin, LOW);
//  pinMode(voutpin, INPUT);
  
  pinMode(xpin, INPUT);
  pinMode(ypin, INPUT);
  pinMode(zpin, INPUT);
}

void loop() {
  Serial.print(analogRead(xpin)); Serial.print("\t");
  Serial.print(analogRead(ypin)); Serial.print("\t");
  Serial.print(analogRead(zpin)); Serial.println();
  delay(10);

  // Check for a valid prediction.
  int val = Serial.parseInt();
  if (val != 0) {
    // Turn off all the LEDs.
    analogWrite(redpin, 0);
    analogWrite(greenpin, 0);
    analogWrite(bluepin, 0);

    // Turn on the LED corresponding to the prediction.
    if (val == 1) analogWrite(redpin, 255);
    if (val == 2) analogWrite(greenpin, 255);
    if (val == 3) analogWrite(bluepin, 255);
  }
}
