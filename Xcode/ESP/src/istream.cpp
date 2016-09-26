#if defined( __WIN32__ ) || defined( _WIN32 )
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

#include "istream.h"

#include <GRT/GRT.h>
#include <chrono>         // std::chrono::milliseconds
#include <thread>         // std::this_thread::sleep_for

extern "C" {
#include <ep/ep.h>
#include <ep/ep_dbg.h>
#include <ep/ep_app.h>
#include <ep/ep_time.h>
#include <gdp/gdp.h>
#include <event2/buffer.h>
}

#include "ofxNetwork.h"  

InputStream::InputStream() : data_ready_callback_(nullptr) {}

vector<double> InputStream::normalize(vector<double> input) {
    if (vectorNormalizer_ != nullptr) {
        return vectorNormalizer_(input);
    } else if (normalizer_ != nullptr) {
        vector<double> output;
        std::transform(input.begin(), input.end(), back_inserter(output), normalizer_);
        return output;
    } else {
        return input;
    }
}

void InputStream::setLabelsForAllDimensions(const vector<string> labels) {
    InputStream_labels_ = labels;
}

void InputStream::setLabelsForAllDimensions(std::initializer_list<string> list) {
    if (list.size() != getNumOutputDimensions()) { return; }
    vector<string> labels(list);
    InputStream_labels_ = labels;
}

const vector<string>& InputStream::getLabels() const {
    return InputStream_labels_;
}

AudioStream::AudioStream(uint32_t downsample_rate)
        : downsample_rate_(downsample_rate),
          sound_stream_(new ofSoundStream()) {
    setup_successful_ = sound_stream_->setup(this, 0, 2,
                                             kOfSoundStream_SamplingRate,
                                             kOfSoundStream_BufferSize,
                                             kOfSoundStream_nBuffers);
    sound_stream_->stop();
}

bool AudioStream::start() {
    if (!setup_successful_) return false;
    if (!has_started_) {
        sound_stream_->start();
        has_started_ = true;
    }
    return true;
}

void AudioStream::stop() {
    if (has_started_) {
        sound_stream_->stop();
        has_started_ = false;
    }
}

int AudioStream::getNumInputDimensions() {
    return 1; // set by the call to sound_stream->setup() above
}

void AudioStream::audioIn(float* input, int buffer_size, int nChannel) {
    // set nChannelOut as 1 to load only a single channel (left).
    int nChannelOut = 1;
    GRT::MatrixDouble data(buffer_size / nChannel / downsample_rate_, nChannelOut);

    for (int i = 0; i < buffer_size / nChannel / downsample_rate_; i++)
        for (int j = 0; j < nChannelOut; j++)
            data[i][j] = input[i * nChannel * downsample_rate_ + j];

    if (data_ready_callback_ != nullptr) {
        data_ready_callback_(data);
    }
}

AudioFileStream::AudioFileStream(char *file, bool loop) {
    player_.load(file);
    player_.setLoop(loop);
}

bool AudioFileStream::start() {
    if (!has_started_) {
        player_.play();
        update_thread_.reset(new std::thread(&AudioFileStream::readSpectrum, this));
        has_started_ = true;
    }

    return true;
}

void AudioFileStream::stop() {
    player_.stop();
    has_started_ = false;
    if (update_thread_ != nullptr && update_thread_->joinable()) {
        update_thread_->join();
    }
}

void AudioFileStream::readSpectrum() {
    while (has_started_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / (44100 / 1024)));
        float *spectrum = ofSoundGetSpectrum(512);
        GRT::VectorDouble data(spectrum, spectrum + 512);
        GRT::MatrixDouble out; out.push_back(data);
        if (data_ready_callback_ != nullptr) data_ready_callback_(out);
    }
}

int AudioFileStream::getNumInputDimensions() {
    return 512;
}

BaseSerialInputStream::BaseSerialInputStream(uint32_t baud, int dimensions)
        : port_(-1), baud_(baud), dimensions_(dimensions), serial_(new ofSerial()) {
    // Print all devices for convenience.
    // serial_->listDevices();
}

BaseSerialInputStream::BaseSerialInputStream(uint32_t port, uint32_t baud, int dimensions)
        : port_(port), baud_(baud), dimensions_(dimensions), serial_(new ofSerial()) {
    // Print all devices for convenience.
    // serial_->listDevices();
}

bool BaseSerialInputStream::start() {
    if (port_ == -1) {
        ofLog(OF_LOG_ERROR) << "USB Port will be selected by user.";
        return false;
    }

    if (!has_started_) {
        if (!serial_->setup(port_, baud_)) return false;
        reading_thread_.reset(new std::thread(&BaseSerialInputStream::readSerial, this));
        has_started_ = true;
    }

    return true;
}

void BaseSerialInputStream::stop() {
    has_started_ = false;
    if (reading_thread_ != nullptr && reading_thread_->joinable()) {
        reading_thread_->join();
    }
}

int BaseSerialInputStream::getNumInputDimensions() {
    return dimensions_;
}

void BaseSerialInputStream::readSerial() {
    // TODO(benzh) This readSerial is running in a different thread
    // and performing a busy polling (100% CPU usage). Should be
    // optimized.
    int sleep_time = 1000 / (baud_ / 10); // sleep about long enough to receive a byte
    ofLog() << "Serial port will be read every " << sleep_time << " ms";
    while (has_started_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        const int kBufSize = 32;
        unsigned char buf[kBufSize];
        while (serial_->available() > 0) {
            int result = serial_->readBytes(buf, kBufSize);

            if ( result == OF_SERIAL_ERROR ) {
                ofLog( OF_LOG_ERROR, "unrecoverable error reading from serial" );
                break;
            } else if ( result != OF_SERIAL_NO_DATA ) {
                buffer_.insert(buffer_.end(), buf, buf + result);
            }
        }

        parseSerial(buffer_);
    }
}

SerialStream::SerialStream(uint32_t port, uint32_t baud = 115200)
        : port_(port), baud_(baud), serial_(new ofSerial()) {
    // Print all devices for convenience.
    // serial_->listDevices();
}

bool SerialStream::start() {
    if (port_ == -1) {
        ofLog(OF_LOG_ERROR) << "USB Port has not been properly set";
        return false;
    }

    if (!has_started_) {
        if (!serial_->setup(port_, baud_)) return false;
        reading_thread_.reset(new std::thread(&SerialStream::readSerial, this));
        has_started_ = true;
    }

    return true;
}

void SerialStream::stop() {
    has_started_ = false;
    if (reading_thread_ != nullptr && reading_thread_->joinable()) {
        reading_thread_->join();
    }
}

int SerialStream::getNumInputDimensions() {
    return 1;
}

void SerialStream::readSerial() {
    // TODO(benzh) This readSerial is running in a different thread
    // and performing a busy polling (100% CPU usage). Should be
    // optimized.
    int sleep_time = kBufferSize_ * 1000 / (baud_ / 10);
    ofLog() << "Serial port will be read every " << sleep_time << " ms";
    while (has_started_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

        int local_buffer_size = kBufferSize_;
        int bytesRequired = kBufferSize_;
        uint8_t *bytes = new uint8_t[bytesRequired];
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
                }
                else if (result == OF_SERIAL_NO_DATA) {
                    // nothing was read, try again
                }
                else {
                    // we read some data!
                    bytesRemaining -= result;
                }
            }
        }
        delete[] bytes;
        GRT::MatrixDouble data(local_buffer_size, 1);
        for (int i = 0; i < local_buffer_size; i++) {
            int b = bytes[i];
            data[i][0] = (normalizer_ != nullptr) ? normalizer_(b) : b;
        }
        if (data_ready_callback_ != nullptr) {
            data_ready_callback_(data);
        }
    }
}

FirmataStream::FirmataStream(uint32_t port) : port_(port) {
    ofSerial serial;
    serial.listDevices();
}

void FirmataStream::useAnalogPin(int i) {
    pins_.push_back(i);
};

bool FirmataStream::start() {
    if (port_ == -1) {
        ofLog(OF_LOG_ERROR) << "USB Port has not been properly set";
        return false;
    }

    if (pins_.size() == 0) {
        ofLog(OF_LOG_ERROR) << "Pin has not been properly set";
        return false;
    }


    if (!has_started_) {
        ofSerial serial;
        configured_arduino_ = false;
        if (!arduino_.connect(serial.getDeviceList()[port_].getDevicePath()))
            return false;
        update_thread_.reset(new std::thread(&FirmataStream::update, this));
        has_started_ = true;
    }

    return true;
}

void FirmataStream::stop() {
    has_started_ = false;
    if (update_thread_ != nullptr && update_thread_->joinable()) {
        update_thread_->join();
    }
}

int FirmataStream::getNumInputDimensions() {
    return pins_.size();
}

void FirmataStream::update() {
    int sleep_time = 10;
    ofLog() << "Serial port will be read every " << sleep_time << " ms";
    while (has_started_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        arduino_.update();

        if (configured_arduino_) {
            vector<double> data(pins_.size());
            for (int i = 0; pins_.size(); i++)
                data[i] = arduino_.getAnalog(pins_[i]);
            data = normalize(data);
            GRT::MatrixDouble matrix;
            matrix.push_back(data);
            if (data_ready_callback_ != nullptr) data_ready_callback_(matrix);
        } else if (arduino_.isInitialized()) {
            ofLog() << "Configuring Arduino.";
            for (int i = 0; i < pins_.size(); i++)
                arduino_.sendAnalogPinReporting(pins_[i], ARD_ON);
            configured_arduino_ = true;
        }
    }
}

void gdp_callback(gdp_event_t *gev) {
	cout << "Got GDP event of type " << gdp_event_gettype(gev) << endl;
	gdp_datum_t *datum = gdp_event_getdatum(gev);
	EP_TIME_SPEC ts;
	gdp_datum_getts(datum, &ts);
	cout << "Datum: #" << gdp_datum_getrecno(datum) << " at "
	     << ts.tv_sec << endl;
	gdp_buf_t *buf = gdp_datum_getbuf(datum);
	gdp_datum_print(datum, stdout, GDP_DATUM_PRTEXT);
	gdp_event_free(gev);
}

GDPStream::GDPStream(const char *log_name) {
        gdp_name_t gclname;
        gdp_iomode_t open_mode = GDP_MODE_RO;
        gdp_gcl_t *gcl;
    
	EP_STAT estat = gdp_init(NULL);
	if (!EP_STAT_ISOK(estat))
	{
		ep_app_error("GDP Initialization failed");
		return;
	}

	// allow thread to settle to avoid interspersed debug output
	ep_time_nanosleep(INT64_C(100000000));		// 100 msec

	// parse the name (either base64-encoded or symbolic)
	estat = gdp_parse_name(log_name, gclname);
	if (!EP_STAT_ISOK(estat))
	{
		ep_app_fatal("illegal GCL name syntax:\n\t%s", log_name);
		return;
	}

	// convert it to printable format and tell the user what we are doing
        gdp_pname_t gclpname;

        gdp_printable_name(gclname, gclpname);
        fprintf(stderr, "Reading GCL %s\n", gclpname);

	// open the GCL; arguably this shouldn't be necessary
	estat = gdp_gcl_open(gclname, open_mode, NULL, &gcl);
	if (!EP_STAT_ISOK(estat))
	{
		char sbuf[100];

		ep_app_error("Cannot open GCL:\n    %s",
				ep_stat_tostr(estat, sbuf, sizeof sbuf));
		return;
	}
	
	EP_TIME_SPEC now;
	
	gdp_gcl_subscribe(gcl, 1, 0, NULL, gdp_callback, (void *)this);
}

bool TcpInputStream::start() {
	server_ = new ofxTCPServer();
    server_->setup(port_num_);
    server_->setMessageDelimiter("\n");
    has_started_ = true;

    reading_thread_.reset(new std::thread([this]() {
        int sleep_time = 10;
        ofLog() << "TCP inputs are checked every " << sleep_time << " ms";
        while (has_started_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            for (int i = 0; i < server_->getLastID(); i++) {
                if (server_->isClientConnected(i)) {
                    string str = server_->receive(i);
                    if (str != "") {
                        parseInput(str);
                    }
                }
            }
        }
    }));
    return true;
}

void TcpInputStream::parseInput(const string& buffer) {
    if (data_ready_callback_ != nullptr) {
        istringstream iss(buffer);
        vector<double> data;
        double d;

        while (iss >> d)
            data.push_back(d);

        if (data.size() > 0) {
            data = normalize(data);

            GRT::MatrixDouble matrix;
            matrix.push_back(data);

            data_ready_callback_(matrix);
        }
    }
}

void TcpInputStream::stop() {
    has_started_.store(false);
    server_->close();
    if (reading_thread_ != nullptr && reading_thread_->joinable()) {
        reading_thread_->join();
    }
}

int TcpInputStream::getNumInputDimensions() {
    return dim_;
}

bool OscInputStream::start() {
    receiver_.setup(port_num_);
    has_started_ = true;

    reading_thread_.reset(new std::thread([this]() {
        int sleep_time = 10;
        ofLog() << "OSC inputs are checked every " << sleep_time << " ms";
        while (has_started_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

            ofxOscMessage m;
            while (receiver_.hasWaitingMessages()) {
                receiver_.getNextMessage(m);
                handleMessage(m);
            }
        }
    }));
    return true;
}

void OscInputStream::handleMessage(ofxOscMessage& m) {
    // check for mouse moved message
    if (data_ready_callback_ != nullptr && m.getAddress() == addr_) {
        vector<double> data;
        GRT::MatrixDouble matrix;
        for (int i = 0; i < dim_; i++) {
            data.push_back(m.getArgAsFloat(i));
        }
        matrix.push_back(data);
        data_ready_callback_(matrix);
    }
}

void OscInputStream::stop() {
    has_started_.store(false);
    if (reading_thread_ != nullptr && reading_thread_->joinable()) {
        reading_thread_->join();
    }
}

int OscInputStream::getNumInputDimensions() {
    return dim_;
}
