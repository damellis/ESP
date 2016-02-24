/*
 * OStream supports emitting prediction results to some downstream consumers.
 *
 * For example: Mac OS Keyboard Emulator, OSC Stream, etc.
 */
#pragma once

#include <ApplicationServices/ApplicationServices.h>
#include <cstdarg>
#include <cstdint>

#include "ofMain.h"

class OStream {
  public:
    virtual void onReceive(uint32_t label) = 0;
};

// MacOSKeyboardOStream will emulate keyboard key press event upon receiving
// classification results (via the callback `onReceive`). Users of this class
// can supply a map<integer, char> so that the labels will be translated to
// proper key strokes.
class MacOSKeyboardOStream : public OStream {
  public:
    MacOSKeyboardOStream(std::map<uint32_t, char> key_mapping)
            : key_mapping_(key_mapping) {
    }
    
    MacOSKeyboardOStream(uint32_t count, ...) {
        va_list args;
        va_start(args, count);
        for (uint32_t i = 1; i <= count; i++) {
            key_mapping_[i] = va_arg(args, int);
        }
        va_end(args);
    }

    void onReceive(uint32_t label) {
        ofLog() << key_mapping_[label];
        // sendKey(key_mapping_[label]);
    }

    void sendKey(char c) {
        // Get the process number for the front application.
        ProcessSerialNumber psn = { 0, kNoProcess };
        GetFrontProcess( &psn );
        
        UniChar uni_char = c;
        CGEventRef e = CGEventCreateKeyboardEvent(NULL, 0, true);
        CGEventKeyboardSetUnicodeString(e, 1, &uni_char);
        CGEventPostToPSN(&psn, e);
        CFRelease(e);
    }

    void sendString(const std::string& str) {
        // Get the process number for the front application.
        ProcessSerialNumber psn = { 0, kNoProcess };
        GetFrontProcess( &psn );

        UniChar s[str.length()];
        for (uint32_t i = 0; i < str.length(); i++) {
            s[i] = str[i];
        }
        
        CGEventRef e = CGEventCreateKeyboardEvent(NULL, 0, true);
        CGEventKeyboardSetUnicodeString(e, str.length(), s);
        CGEventPostToPSN(&psn, e);
        CFRelease(e);
    }
    
  private:
    std::map<uint32_t, char> key_mapping_;
};