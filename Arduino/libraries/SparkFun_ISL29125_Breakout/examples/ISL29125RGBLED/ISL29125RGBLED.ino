/******************************************************************************
ISL29125_RGB_LED.ino
Example for using the ISL29125 RGB sensor library along with an RGB LED.
Jordan McConnell @ SparkFun Electronics
11 Apr 2014
https://github.com/sparkfun/SparkFun_ISL29125_Breakout_Arduino_Library

This example declares an SFE_ISL29125 object called RGB_sensor. The 
object/sensor is initialized with a basic configuration so that it continuously
samples the light intensity of red, green and blue spectrums. An RGB LED is set
up so that it can be placed above the sensor to change the sensor's readings.
The Red component of the LED is turned on and then sensor readings are printed
to the serial monitor. If this is done in a dark environment, you should notice
the readings for Red will be significantly higher than for green or blue. The
sketch then waits two seconds and does this process again for green, and then 
again for blue. Since the RGB LED is on PWM pins, you can change the brightness
and see how this affects the readings. Again, best results in a dark environment.

Developed/Tested with:
Arduino Uno
Arduino IDE 1.0.5

Requires:
SparkFun_ISL29125_Arduino_Library

This code is beerware.
Distributed as-is; no warranty is given. 
******************************************************************************/

#include <Wire.h>
#include "SparkFunISL29125.h"

// Declare pins for RGB LED, all PWM pins
uint8_t rPin = 3;
uint8_t gPin = 5;
uint8_t bPin = 6;

// Declare sensor object
SFE_ISL29125 RGB_sensor;

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);
  
  // Set up pins for RGB LED
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  
  // Initialize the ISL29125 with simple configuration so it starts sampling
  if (RGB_sensor.init())
  {
    Serial.println("Sensor Initialization Successful\n\r");
  }
}

// Turns on one color at a time of an RGB LED then prints sensor readings of the sensor to the serial monitor
void loop()
{
  // Turn only Red LED on
  analogWrite(rPin, 255);
  analogWrite(gPin, 0);
  analogWrite(bPin, 0);
  delay(300);
  // Take sensor readings for each color and print them to serial monitor
  Serial.println("Red LED ON");
  Serial.print("Red: "); Serial.println(RGB_sensor.readRed(),HEX);
  Serial.print("Green: "); Serial.println(RGB_sensor.readGreen(),HEX);
  Serial.print("Blue: "); Serial.println(RGB_sensor.readBlue(),HEX);
  Serial.println();
  delay(2000);
  
  // Turn only Green LED on
  analogWrite(rPin, 0);
  analogWrite(gPin, 255);
  analogWrite(bPin, 0);
  delay(300);
  // Take sensor readings for each color and print them to serial monitor
  Serial.println("Green LED ON");
  Serial.print("Red: "); Serial.println(RGB_sensor.readRed(),HEX);
  Serial.print("Green: "); Serial.println(RGB_sensor.readGreen(),HEX);
  Serial.print("Blue: "); Serial.println(RGB_sensor.readBlue(),HEX);
  Serial.println();
  delay(2000);
  
  // Turn only Blue LED on
  analogWrite(rPin, 0);
  analogWrite(gPin, 0);
  analogWrite(bPin, 255);
  delay(300);
  // Take sensor readings for each color and print them to serial monitor
  Serial.println("Blue LED ON");
  Serial.print("Red: "); Serial.println(RGB_sensor.readRed(),HEX);
  Serial.print("Green: "); Serial.println(RGB_sensor.readGreen(),HEX);
  Serial.print("Blue: "); Serial.println(RGB_sensor.readBlue(),HEX);
  Serial.println();
  delay(2000);
}
