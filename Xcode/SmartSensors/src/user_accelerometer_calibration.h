#include <SmartSensors.h>

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
double threshold = 0.4;

void setup()
{
    stream.setLabelsForAllDimensions({"x", "y", "z"});
    useStream(stream);

    calibrator.setCalibrateFunction(processAccelerometerData);
    calibrator.addCalibrateProcess("Resting",
        "Rest accelerometer on flat surface.", restingDataCollected);
    useCalibrator(calibrator);

    pipeline.setClassifier(DTW(false, true, threshold));
    pipeline.addPostProcessingModule(ClassLabelTimeoutFilter(timeout));
    usePipeline(pipeline);

    registerTuneable(threshold, 0.1, 3.0,
        "Similarity",
        "How similar a live gesture needs to be to a training sample. "
        "The lower the number, the more similar it needs to be.");
    registerTuneable(timeout, 1, 1000,
        "Timeout",
        "How long (in milliseconds) to wait after recognizing a "
        "gesture before recognizing another one.");

    useOStream(oStream);
}
