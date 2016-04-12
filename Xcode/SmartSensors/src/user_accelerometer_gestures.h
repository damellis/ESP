#include <SmartSensors.h>

ASCIISerialStream stream(0, 9600, 3);
GestureRecognitionPipeline pipeline;
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

int timeout = 500;
double threshold = 0.4;

void setup()
{
    stream.useNormalizer(normalizeADXL335);
    stream.setLabelsForAllDimensions({"x", "y", "z"});
    useStream(stream);
    
    pipeline.setClassifier(DTW(false, true, threshold)); // don't use scaling, use null rejection, null rejection parameter
    // null rejection parameter is multiplied by the standard deviation to determine
    // the rejection threshold. the higher the number, the looser the filter; the
    // lower the number, the tighter the filter.
    
    // We don't use a ClassLabelTimeoutFilter because it doesn't work
    // properly when replaying saved sensor data (which is stored without
    // timestamps). Instead, filter by number of samples using a
    // ClassLabelFilter.
    pipeline.addPostProcessingModule(ClassLabelTimeoutFilter(timeout));
    //pipeline.addPostProcessingModule(ClassLabelFilter(1, 25));
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