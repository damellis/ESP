#include <SmartSensors.h>

ASCIISerialStream stream(0, 9600, 3);
GestureRecognitionPipeline pipeline;
Calibrator calibrator;
TcpOStream oStream("localhost", 5204, 3, "l", "r", " ");

// accelerometer characteristics to be calculated from calibration
double zeroG, oneG;

vector<double> processAccelerometerData(vector<double> input)
{
    if (input.size() < 3) return input;

    vector<double> output(input.size());

    output[0] = (input[0] - zeroG) / (oneG - zeroG);
    output[1] = (input[1] - zeroG) / (oneG - zeroG);
    output[2] = (input[2] - zeroG) / (oneG - zeroG);

    return output;
}

void restingDataCollected(const MatrixDouble& data)
{
    std::vector<float> mean(3, 0.0);

    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < data.getNumRows(); i++)
        {
            mean[j] += data[i][j];
        }
        mean[j] /= data.getNumRows();
    }

    // TODO: give warning if mean[0] (X acceleration) and mean[1] (Y accleration) are different.
    zeroG = (mean[0] + mean[1]) / 2; // take average of X and Y acceleration as the zero G value
    oneG = mean[2]; // use Z acceleration as one G value (due to gravity)
}

float analogReadToVoltage(float input)
{
    return input / 1024.0 * 5.0;
}

int timeout = 500;
double threshold = 0.4;

void setup()
{
    stream.setLabelsForAllDimensions({"x", "y", "z"});
    useStream(stream);

    calibrator.setCalibrateFunction(processAccelerometerData);
    // The elaborate version is:
    // CalibrateProcess cp("Resting", "Rest accelerometer on flat surface, w/ z-axis vertical.", restingDataCollected);
    // calibrator.addCalibrateProcess(cp);
    calibrator.addCalibrateProcess("Resting", "Rest accelerometer on flat surface, w/ z-axis vertical.", restingDataCollected);
    useCalibrator(calibrator);

    pipeline.setClassifier(DTW(false, true, threshold));
    pipeline.addPostProcessingModule(ClassLabelTimeoutFilter(timeout));
    usePipeline(pipeline);

    registerTuneable(threshold, 0.1, 3.0,
        "Distance Threshold",
        "How similar a live gesture needs to be to a training sample. "
        "The lower the number, the more similar it needs to be.");
    registerTuneable(timeout, 10, 1000,
        "Delay Between Gestures",
        "How long (in milliseconds) to wait after recognizing a "
        "gesture before recognizing another one.");

    useOStream(oStream);
}
