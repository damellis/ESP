/******************************************************************************
ISL29125_interrupts.ino
Example demonstrating use of the ISL29125 RGB sensor library with interrupts.
Jordan McConnell @ SparkFun Electronics
18 Apr 2014
https://github.com/sparkfun/SparkFun_ISL29125_Breakout_Arduino_Library

This example shows how to use the ISL29125 sensor using interrupts. It
demonstrates how to make a more advanced configuration to set this up that goes
beyond the basics needed simply to acquire readings from the sensor. Interrupts
are triggered when sensor readings are above or below set thresholds and this
example also shows how to set those up. It also teaches how to read the sensor
after the interrupt and how to read/clear status flags so another interrupt can
be triggered and so you can interpret the status of the sensor. If you plan on
using this sensor in an interrupt driven project, this is the example for you.

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

// Declare sensor object
SFE_ISL29125 RGB_sensor;

// The number of triggered interrupts
unsigned int i = 0;

// Set up serial communication, initialize the sensor, and set up interrupts
void setup()
{
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize the ISL29125 and verify its presence
  if (RGB_sensor.init())
  {
    Serial.println("Sensor Initialization Successful\n\r");
  }
  
  // Advanced configuration: Interrupts based solely on red light intensity. ~100ms per sensor reading.
  // Config1: CFG1_MODE_R - only read red
  //          CFG1_10KLUX - 10K Lux is full light scale
  // Config2: CFG2_IR_ADJUST_HIGH - common IR filter setting as a starting point, see datasheet if you desire to calibrate it to your exact lighting situation
  // Config3: CFG3_R_INT - trigger interrupts based on sensor readings for red light intensity
  //          CFG3_INT_PRST8 - only trigger interrupt if red sensor reading crosses threshold 8 consecutive times
  // For other configuration options, look at the SFE_ISL29125.h file in the SFE_ISL29125_Library folder
  RGB_sensor.config(CFG1_MODE_R | CFG1_10KLUX, CFG2_IR_ADJUST_HIGH, CFG3_R_INT | CFG3_INT_PRST8);
  
  // Enable interrupt 0 (pin 2 on the Uno) which is connected interrupt (int) pin of the ISL29125
  // When the interrupt pin goes low (falling edge), call the increment function
  attachInterrupt(0, increment, FALLING);
  
  // Set the red upper threshold (set to 0xFFFF by default)
  // An interrupt will trigger when the red sensor value is above this threshold (for 8 consecutive samples)
  RGB_sensor.setUpperThreshold(0x0B00);
  
  // You can also set the red lower threshold if desired (set to 0x0000 by default)
  //RGB_sensor.setLowerThreshold(0x0300);
}

// Continuously check if an interrupt occured
// If so, print out interrupt #, sensor reading for red light, and time since last interrupt to serial monitor
void loop()
{
  static unsigned int lasti = 0; // Stores the number of the last interrupt
  static unsigned long ms = millis(); // Used to calculate the time between interrupts
  uint16_t red_value = 0; // Stores sensor reading for red light intensity
  uint8_t flags = 0; // Stores status flags read from the sensor
  
  // Check if an interrupt has occured, if so, enter the if block
  if (lasti != i)
  {
    // Read the detected light intensity of the red visible spectrum
    red_value = RGB_sensor.readRed();

    // Print out the interrupt # and sensor reading
    Serial.print("Interrupt #: ");
    Serial.println(i);
    Serial.print("Red Sensor Value (HEX): ");
    Serial.println(red_value, HEX);
    // Print out the # of milliseconds since the last interrupt
    Serial.print("Milliseconds since last interrupt: ");
    Serial.println(millis() - ms);
    Serial.println();
    ms = millis(); // Reset ms so we can start counting milliseconds up to the next interrupt
    
    // Set lasti to i, so that this if statement is not entered again until another interrupt is triggered
    lasti = i;
    
    // Read and clear the status flags including the interrupt triggered flag
    // This must be done otherwise another interrupt from the sensor can not be triggered
    flags = RGB_sensor.readStatus();
    
    // If you desire to see the reported status of the chip, uncomment the line below
    //checkSensorStatus(flags);
  }
}

// ISR - When an interrupt is triggered by the sensor, this function is called and 'i' is incremented by one.
void increment()
{
  i++;
}

// Read status of sensor and report findings to the serial monitor
void checkSensorStatus(uint8_t flags)
{
  if (flags & FLAG_INT)
  {
    Serial.println("Interrupt triggered");
  }
  else
  {
    Serial.println("Interrupt cleared or not triggered yet");
  }
  if (flags & FLAG_CONV_DONE)
  {
    Serial.println("Sensor ADC conversion completed");
  }
  else
  {
    Serial.println("Sensor ADC is still converting or cleared");
  }
  if (flags & FLAG_BROWNOUT)
  {
    Serial.println("Power down or brownout occurred");
  }
  else
  {
    Serial.println("No power down or brownout detected");
  }
  if (flags & FLAG_CONV_G)
  {
    Serial.println("Green under conversion");
  }
  else if (flags & FLAG_CONV_R)
  {
    Serial.println("Red under conversion");
  }
  else if (flags & FLAG_CONV_B)
  {
    Serial.println("Blue under conversion");
  }
  else
  {
    Serial.println("No current RGB conversion");
  }
  Serial.println();
}
