/** @example user_accelerometer_poses.cpp
 * Pose detection using accelerometers.
 */
#include <ESP.h>

ASCIISerialStream stream(0, 9600, 3);
GestureRecognitionPipeline pipeline;
Calibrator calibrator;
TcpOStream oStream("localhost", 5204, 3, "l", "r", " ");

float analogReadToVoltage(float input)
{
    return input / 1024.0 * 5.0;
}

float normalizeADXL335(float input)
{
    return (analogReadToVoltage(input) - 1.66) / 0.333;
}

float normalizeArduino101(float input)
{
    return input / 4096;
}

// accelerometer characteristics to be calculated from calibration
double zeroG, oneG;
double calibration_tolerance = 0.1; // 0.1 g difference

vector<double> processAccelerometerData(vector<double> input)
{
    if (input.size() < 3) return input;

    vector<double> output(input.size());

    output[0] = (input[0] - zeroG) / (oneG - zeroG);
    output[1] = (input[1] - zeroG) / (oneG - zeroG);
    output[2] = (input[2] - zeroG) / (oneG - zeroG);

    return output;
}

CalibrateResult restingDataCollected(const MatrixDouble& data)
{
    std::vector<float> mean(3, 0.0);

    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < data.getNumRows(); i++)
        {
            mean[j] += data[i][j];
        }
        mean[j] /= data.getNumRows();
    }

    // TODO: give warning if mean[0] (X acceleration) and mean[1] (Y
    // accleration) are different.
    if (std::abs(mean[0] - mean[1]) > calibration_tolerance) {
        return CalibrateResult(CalibrateResult::FAILURE,
                               "Please rest the accelerometer on a flat surface"
                               " so that x- and y-axis will be the same");
    }

    zeroG = (mean[0] + mean[1]) / 2; // take average of X and Y acceleration as the zero G value
    oneG = mean[2]; // use Z acceleration as one G value (due to gravity)
    return CalibrateResult::SUCCESS;
}

double null_rej = 5.0;
bool always_pick_something = false;
bool send_repeated_predictions = false;
int timeout = 100;

void setup()
{
    stream.useNormalizer(normalizeADXL335);
    stream.setLabelsForAllDimensions({"x", "y", "z"});
    useInputStream(stream);
    useOutputStream(oStream);
    //useStream(stream);

    calibrator.setCalibrateFunction(processAccelerometerData);
    // The elaborate version is:
    // CalibrateProcess cp("Resting", "Rest accelerometer on flat surface, w/ z-axis vertical.", restingDataCollected);
    // calibrator.addCalibrateProcess(cp);
    calibrator.addCalibrateProcess("Resting", "Rest accelerometer on flat surface, w/ z-axis vertical.", restingDataCollected);
    useCalibrator(calibrator);

    pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 3, false, true, true, false, false));
    pipeline.setClassifier(ANBC(false, !always_pick_something, null_rej)); // use scaling, use null rejection, null rejection parameter
    // null rejection parameter is multiplied by the standard deviation to determine
    // the rejection threshold. the higher the number, the looser the filter; the
    // lower the number, the tighter the filter.

    if (send_repeated_predictions)
        pipeline.addPostProcessingModule(ClassLabelTimeoutFilter(timeout));
    usePipeline(pipeline);

    registerTuneable(always_pick_something, "Always Pick Something",
        "Whether to always pick (predict) one of the classes of training data, "
        "even if it's not a very good match. If selected, 'Variability' will "
        "not be used.");
    registerTuneable(null_rej, 1.0, 25.0, "Variability",
         "How different from the training data a new gesture can be and "
         "still be considered the same gesture. The higher the number, the more "
         "different it can be.");
    registerTuneable(send_repeated_predictions, "Send Repeated Predictions",
        "Whether to send repeated predictions while a pose is being held. If "
        "not selected, predictions will only be sent on transition from one "
        "pose to another.");
    registerTuneable(timeout, 1, 3000,
        "Timeout",
        "How long (in milliseconds) to wait after recognizing a class before "
        "recognizing a different one. Only used if 'Send Repeated Predictions' "
        "is selected.");
}
