#pragma once

#include "istream.h"
#include "ostream.h"

class IOStream : public InputStream, public OStream {};
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

/**
 Tells the ESP system which stream to use, for both input and output. Call
 from your setup() function. The specified stream will be automatically
 started by the ESP system. Note that only one input stream is supported
 at a time; subsequent calls to useStream() will cause the new stream to be
 used for input, replacing input from streams passed to any previous calls to
 useStream() or useInputStream(). Multiple simultaneous output streams are
 supported, however, so calling useStream() will cause output to be sent to
 the specified stream in addition to streams previously specified with
 useStream() or useOutputStream().

 @param stream: the stream to use for input and output
 */
void useStream(IOStream &stream);
void useStream(IOStreamVector &stream);