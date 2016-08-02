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

void BinaryIntArraySerialStream::onReceive(uint32_t label) {
    serial_->writeByte(label + '0');
    serial_->writeByte('\n');
}

void BinaryIntArraySerialStream::onReceive(vector<double> data) {
    for (int i = 0; i < data.size(); i++) {
        string s = to_string(data[i]);
        if (i > 0) serial_->writeByte('\t');
        for (int j = 0; j < s.size(); j++) serial_->writeByte(s[j]);
    }
    if (data.size() > 0) serial_->writeByte('\n');
}

void BinaryIntArraySerialStream::parseSerial(vector<unsigned char> &buffer) {
    auto start = std::find(buffer.begin(), buffer.end(), 0); // look for packet start
    if (start != buffer.end()) {
        buffer.erase(buffer.begin(), start); // advance to start of packet
        if (buffer.size() >= 3) { // 0, LSB(n), MSB(n)
            unsigned char checksum = 0;
            int LSB = buffer[1]; checksum += LSB;
            int MSB = buffer[2]; checksum += MSB;
            int n = ((MSB & 0x7F) << 7) | (LSB & 0x7F);
            if (buffer.size() >= 4 + 2 * n) {
                vector<double> vals;
                //std::cout << "Got array of " << n << " bytes: " << buffer.size();
                for (int j = 3; j < (3 + 2 * n);) {
                    LSB = buffer[j++]; checksum += LSB;
                    MSB = buffer[j++]; checksum += MSB;
                    int val = ((MSB & 0x7F) << 7) | (LSB & 0x7F);
                    //std::cout << val << " ";
                    vals.push_back(val);
                }
                //std::cout << "(" << (checksum | 0x80) << " <> " << buffer[3 + 2 * n] << ")" << std::endl;
                if ((checksum | 0x80) != buffer[3 + 2 * n]) {
                    ofLog(OF_LOG_WARNING) << "Invalid checksum, discarding serial packet.";
                } else if (n != getNumInputDimensions()) {
                    ofLog(OF_LOG_WARNING) << "Serial packet contains " << n <<
                        " dimensions. Expected " << getNumInputDimensions();
                } else {
                    GRT::MatrixDouble data(1, n);
                    for (int i = 0; i < getNumInputDimensions(); i++) {
                        int b = vals[i];
                        data[0][i] = (normalizer_ != nullptr) ? normalizer_(b) : b;
                    }
                    if (data_ready_callback_ != nullptr) {
                        data_ready_callback_(data);
                    }
                    
                }
                
                buffer.erase(buffer.begin(), buffer.begin() + (4 + 2 * n));
            }
        }
    }
}