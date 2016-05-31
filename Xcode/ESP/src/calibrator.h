#pragma once

#include <GRT/GRT.h>
#include <set>
#include <string>

class CalibrateResult {
  public:
    enum Result {
        SUCCESS,
        WARNING,
        FAILURE,
    };

    CalibrateResult(Result result);
    CalibrateResult(Result result, string message);
  private:
    Result result_;
    string result_message_;

    static const string kDefaultSuccessMessage;
    static const string kDefaultWarningMessage;
    static const string kDefaultFailureMessage;
};

/**
 @brief A data sample to be collected by the user and callback for processing
 that sample.

 A CalibrateProcess stores the name and description of a sample of sensor data
 to be collected by the user, along with a callback for processing that sample
 once it's collected. To use a CalibrateProcess instance, add it to a
 Calibrator with Calibrator::addCalibrateProcess().
 */
class CalibrateProcess {
  public:
    typedef CalibrateResult (*CalibratorCallback)(const GRT::MatrixDouble&);

    /**
    Create a CalibrateProcess.

    @param name: the name of the calibration sample to be collected by the user
    @param description: the description of the calibration sample to be
    collected by the user
    @param cb: the callback to call with the data collected by the user (as a
    MatrixDouble &). Called each time the user collects or re-collects the
    associated calibration sample.
    */
    CalibrateProcess(std::string name, std::string description,
                     CalibratorCallback cb)
            : name_(name), description_(description), cb_(cb),
            is_calibrated_(false) {}

    // run the user callback to apply the calibration function to the
    // calibration data.  this should only be called after setData() is called.
    // TODO(damellis): a way for calibration to fail?
    void calibrate() { cb_(data_); is_calibrated_ = true; }
    bool isCalibrated() const { return is_calibrated_; }

    std::string getName() const { return name_; }
    std::string getDescription() const { return description_; }

    void setData(GRT::MatrixDouble data) { data_ = data; }
    GRT::MatrixDouble getData() { return data_; }
  private:
    std::string name_;
    std::string description_;
    CalibratorCallback cb_;
    bool is_calibrated_;
    GRT::MatrixDouble data_;
};

/**
 @brief Specifies data samples and code used to calibrate incoming sensor data.

 A Calibrator consists of two basic parts. The calibration function transforms
 each sample of incoming live sensor data. It's called with data coming from
 the input stream and its output is passed to the machine learning pipeline.
 Note that incoming data is processed by the calibration function before being
 stored in a training sample (i.e. training samples record calibrated data).

 The calibration processes (CalibrateProcess instances) consist of a data
 sample to be collected by the user and a callback for processing that data
 sample once it's collected. Typically, the calibration process callback
 performs some analysis of the data sample collected by the user and stores
 that information for later use by the calibration function. For instance, a
 calibration process callback might record the mean values of data in the
 sample and use it as a baseline to be subtracted from future live sensor
 inputs in the calibration function.

 The calibration function will not be called until the user has collected a
 sample of data for each CalibrateProcess instance and the corresponding
 callbacks have run. There is, however, no guarantee about the order in which
 the CalibrateProcess callbacks will be executed, as it depends on the order
 in which the user collects the corresponding samples of calibration data.

 To tell the ESP system to use a Calibrator, pass it to useCalibrator() in
 your setup() function.
 */
class Calibrator {
  public:
    /**
     @brief Transforms each dimension of incoming samples of live sensor data.

     SimpleCalibrateFunc are used in cases where the same transformation should
     be applied to each dimension of incoming sensor data. The function is
     called on each dimension of incoming data.
     */
    using SimpleCalibrateFunc = std::function<double(double)>;

    /**
     @brief Transforms incoming samples of live sensor data.

     CalibrateFunc allows for more complex transformations of incoming sensor
     data, e.g. for different dimensions of a sample to be transformed in
     different way, or for transformations that need to be be calculated using
     multiple dimensions of the incoming data. Note that currently the number
     of input dimensions must equal the number of output dimensions (i.e. the
     vector returned by the calibration function must have the same size as
     the vector passed in to the calibration function), although this may
     change in the future.
     */
    using CalibrateFunc = std::function<vector<double>(vector<double>)>;

    /**
     Create a Calibrator without specifying a calibration function. If you use
     this constructor, you'll need to specify the calibration function with
     setCalibrateFunction().
     */
    Calibrator() : simple_calibrate_func_(nullptr), calibrate_func_(nullptr) {}

    /**
     Create a Calibrator with the specified SimpleCalibrateFunc. This function
     will be applied to incoming live sensor data.
     */
    Calibrator(SimpleCalibrateFunc f)
            : simple_calibrate_func_(f), calibrate_func_(nullptr) {}
    /**
     Create a Calibrator with the specified CalibrateFunc. This function will
     be applied to incoming live sensor data.
     */
    Calibrator(CalibrateFunc f)
            : simple_calibrate_func_(nullptr), calibrate_func_(f) {}

    /**
     Set the calibration function. Replaces any currently set calibration
     function.

     @return *this, to allow for chaining of Calibrator methods
     */
    Calibrator& setCalibrateFunction(CalibrateFunc f);

    /**
     Set the calibration function. Replaces any currently set calibration
     function.

     @return *this, to allow for chaining of Calibrator methods
     */
    Calibrator& setCalibrateFunction(SimpleCalibrateFunc f);
    /**
     Add a CalibrateProcess to this Calibrator. The CalibrateProcess instances
     in the active Calibrator (as set by useCalibrator()) are the ones that
     will be shown to the user in the interface.

     @return *this, to allow for chaining of Calibrator methods
     */
    Calibrator& addCalibrateProcess(CalibrateProcess cp);

    /**
     Shortcut for adding a new CalibrateProcess to this Calibrator. This
     creates a new CalibrateProcess with the specified name, description, and
     callback and adds it to the Calibrator. See
     CalibrateProcess::CalibrationProcess() for a description of the arguments.

     @return *this, to allow for chaining of Calibrator methods
     */
    Calibrator& addCalibrateProcess(const string& name,
                                    const string& description,
                                    const CalibrateProcess::CalibratorCallback cb);
    /**
     Get the CalibrateProcess instances in this Calibrator.
     */
    vector<CalibrateProcess>& getCalibrateProcesses() {
        return calibrate_processes_;
    }

    /**
     * Calibrate the input data using functions registered. If a CalibrateFunc
     * is provided and set, use it to calibrate the input; otherwise,
     * SimpleCalibrateFunc must be set and will be used.
     *
     * @param input, an input vector
     * @return output, the filtered result
     */
    vector<double> calibrate(vector<double> input);


    /**
     * Returns true if all calibrate processes are calibrated.
     */
    bool isCalibrated();

  private:
    void registerCalibrateProcess(const CalibrateProcess& cp);

    bool isCalibrateProcessRegistered(const CalibrateProcess& cp);

    SimpleCalibrateFunc simple_calibrate_func_;
    CalibrateFunc calibrate_func_;
    vector<CalibrateProcess> calibrate_processes_;
    std::set<std::string> registered_;
};

/**
 @brief Specify the Calibrator to be used by the ESP system.

 This Calibrator will be applied to data coming from the current input stream
 (IStream instance specified by useInputStream()) before it is passed to the
 current machine learning pipeline (GestureRecognitionPipeline specified by
 usePipeline()). Only one calibrator can be active at a time, but it can
 include multiple CalibrateProcess instances, each of which specifies one
 sample of calibration data to be collected by the user.
 */
void useCalibrator(Calibrator &calibrator);
