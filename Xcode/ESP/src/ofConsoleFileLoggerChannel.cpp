#include "ofConsoleFileLoggerChannel.h"

ofConsoleFileLoggerChannel::ofConsoleFileLoggerChannel() {}

ofConsoleFileLoggerChannel::ofConsoleFileLoggerChannel(const string & path, bool append) :
    fileLogger(path, append) {}

void ofConsoleFileLoggerChannel::close() {
    fileLogger.close();
}

void ofConsoleFileLoggerChannel::setFile(const string & path,bool append) {
    fileLogger.setFile(path, append);
}

void ofConsoleFileLoggerChannel::log(
    ofLogLevel level, const string &module, const string &message) {
    if (level >= consoleLogLevel) consoleLogger.log(level, module, message);
    if (level >= fileLogLevel) fileLogger.log(level, module, message);
}

void ofConsoleFileLoggerChannel::log(ofLogLevel level, const string & module, const char* format, ...){
    va_list args;
    va_start(args, format);
    log(level, module, format, args);
    va_end(args);
}

void ofConsoleFileLoggerChannel::log(
    ofLogLevel level, const string & module, const char* format, va_list args) {
    if (level >= consoleLogLevel) consoleLogger.log(level, module, format, args);
    if (level >= fileLogLevel) fileLogger.log(level, module, format, args);
}