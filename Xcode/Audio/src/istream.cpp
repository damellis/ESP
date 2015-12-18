#include "istream.h"

IStream::IStream()
        : is_start_(false), buffer_size_(256),
          serial_(new ofSerial()), sound_stream_(new ofSoundStream()) {
}

vector<string> IStream::getIStreamList() {
    vector<ofSerialDeviceInfo> serial_devices = serial_->getDeviceList();
    vector<ofSoundDevice> sound_devices = sound_stream_->getDeviceList();

    for (auto& d : serial_devices) {
        const string& name = d.getDeviceName();
        names_.emplace_back(name);
        inputs_.emplace_back(d);
    }

    for (const auto& d : sound_devices) {
        if (d.isDefaultInput) {
            names_.emplace_back(d.name);
            inputs_.emplace_back(d);
        }
    }
    return names_;
}

void IStream::useIStream(int i) {
    // TODO(benzh) Work on support select different streams.
    ofLog(OF_LOG_ERROR) << "Unsupported operation yet";
    ofLog() << "selected " << i;

    // Should use inputs_[i], for now let's hardcode it to audio
    if (!has_audio_setup_) {
        ofLog() << "Use Audio!";
        sound_stream_->setup(this, 0, 2, 44100, buffer_size_, 4);
        has_audio_setup_ = true;
    }
    sound_stream_->stop();

    // TODO(benzh) For now, we are only using built-in microphone.
    return;
    vector<ofSerialDeviceInfo> serial_devices = serial_->getDeviceList();
    for (auto& d : serial_devices) {
        if (names_[i].compare(d.getDeviceName()) != 0) {
            serial_->setup(i, 115200);
            return;
        }
    }
}

//--------------------------------------------------------------
void IStream::audioIn(float* input, int buffer_size, int nChannel) {
    vector<double> data(buffer_size);

    for (int i = 0; i < buffer_size; i++) {
        // only left channel
        data[i] = input[i * 2];
    }

    if (data_ready_callback_ != nullptr) {
        data_ready_callback_(data);
    }
}

void IStream::start() {
    if (has_audio_setup_ && !is_start_) {
        sound_stream_->start();
        is_start_ = true;
        return;
    }

    // Otherwise read from serial.
    is_start_ = true;
    reading_thread_.reset(new std::thread(&IStream::readSerial, this));
}

void IStream::readSerial() {
    // TODO(benzh) This readSerial is running in a different thread
    // and performing a busy polling (100% CPU usage). Should be
    // optimized.
    while (is_start_) {
        int local_buffer_size = 24;
        int bytesRequired = local_buffer_size * 2;
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
                }
                else {
                    // we read some data!
                    bytesRemaining -= result;
                }
            }
        }
        vector<double> data(local_buffer_size);
        for (int i = 0; i < local_buffer_size; i++) {
            int b1 = bytes[i];
            int b2 = bytes[i + 1];
            data[i] = b1 + (b2 << 8);
        }
        if (data_ready_callback_ != nullptr) {
            data_ready_callback_(data);
        }
    }
}

void IStream::stop() {
    if (has_audio_setup_) {
        sound_stream_->stop();
        return;
    }

    is_start_ = false;
}
