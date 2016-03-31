#pragma once

#include <string>

#include <GRT/GRT.h>

class CalibrateProcess {
  public:
    typedef void (*CalibratorCallback)(const MatrixDouble&);

    CalibrateProcess(std::string name, std::string description, CalibratorCallback cb)
            : name_(name), description_(description), cb_(cb), is_calibrated_(false) {}

    // run the user callback to apply the calibration function to the
    // calibration data.  this should only be called after setData() is called.
    // TODO(damellis): a way for calibration to fail?
    void calibrate() { cb_(data_); is_calibrated_ = true; }
    bool isCalibrated() const { return is_calibrated_; }

    std::string getName() { return name_; }
    std::string getDescription() { return description_; }

    void setData(GRT::MatrixDouble data) { data_ = data; }
    GRT::MatrixDouble getData() { return data_; }
  private:
    bool is_calibrated_;
    CalibratorCallback cb_;
    std::string name_;
    std::string description_;
    GRT::MatrixDouble data_;
};

class Calibrator {
  public:
    typedef std::function<vector<double>(vector<double>)> CalibrateFunc;
    Calibrator(CalibrateFunc f) : calibrate_func_(f) {}

    Calibrator& addCalibrateProcess(const CalibrateProcess cp) {
        calibrate_processes_.push_back(cp);
        return *this;
    }

    Calibrator& addCalibrateProcess(const string& name, const string& description,
                                    const CalibrateProcess::CalibratorCallback cb) {
        CalibrateProcess cp(name, description, cb);
        return addCalibrateProcess(cp);
    }

    vector<CalibrateProcess>& getCalibrateProcesses() {
        return calibrate_processes_;
    }

    vector<double> calibrate(vector<double> input) {
        if (calibrate_func_ != nullptr) {
            return calibrate_func_(input);
        }
    }

    bool isCalibrated() {
        for (const auto& cp : calibrate_processes_) {
            if (!cp.isCalibrated()) {
                return false;
            }
        }
        return true;
    }

  private:
    CalibrateFunc calibrate_func_;
    vector<CalibrateProcess> calibrate_processes_;
};

void useCalibrator(Calibrator &calibrator);
