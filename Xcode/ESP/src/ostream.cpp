#include "ofApp.h"
#include "ostream.h"

void useOStream(OStream &stream) {
    ((ofApp *) ofGetAppPtr())->useOStream(stream);
}
