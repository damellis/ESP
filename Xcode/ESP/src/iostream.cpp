#include "ofApp.h"
#include "iostream.h"

void useStream(IOStream &stream) {
    ((ofApp *) ofGetAppPtr())->useIStream(stream);
    ((ofApp *) ofGetAppPtr())->useOStream(stream);
}

void useStream(IOStreamVector &stream) {
    ((ofApp *) ofGetAppPtr())->useIStream(stream);
    ((ofApp *) ofGetAppPtr())->useOStream(stream);
}

void ASCIISerialStream::onReceive(uint32_t label) {
    serial_->writeByte(label + '0');
    serial_->writeByte('\n');
}

void ASCIISerialStream::onReceive(vector<double> data) {
    for (int i = 0; i < data.size(); i++) {
        string s = to_string(data[i]);
        if (i > 0) serial_->writeByte('\t');
        for (int j = 0; j < s.size(); j++) serial_->writeByte(s[j]);
    }
    if (data.size() > 0) serial_->writeByte('\n');
}

void ASCIISerialStream::parseSerial(vector<unsigned char> &buffer) {
    auto newline = find(buffer.begin(), buffer.end(), '\n');
    if (newline != buffer.end()) {
        string s(buffer.begin(), newline);
        
        buffer.erase(newline);
        buffer.erase(buffer.begin(), newline);
        
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
