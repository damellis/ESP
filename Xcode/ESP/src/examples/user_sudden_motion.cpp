#include <ESP.h>

ASCIISerialStream stream(0, 9600, 3);
GestureRecognitionPipeline pipeline;
Calibrator calibrator;

double zeroG = 0, oneG = 0;

double processAccelerometerData(double input) {
    return (input - zeroG) / (oneG - zeroG);
}

void restingDataCollected(const MatrixDouble& data) {
    // take average of X and Y acceleration as the zero G value
    zeroG = (data.getMean()[0] + data.getMean()[1]) / 2;
    oneG = data.getMean()[2]; // use Z acceleration as one G value
}

int num_dim = 3;

void setup() {
    stream.setLabelsForAllDimensions({"x", "y", "z"});
    useInputStream(stream);

    calibrator.setCalibrateFunction(processAccelerometerData);
    calibrator.addCalibrateProcess("Resting",
        "Rest accelerometer on flat surface.", restingDataCollected);
    useCalibrator(calibrator);

    pipeline.addPreProcessingModule(
        Derivative(Derivative::FIRST_DERIVATIVE, 1, num_dim));
    pipeline.addFeatureExtractionModule(
        ZeroCrossingCounter(3, 0.1, num_dim, ZeroCrossingCounter::COMBINED_FEATURE_MODE));
    usePipeline(pipeline);
}
