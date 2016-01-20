#include "istream.h"

IStream::IStream() : has_started_(false), data_ready_callback_(nullptr) {}

AudioStream::AudioStream() :
        sound_stream_(new ofSoundStream()) {
    sound_stream_->setup(this, 0, 2, 44100, 256, 4);
    sound_stream_->stop();
}

void AudioStream::start() {
    if (!has_started_) {
        sound_stream_->start();
        has_started_ = true;
    }
}

void AudioStream::stop() {
    if (has_started_) {
        sound_stream_->stop();
        has_started_ = false;
    }
}

void AudioStream::audioIn(float* input, int buffer_size, int nChannel) {
    vector<double> data(buffer_size);

    for (int i = 0; i < buffer_size; i++) {
        // only left channel
        data[i] = input[i * 2];
    }

    if (data_ready_callback_ != nullptr) {
        data_ready_callback_(data);
    }
}

SerialStream::SerialStream() : serial_(new ofSerial()) {
    // Print all devices for convenience.
    serial_->listDevices();
}

void SerialStream::start() {
    if (port_ == -1) {
        ofLog(OF_LOG_ERROR) << "USB Port has not been properly set";
    }

    if (!has_started_) {
        serial_->setup(port_, 115200);
        reading_thread_.reset(new std::thread(&SerialStream::readSerial, this));
        has_started_ = true;
    }
}

void SerialStream::stop() {
    has_started_ = false;
    reading_thread_->join();
}

void SerialStream::useUSBPort(int i) {
    port_ = i;
};

void SerialStream::useAnalogPin(int i) {
    pin_ = i;
};

void SerialStream::readSerial() {
    // TODO(benzh) This readSerial is running in a different thread
    // and performing a busy polling (100% CPU usage). Should be
    // optimized.
    while (has_started_) {
        int local_buffer_size = buffer_size_ / 2;
        int bytesRequired = buffer_size_;
        unsigned char bytes[bytesRequired];
        int bytesRemaining = bytesRequired;
        while (bytesRemaining > 0) {
            // check for data
            if (serial_->available() > 0) {
                // try to read - note offset into the bytes[] array, this is so
                // that we don't overwrite the bytes we already have
                int bytesArrayOffset = bytesRequired - bytesRemaining;
                int result = serial_->readBytes(&bytes[bytesArrayOffset],
                                                bytesRemaining);

                // check for error code
                if (result == OF_SERIAL_ERROR) {
                    // something bad happened
                    ofLog(OF_LOG_ERROR) << "Error reading from serial";
                    break;
                } else if (result == OF_SERIAL_NO_DATA) {
                    // nothing was read, try again
                } else {
                    // we read some data!
                    bytesRemaining -= result;
                }
            }
        }
        vector<double> data(local_buffer_size);
        for (int i = 0; i < local_buffer_size; i++) {
            int b = bytes[i];
            data[i] = (normalizer_ != nullptr) ? normalizer_(b) : b;
        }
        if (data_ready_callback_ != nullptr) {
            data_ready_callback_(data);
        }
    }
}
