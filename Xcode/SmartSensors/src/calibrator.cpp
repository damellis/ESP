#include "ofApp.h"

#include "Calibrator.h"

void useCalibrator(Calibrator &calibrator) {
    ((ofApp *) ofGetAppPtr())->useCalibrator(calibrator);
}

Calibrator::Calibrator(std::string name, std::string description, CalibratorCallback cb) : name_(name), description_(description), cb_(cb), is_calibrated_(false) {}
