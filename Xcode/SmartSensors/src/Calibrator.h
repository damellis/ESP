#ifndef Calibrator_h
#define Calibrator_h

#include <string>

#include <GRT/GRT.h>

class Calibrator {
  public:
    typedef void (*CalibratorCallback)(Calibrator &);
    
    Calibrator(std::string name, std::string description, CalibratorCallback cb);
    
    // run the user callback to apply the calibration function to the calibration data.
    // this should only be called after setData() is called.
    // TODO(damellis): a way for calibration to fail?
    void calibrate() { cb_(*this); is_calibrated_ = true; }
    bool isCalibrated() { return is_calibrated_; }
    
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

void useCalibrator(Calibrator &calibrator);

#endif
