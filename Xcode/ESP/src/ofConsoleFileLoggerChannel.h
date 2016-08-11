#pragma once

#include <ofLog.h>

/// \brief A logger channel that logs its messages to the console and a log file.
class ofConsoleFileLoggerChannel: public ofBaseLoggerChannel{
public:
	/// \brief Create an ofConsoleFileLoggerChannel.
	ofConsoleFileLoggerChannel();
	
	/// \brief Create an ofConsoleFileLoggerChannel with parameters.
	/// \param path The file path for the log file.
	/// \param append True if the log data should be added to an existing file.
	ofConsoleFileLoggerChannel(const string & path, bool append);

	/// \brief Set the log file.
	/// \param path The file path for the log file.
	/// \param append True if the log data should be added to an existing file.
	void setFile(const string & path,bool append=false);

	void log(ofLogLevel level, const string & module, const string & message);
	void log(ofLogLevel level, const string & module, const char* format, ...) OF_PRINTF_ATTR(4, 5);
	void log(ofLogLevel level, const string & module, const char* format, va_list args);

	/// \brief CLose the log file.
	void close();

private:
	ofConsoleLoggerChannel consoleLogger;
        ofFileLoggerChannel fileLogger;
	
};