#include "iostream.h"

ASCIISerialStream::ASCIISerialStream(uint32_t port, uint32_t baud, uint32_t dim)
        : serial_(new ofSerial()), port_(port), baud_(baud), numDimensions_(dim) {
}

ASCIISerialStream::ASCIISerialStream(uint32_t baud, uint32_t dim)
        : serial_(new ofSerial()), port_(-1), baud_(baud), numDimensions_(dim) {
}

bool ASCIISerialStream::start() {
    if (port_ == -1) {
        ofLog(OF_LOG_ERROR) << "USB Port will be selected by user";
        return false;
    }

    if (!has_started_) {
        if (!serial_->setup(port_, baud_)) return false;
        reading_thread_.reset(new std::thread(&ASCIISerialStream::readSerial, this));
        has_started_ = true;
    }

    return true;
}

void ASCIISerialStream::stop() {
    has_started_ = false;
    if (reading_thread_ != nullptr && reading_thread_->joinable()) {
        reading_thread_->join();
    }
}

void ASCIISerialStream::onReceive(uint32_t label) {
    serial_->writeByte(label + '0');
    serial_->writeByte('\n');
}

int ASCIISerialStream::getNumInputDimensions() {
    return numDimensions_;
}

void ASCIISerialStream::readSerial() {
    // TODO(benzh) This readSerial is running in a different thread
    // and performing a busy polling (100% CPU usage). Should be
    // optimized.
    int sleep_time = 10;
    ofLog() << "Serial port will be read every " << sleep_time << " ms";
    while (has_started_) {
        string s;

        do {
            while (serial_->available() < 1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            }
            s += serial_->readByte();
        } while(s[s.length() - 1] != '\n');

        //ofLog() << "read: '" << s << "'" << endl;

        if (data_ready_callback_ != nullptr) {
            istringstream iss(s);
            vector<double> data;
            double d;

            while (iss >> d) data.push_back(d);

            if (data.size() > 0) {
                data = normalize(data);

                GRT::MatrixDouble matrix;
                matrix.push_back(data);

                data_ready_callback_(matrix);
            }
        }
    }
}

