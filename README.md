# ESP (Example-based Sensor Predictions)

This project aims to help novices make sophisticated use of sensors in
interactive projects through the application of machine learning.

## Pre-requisites

At the moment, this project runs only on OS X. You'll need Xcode and
git (to clone this repository and its submodules).

## Installation

To install, first clone this repository, then run the setup script:

```
git clone https://github.com/damellis/sensors.git
cd sensors
./setup
```

This will clone the relevant git submodules and create some symbolic links.

## Running

The main application is the SmartSensors Xcode project. To select an example
to run, uncomment the corresponding line in user.h. Many of these examples
expect an Arduino board to be connected to the computer and running an
appropriate sketch. Example include:

- user_audio_beat.h: recognizes periodic sounds (e.g. dialtones, bells ringing,
  whistling) using an FFT and support vector machines algorithm. Works with
  your computer's built-in microphone.

- user_color_sensor.h: detects objects by color using a naive Bayes classifier.
  Works with either the [Adafruit TCS34725 breakout](https://www.adafruit.com/products/1334)
  (using the sketch in Arduino/ColorSensor) or the [SparkFun ISL29125 breakout](https://www.sparkfun.com/products/12829)
  (using the sketch in Arduino/ColorSensor_SparkFun_ISL29125). See
  documentation for the sensors for hookup information. 
  
- user_accelerometer_calibration.h: recognizes gestures using a dynamic time
  warping algorith. Works with either an [ADXL335 accelerometer](https://www.adafruit.com/products/163)
  (using the Arduino/ADXL335 sketch) or the built-in accelerometer on an
  [Arduino 101](http://www.arduino.cc/en/Main/ArduinoBoard101) (using the
  Arduino/Arduino101_Accelerometer sketch).
  
- user_accelerometer_poses.h: recognizes the orientations of an object using
  a naive Bayes classifier. Works with accelerometers as for the
  user_accelerometer_calibration.h example.
  
## API

See the [online documentation of the ESP API](http://damellis.github.io/ESP/).

## Dependencies

These should be automatically installed by the setup script:

- [openFrameworks](http://openframeworks.cc/), a C++ toolkit for creative
  coding.

- [GRT](http://www.nickgillian.com/software/grt), Gesture Recognition Toolkit,
  a cross-platform, open-source, C++ machine learning library that has been
  specifically designed for real-time gesture recognition. Specifically
  [our fork of the GRT repository](https://github.com/damellis/grt).

- [ofxGrt](https://github.com/nickgillian/ofxGrt), an openFrameworks extension
  for the Gesture Recognition Toolkit (GRT). Specifically
  [our fork of the ofxGrt repository](https://github.com/nebgnahz/ofxGrt/tree/snapshot-for-sensors).

## License

See [LICENSE.txt](LICENSE.txt) for licensing information.
