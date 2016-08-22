#pragma once

#include "istream.h"
#include "ostream.h"

class IOStream : public virtual InputStream, public OStream {};
class IOStreamVector : public virtual InputStream, public OStreamVector {};

/**
 @brief Input stream for reading ASCII data from a (USB) serial port.

 Data should be formatted as ASCII text, in newline-terminated lines. Each line
 consists of whitespace-separated numbers, e.g.
 @verbatim 123 45 678 90 @endverbatim
 The numbers in each line of text are turned into a single data sample.

 To use an ASCIISerialStream in your application, pass it to useStream() in
 your setup() function.
 */
class ASCIISerialStream : public BaseSerialInputStream, public IOStreamVector {
  public:
    using BaseSerialInputStream::BaseSerialInputStream; // inherit constructors

    virtual void onReceive(uint32_t label);
    virtual void onReceive(vector<double> data);

  private:
    virtual void parseSerial(vector<unsigned char> &buffer);
};

class BinaryIntArraySerialStream : public BaseSerialInputStream, public IOStreamVector {
  public:
    using BaseSerialInputStream::BaseSerialInputStream; // inherit constructors

    virtual void onReceive(uint32_t label);
    virtual void onReceive(vector<double> data);

  private:
    virtual void parseSerial(vector<unsigned char> &buffer);
};
