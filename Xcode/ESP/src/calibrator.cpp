#include "ofApp.h"

#include "Calibrator.h"

void useCalibrator(Calibrator &calibrator) {
    ((ofApp *) ofGetAppPtr())->useCalibrator(calibrator);
}
