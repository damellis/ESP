#include "ofApp.h"
#include "ostream.h"

void useOutputStream(OStream &stream) {
    ((ofApp *) ofGetAppPtr())->useOStream(stream);
}
