#include <ESP.h>

ASCIISerialStream iStream(115200, 3);
TcpOStream oStream("localhost", 5204);
GestureRecognitionPipeline pipeline;
Calibrator calibrator;

double zeroG = 0, oneG = 0;

double processAccelerometerData(double input) {
  return (input - zeroG) / (oneG - zeroG);
}

CalibrateResult restingDataCollected(const MatrixDouble& data) {
    // take average of X and Y acceleration as the zero G value
    zeroG = (data.getMean()[0] + data.getMean()[1]) / 2;
    oneG = data.getMean()[2]; // use Z acceleration as one G value
    
    double range = abs(oneG - zeroG);
    vector<double> stddev = data.getStdDev();
    
    if (stddev[0] / range > 0.05 ||
        stddev[1] / range > 0.05 ||
        stddev[2] / range > 0.05)
        return CalibrateResult(CalibrateResult::WARNING,
            "Accelerometer readings are noisy, check circuit.");
    
    return CalibrateResult::SUCCESS;
}

TrainingSampleCheckerResult checkTrainingSample(const MatrixDouble &in) {
    VectorDouble stddev = in.getStdDev();
    if (*max_element(stddev.begin(), stddev.end()) < 0.1)
        return TrainingSampleCheckerResult(TrainingSampleCheckerResult::WARNING,
            "Warning: Gesture contains very little movement.");
    return TrainingSampleCheckerResult::SUCCESS;
}

double null_rej = 0.4;

void updateVariability(double new_null_rej) {
    pipeline.getClassifier()->setNullRejectionCoeff(new_null_rej);
    pipeline.getClassifier()->recomputeNullRejectionThresholds();
}

void setup() {
    useInputStream(iStream);
    useOutputStream(oStream);

    calibrator.setCalibrateFunction(processAccelerometerData);
    calibrator.addCalibrateProcess("Resting",
        "Rest accelerometer on flat surface.", restingDataCollected);
    useCalibrator(calibrator);
  
    DTW dtw(false, true, null_rej);

    pipeline.setClassifier(dtw);
    usePipeline(pipeline);

    registerTuneable(null_rej, 0.1, 5.0, "Variability",
         "How different from the training data a new gesture can be and "
         "still be considered the same gesture. The higher the number, the "
         "more different it can be.", updateVariability);

    useTrainingSampleChecker(checkTrainingSample);
}
