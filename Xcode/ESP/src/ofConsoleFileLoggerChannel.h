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
    
        /// \brief Set the minimum log level for the console log.
        ///
        /// Note that this is additional level of filtering beyond that
        /// provided by the global ofSetLogLevel(). Anything below that log
        /// level will not be logged, regardless of the value of the
        /// console-specific log level.
        /// \param level the log level to apply to the console log (defaults to
        /// OF_LOG_VERBOSE)
        void setConsoleLogLevel(ofLogLevel level) { consoleLogLevel = level; }

        /// \brief Set the minimum log level for the file log.
        ///
        /// Note that this is additional level of filtering beyond that
        /// provided by the global ofSetLogLevel(). Anything below that log
        /// level will not be logged, regardless of the value of the
        /// file-specific log level.
        /// \param level the log level to apply to the file log (defaults to
        /// OF_LOG_VERBOSE)
        void setFileLogLevel(ofLogLevel level) { fileLogLevel = level; }

private:
	ofConsoleLoggerChannel consoleLogger;
        ofFileLoggerChannel fileLogger;
    
        ofLogLevel consoleLogLevel = OF_LOG_VERBOSE;
        ofLogLevel fileLogLevel = OF_LOG_VERBOSE;
};