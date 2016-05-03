/**
 @file istream.h
 @brief Definition of input streams: serial, audio, etc.
 */
#pragma once

#include "GRT/GRT.h"
#include "ofMain.h"

#include <cstdint>

// See more documentation:
// http://openframeworks.cc/documentation/sound/ofSoundStream/#show_setup
const uint32_t kOfSoundStream_SamplingRate = 44100;
const uint32_t kOfSoundStream_BufferSize = 256;
const uint32_t kOfSoundStream_nBuffers = 4;

/**
 @brief Base class for input streams that provide live sensor data to the ESP
 system.

 To use an IStream instance in your application, pass it to useStream() in your
 setup() function.
 */
class IStream {
  public:
    IStream();
    virtual ~IStream() = default;

    /**
     Start the input stream.
     */
    virtual bool start() = 0;
    virtual void stop() = 0;

    void toggle() {
        if (has_started_) { stop(); }
        else { start(); }
    }

    /**
     Get the number of dimensions of the data that's provided by the
     input stream (before it's run through the normalizer, if any).
     */
    virtual int getNumInputDimensions() = 0;
    virtual int getNumOutputDimensions() {
        vector<double> input(getNumInputDimensions(), 1.0);
        vector<double> output = normalize(input);
        return output.size();
    }

    typedef std::function<double(double)> normalizeFunc;
    typedef std::function<vector<double>(vector<double>)> vectorNormalizeFunc;

    // Supply a normalization function: double -> double.
    // Applied to each dimension of each vector of incoming data.
    void useNormalizer(normalizeFunc f) {
        normalizer_ = f;
        vectorNormalizer_ = nullptr;
    }

    // Supply a normalization function: vector<double> -> vector<double>
    // Applied to each vector of incoming data.
    void useNormalizer(vectorNormalizeFunc f) {
        normalizer_ = nullptr;
        vectorNormalizer_ = f;
    }

    bool hasStarted() { return has_started_; }

    typedef std::function<void(GRT::MatrixDouble)> onDataReadyCallback;

    void onDataReadyEvent(onDataReadyCallback callback) {
        data_ready_callback_ = callback;
    }

    template<typename T1, typename arg, class T>
    void onDataReadyEvent(T1* owner, void (T::*listenerMethod)(arg)) {
        using namespace std::placeholders;
        data_ready_callback_ = std::bind(listenerMethod, owner, _1);
    }

    // Set labels on all input dimension. This function takes either a vector of
    // strings, or an initialization list (such as {"left", "right"}).
    void setLabelsForAllDimensions(const vector<string> labels);
    void setLabelsForAllDimensions(std::initializer_list<string> list);

    const vector<string>& getLabels() const;

  protected:
    vector<string> istream_labels_;
    bool has_started_;
    onDataReadyCallback data_ready_callback_;
    normalizeFunc normalizer_;
    vectorNormalizeFunc vectorNormalizer_;

    vector<double> normalize(vector<double>);
};

/**
 @brief Input stream for reading audio from the computer's microphone.
 */
class AudioStream : public ofBaseApp, public IStream {
  public:
    AudioStream(uint32_t downsample_rate = 1);
    void audioIn(float *input, int buffer_size, int nChannel);
    virtual bool start() final;
    virtual void stop() final;
    virtual int getNumInputDimensions() final;
  private:
    uint32_t downsample_rate_;
    unique_ptr<ofSoundStream> sound_stream_;
    bool setup_successful_;
};

class SerialStream : public IStream {
  public:
    SerialStream(uint32_t usb_port_num, uint32_t baud);
    virtual bool start() final;
    virtual void stop() final;
    virtual int getNumInputDimensions() final;
  private:
    uint32_t port_ = -1;
    uint32_t baud_;
    // Serial buffer size
    uint32_t kBufferSize_ = 64;

    unique_ptr<ofSerial> serial_;

    // A separate reading thread to read data from Serial.
    unique_ptr<std::thread> reading_thread_;
    void readSerial();
};

/**
 @brief Input stream for reading ASCII data from a (USB) serial port.

 Data should be formatted as ASCII text, in newline-terminated lines. Each line
 consists of whitespace-separated numbers, e.g.
 @verbatim 123 45 678 90 @endverbatim
 The numbers in each line of text are turned into a single data sample.

 To use an ASCIISerialStream in your application, pass it to useStream() in
 your setup() function.
 */
class ASCIISerialStream : public IStream {
  public:
    /**
     Create an ASCIISerialStream instance.

     @param port: the index of the (USB) serial port to use.
     @param baud: the baud rate at which to communicate with the serial port
     @param numDimensions: the number of dimensions in the data that will come
     from the serial port (i.e. the number of numbers in each line of data).
     */
    ASCIISerialStream(uint32_t port, uint32_t baud, uint32_t numDimensions);

    /**
     Create an ASCIISerialStream instance.

     This constructor doesn't require USB port so users will be asked to select
     them at runtime.

     @param baud: the baud rate at which to communicate with the serial port
     @param numDimensions: the number of dimensions in the data that will come
     from the serial port (i.e. the number of numbers in each line of data).
     */
    ASCIISerialStream(uint32_t baud, uint32_t numDimensions);

    virtual bool start() final;
    virtual void stop() final;
    virtual int getNumInputDimensions() final;

    vector<string> getSerialDeviceList() {
        serial_->listDevices();
        vector<string> retval;
        vector<ofSerialDeviceInfo> device_list = serial_->getDeviceList();
        retval.reserve(device_list.size());
        for (auto& d : device_list) {
            retval.push_back(d.getDevicePath());
        }
        return retval;
    }

    bool selectSerialDevice(uint32_t port) {
        assert(has_started_ == false
               && "Should only reach here if ASCIISerialStream hasn't started");

        port_ = port;
        if (!serial_->setup(port_, baud_)) {
            return false;
        }

        reading_thread_.reset(new std::thread(&ASCIISerialStream::readSerial, this));
        has_started_ = true;
        return true;
    }

  private:
    unique_ptr<ofSerial> serial_;
    uint32_t port_;
    uint32_t baud_;
    uint32_t numDimensions_;

    // A separate reading thread to read data from Serial.
    unique_ptr<std::thread> reading_thread_;
    void readSerial();
};

/**
 @brief Input stream for reading analog data from an Arduino running Firmata.

 To use an FirmataStream in your application, pass it to useStream() in your
 setup() function.
 */
class FirmataStream : public IStream {
  public:
    /**
     Create a FirmataStream instance. Assumes the Arduino is communicating at
     57600 baud.

     @param port: the index of the (USB) serial port to use.
     */
    FirmataStream(uint32_t port);
    virtual bool start() final;
    virtual void stop() final;
    virtual int getNumInputDimensions() final;

    /**
     Include readings from the specified analog pin in the data reported by
     this FirmataStream. Data will be ordered according to the sequence of
     calls to this function (i.e. readings from the pin passed to the first
     call to useAnalogPin() will appear first in the data provided by the
     FirmataStream).

     @param i: an analog pin to read from
     */
    void useAnalogPin(int i);
  private:
    uint32_t port_;

    vector<int> pins_;

    bool configured_arduino_;

    ofArduino arduino_;
    unique_ptr<std::thread> update_thread_;
    void update();
};

/**
 Tells the ESP system which input stream to use. Call from your setup()
 function. The specified stream will be automatically started by the ESP
 system. Note that only one input stream is supported; subsequent calls to
 useStream() will replace the previously-specified stream.

 @param stream: the input stream to use
 */
void useStream(IStream &stream);

/**
 Tells the ESP system which machine learning pipeline to use. Call from your
 setup() function. Note that only one pipeline is supported; subsequent calls
 to usePipeline() will replace the previously-specified pipeline.

 The pipeline will be fed with data from the input stream specified using
 useStream().
 */
void usePipeline(GRT::GestureRecognitionPipeline &pipeline);
