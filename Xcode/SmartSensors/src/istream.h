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

    void toggle() {
        if (has_started_) { stop(); }
        else { start(); }
    }

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

class AudioStream : public ofBaseApp, public IStream {
  public:
    AudioStream(uint32_t downsample_rate = 1);
    void audioIn(float *input, int buffer_size, int nChannel);
    virtual void start() final;
    virtual void stop() final;
    virtual int getNumInputDimensions() final;
  private:
    uint32_t downsample_rate_;
    unique_ptr<ofSoundStream> sound_stream_;
};

class SerialStream : public IStream {
  public:
    SerialStream(uint32_t usb_port_num, uint32_t baud);
    virtual void start() final;
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

class ASCIISerialStream : public IStream {
  public:
    ASCIISerialStream(uint32_t port, uint32_t baud, uint32_t numDimensions);
    virtual void start() final;
    virtual void stop() final;
    virtual int getNumInputDimensions() final;
  private:
    unique_ptr<ofSerial> serial_;
    uint32_t port_;
    uint32_t baud_;
    uint32_t numDimensions_;

    // A separate reading thread to read data from Serial.
    unique_ptr<std::thread> reading_thread_;
    void readSerial();
};

class FirmataStream : public IStream {
  public:
    FirmataStream(uint32_t port);
    virtual void start() final;
    virtual void stop() final;
    virtual int getNumInputDimensions() final;
    void useAnalogPin(int i);
  private:
    uint32_t port_;

    vector<int> pins_;

    bool configured_arduino_;

    ofArduino arduino_;
    unique_ptr<std::thread> update_thread_;
    void update();
};

void useStream(IStream &stream);
void usePipeline(GRT::GestureRecognitionPipeline &pipeline);
