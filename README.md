# ESP (Example-based Sensor Predictions)

[![Build Status](https://travis-ci.org/damellis/ESP.svg?branch=master)](https://travis-ci.org/damellis/ESP)

This project aims to help novices make sophisticated use of sensors in
interactive projects through the application of machine learning.

## Pre-requisites

At the moment, this project runs on OS X and Linux. You'll need Xcode
and git (to clone this repository and its submodules). You can also build
the project for Windows, but not all examples are supported yet.

## Installation

To install, first clone this repository, then run the setup script:

```
git clone https://github.com/damellis/ESP.git
cd ESP
./setup.sh
```

This will clone the relevant git submodules and create some symbolic links.

## Running

The main application is an openFrameworks-based GUI application named,
unsurprisingly, ESP. Below are instructions to run on different platform
(currently we support Linux and OS X).

Arduino Project Hub has a more comprehensive [tutorial](https://create.arduino.cc/projecthub/mellis/gesture-recognition-using-accelerometer-and-esp-mac-only-71faa1) on how to use the software.

### OS X

Use Xcode to open the project at `Xcode/ESP/ESP.xcodeproj`. You can choose an
example by uncommenting the corresponding line at
[user.cpp](https://github.com/damellis/ESP/blob/master/Xcode/ESP/src/user.cpp). Many
of these examples expect an Arduino board to be connected to the computer and
running an appropriate sketch. Some of the sketches are hosted in this
repository as well (see `Arduino` folder). Some examples are:

- [user_audio_beat.cpp](http://damellis.github.io/ESP/user\_audio\_beat\_8cpp-example.html):
  recognizes periodic sounds (e.g. dialtones, bells ringing, whistling) using an
  FFT and support vector machines algorithm. Works with your computer's built-in
  microphone.

- [user_color_sensor.cpp](http://damellis.github.io/ESP/user\_color\_sensor\_8cpp-example.html):
  detects objects by color using a naive Bayes classifier.  Works with either
  the [Adafruit TCS34725 breakout](https://www.adafruit.com/products/1334)
  (using the sketch in Arduino/ColorSensor) or the
  [SparkFun ISL29125 breakout](https://www.sparkfun.com/products/12829) (using
  the sketch in Arduino/ColorSensor_SparkFun_ISL29125). See documentation for
  the sensors for hookup information.

- [user_accelerometer_gesture.cpp](http://damellis.github.io/ESP/user\_accelerometer\_gestures\_8cpp-example.html):
  recognizes gestures using a dynamic time warping algorith. Works with either
  an [ADXL335 accelerometer](https://www.adafruit.com/products/163) (using the
  Arduino/ADXL335 sketch) or the built-in accelerometer on an
  [Arduino 101](http://www.arduino.cc/en/Main/ArduinoBoard101) (using the
  Arduino/Arduino101_Accelerometer sketch).

- [user_accelerometer_poses.cpp](http://damellis.github.io/ESP/user\_accelerometer\_poses\_8cpp-example.html):
  recognizes the orientations of an object using a naive Bayes classifier. Works
  with accelerometers as for the `user_accelerometer_gesture.cpp` example.

We also support using `CMake` on OS X to compile the project:
```sh
# Compile openFramework by compiling an emptyExample
xcodebuild -configuration Release -target emptyExample \
  -project "third-party/openFrameworks/scripts/templates/osx/emptyExample.xcodeproj"

# Build ESP
mkdir build
cd build
cmake ..
make -j8
```

### Linux

We use `CMake` on Linux to compile the project. The compilation is a bit more
involved, but should be easy to follow:

```sh
# Install required package
sudo apt-get -y install doxygen
sudo apt-get -y install cmake

# Then build openFrameworks
sudo third-party/openFrameworks/scripts/ci/linux/install.sh
sudo third-party/openFrameworks/scripts/ci/linux/build.sh

# Build and install GRT
cd third-party/grt/build
mkdir -p tmp && cd tmp
cmake .. -DBUILD_EXAMPLES=OFF
make
sudo make install
cd ../../../../

# Build ESP
mkdir build
cd build
cmake ..
make -j4
```

### Windows

See this [setup guide](https://github.com/damellis/ESP/wiki/Windows-Setup-Guide)

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
