/*
 * IStream encapsulates a number of stream input devices: serial,
 * audio, etc.
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

class IStream {
  public:
    IStream();
    virtual ~IStream() = default;

    virtual void start() = 0;
    virtual void stop() = 0;

    // These two functions are no-op by default.
    virtual void useUSBPort(int i) {};
    virtual void useAnalogPin(int i) {};

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

  protected:
    bool has_started_;
    onDataReadyCallback data_ready_callback_;
    normalizeFunc normalizer_;
    vectorNormalizeFunc vectorNormalizer_;

    vector<double> normalize(vector<double>);
};

class AudioStream : public ofBaseApp, public IStream {
  public:
    AudioStream();
    void audioIn(float *input, int buffer_size, int nChannel);
    virtual void start() final;
    virtual void stop() final;
    virtual int getNumInputDimensions() final;
  private:
    unique_ptr<ofSoundStream> sound_stream_;
};

class SerialStream : public IStream {
  public:
    SerialStream();
    virtual void start() final;
    virtual void stop() final;
    virtual int getNumInputDimensions() final;
    virtual void useUSBPort(int i);
    virtual void useAnalogPin(int i);
  private:
    int kBaud_ = 115200;
    // Serial buffer size
    int kBufferSize_ = 64;

    unique_ptr<ofSerial> serial_;
    int port_ = -1;
    int pin_;

    // A separate reading thread to read data from Serial.
    unique_ptr<std::thread> reading_thread_;
    void readSerial();
};

class ASCIISerialStream : public IStream {
  public:
    ASCIISerialStream(int kBaud, int numDimensions);
    virtual void start() final;
    virtual void stop() final;
    virtual int getNumInputDimensions() final;
    virtual void useUSBPort(int i);
  private:
    unique_ptr<ofSerial> serial_;
    int kBaud_;
    int port_ = -1;

    int numDimensions_;

    // A separate reading thread to read data from Serial.
    unique_ptr<std::thread> reading_thread_;
    void readSerial();
};

class FirmataStream : public IStream {
  public:
    FirmataStream();
    virtual void start() final;
    virtual void stop() final;
    virtual int getNumInputDimensions() final;
    virtual void useUSBPort(int i);
    virtual void useAnalogPin(int i);
  private:
    int port_ = -1;

    vector<int> pins_;

    bool configured_arduino_;

    ofArduino arduino_;
    unique_ptr<std::thread> update_thread_;
    void update();
};

void useStream(IStream &stream);
void usePipeline(GRT::GestureRecognitionPipeline &pipeline);
