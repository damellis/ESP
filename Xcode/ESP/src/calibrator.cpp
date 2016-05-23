#include "ofApp.h"

#include "Calibrator.h"

Calibrator& Calibrator::setCalibrateFunction(SimpleCalibrateFunc f) {
    simple_calibrate_func_ = f;
    calibrate_func_ = nullptr;
    return *this;
}

Calibrator& Calibrator::setCalibrateFunction(CalibrateFunc f) {
    simple_calibrate_func_ = nullptr;
    calibrate_func_ = f;
    return *this;
}

Calibrator& Calibrator::addCalibrateProcess(CalibrateProcess cp) {
    if (!isCalibrateProcessRegistered(cp)) {
        registerCalibrateProcess(cp);
        calibrate_processes_.push_back(cp);
    }
    return *this;
}

using CalibratorCallback = CalibrateProcess::CalibratorCallback;
Calibrator& Calibrator::addCalibrateProcess(const string& name,
                                            const string& description,
                                            const CalibratorCallback cb) {
    CalibrateProcess cp(name, description, cb);
    return addCalibrateProcess(cp);
}

vector<double> Calibrator::calibrate(vector<double> input) {
    if (calibrate_func_ != nullptr) {
        return calibrate_func_(input);
    } else {
        assert(simple_calibrate_func_ != nullptr);
        vector<double> output;
        std::transform(input.begin(), input.end(), back_inserter(output),
                       simple_calibrate_func_);
        return output;
    }
}

bool Calibrator::isCalibrated() {
    for (const auto& cp : calibrate_processes_) {
        if (!cp.isCalibrated()) {
            return false;
        }
    }
    return true;
}

void Calibrator::registerCalibrateProcess(const CalibrateProcess& cp) {
    registered_.insert(cp.getName());
}

bool Calibrator::isCalibrateProcessRegistered(const CalibrateProcess& cp) {
    return registered_.find(cp.getName()) != registered_.end();
}


void useCalibrator(Calibrator &calibrator) {
    ((ofApp *) ofGetAppPtr())->useCalibrator(calibrator);
}
