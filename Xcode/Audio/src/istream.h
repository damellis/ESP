/*
 * IStream encapsulates a number of stream input devices: serial,
 * audio, etc.
 */
#pragma once

#include <boost/any.hpp>

#include "ofMain.h"

class IStream : public ofBaseApp {
  public:
    IStream();
    vector<string> getIStreamList();
    void useIStream(int i);
    void audioIn(float *input, int buffer_size, int nChannel);
    void start();
    void stop();
    bool hasStarted() { return is_start_; }

    typedef std::function<void(vector<double>)> onDataReadyCallback;

    void onDataReadyEvent(onDataReadyCallback callback) {
        data_ready_callback_ = callback;
    }

    template<typename T1, typename arg, class T>
    void onDataReadyEvent(T1* owner, void (T::*listenerMethod)(arg)) {
        using namespace std::placeholders;
        data_ready_callback_ = std::bind(listenerMethod, owner, _1);
    }

  private:
    bool is_start_ = false;

    onDataReadyCallback data_ready_callback_;

    // A separate reading thread to read data from Serial.
    unique_ptr<std::thread> reading_thread_;
    void readSerial();

    int buffer_size_ = 256;

    // Because ofSoundStream will call the callback, we use this
    // variable to track whether we have called ofSoundStream::setup
    // or not. It directly affects the behavior of audioIn.
    bool has_audio_setup_ = false;

    // TODO(benzh) Figure out a better way to represent different inputs.
    vector<boost::any> inputs_;
    vector<string> names_;

    unique_ptr<ofSerial> serial_;
    unique_ptr<ofSoundStream> sound_stream_;
};
