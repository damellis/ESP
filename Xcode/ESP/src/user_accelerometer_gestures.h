#include <ESP.h>

ASCIISerialStream stream(0, 9600, 3);
GestureRecognitionPipeline pipeline;
Calibrator calibrator;
TcpOStream oStream("localhost", 5204, 3, "l", "r", " ");

double zeroG = 0, oneG = 0;

double processAccelerometerData(double input)
{
    return (input - zeroG) / (oneG - zeroG);
}

void restingDataCollected(const MatrixDouble& data)
{
    // take average of X and Y acceleration as the zero G value
    zeroG = (data.getMean()[0] + data.getMean()[1]) / 2;
    oneG = data.getMean()[2]; // use Z acceleration as one G value
}

int timeout = 500; // milliseconds
double null_rej = 0.4;

void setup()
{
    stream.setLabelsForAllDimensions({"x", "y", "z"});
    useInputStream(stream);
    useOutputStream(oStream);
    //useStream(stream);

    calibrator.setCalibrateFunction(processAccelerometerData);
    calibrator.addCalibrateProcess("Resting",
        "Rest accelerometer on flat surface.", restingDataCollected);
    useCalibrator(calibrator);

    pipeline.setClassifier(DTW(false, true, null_rej));
    pipeline.addPostProcessingModule(ClassLabelTimeoutFilter(timeout));
    usePipeline(pipeline);

    registerTuneable(null_rej, 0.1, 25.0, "Variability",
         "How different from the training data a new gesture can be and "
         "still be considered the same gesture. The higher the number, the "
         "more different it can be.");
    registerTuneable(timeout, 1, 3000,
        "Timeout",
        "How long (in milliseconds) to wait after recognizing a "
        "gesture before recognizing another one.");
}
