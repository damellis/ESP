/** @example user_accelerometer_gestures.cpp
 * Gesture detection using accelerometers.
 */
#include <ESP.h>

ASCIISerialStream stream(115200, 3);
GestureRecognitionPipeline pipeline;
Calibrator calibrator;
TcpOStream oStream("localhost", 5204);

double zeroG = 0, oneG = 0;

double processAccelerometerData(double input)
{
    return (input - zeroG) / (oneG - zeroG);
}

CalibrateResult restingDataCollected(const MatrixDouble& data)
{
    // take average of X and Y acceleration as the zero G value
    zeroG = (data.getMean()[0] + data.getMean()[1]) / 2;
    oneG = data.getMean()[2]; // use Z acceleration as one G value
    
    double range = abs(oneG - zeroG);
    vector<double> stddev = data.getStdDev();
    
    if (stddev[0] / range > 0.05 ||
        stddev[1] / range > 0.05 ||
        stddev[2] / range > 0.05)
        return CalibrateResult(CalibrateResult::WARNING,
            "Accelerometer seemed to be moving; consider recollecting the "
            "calibration sample.");
    
    if (abs(data.getMean()[0] - data.getMean()[1]) / range > 0.1)
        return CalibrateResult(CalibrateResult::WARNING,
            "X and Y axes differ by " + std::to_string(
            abs(data.getMean()[0] - data.getMean()[1]) / range * 100) +
            " percent. Check that accelerometer is flat.");

    return CalibrateResult::SUCCESS;
}

TrainingSampleCheckerResult checkTrainingSample(const MatrixDouble &in)
{
    if (in.getNumRows() < 10)
        return TrainingSampleCheckerResult(TrainingSampleCheckerResult::WARNING,
            "Warning: Sample is short. Did you hold down the key for "
            "the whole time you were making the gesture?");
    VectorDouble stddev = in.getStdDev();
    if (*max_element(stddev.begin(), stddev.end()) < 0.1)
        return TrainingSampleCheckerResult(TrainingSampleCheckerResult::WARNING,
            "Warning: Gesture contains very little movement.");
    return TrainingSampleCheckerResult::SUCCESS;
}

int timeout = 500; // milliseconds
double null_rej = 0.4;

void setup()
{
    stream.setLabelsForAllDimensions({"x", "y", "z"});
    useStream(stream);
    useOutputStream(oStream);
    //useStream(stream);

    calibrator.setCalibrateFunction(processAccelerometerData);
    calibrator.addCalibrateProcess("Resting",
        "Rest accelerometer on flat surface.", restingDataCollected);
    useCalibrator(calibrator);

    pipeline.setClassifier(DTW(false, true, null_rej));
    pipeline.addPostProcessingModule(ClassLabelTimeoutFilter(timeout));
    usePipeline(pipeline);

    registerTuneable(null_rej, 0.1, 5.0, "Variability",
         "How different from the training data a new gesture can be and "
         "still be considered the same gesture. The higher the number, the "
         "more different it can be.");
    registerTuneable(timeout, 1, 3000,
        "Timeout",
        "How long (in milliseconds) to wait after recognizing a "
        "gesture before recognizing another one.");
    
    useTrainingSampleChecker(checkTrainingSample);
}
