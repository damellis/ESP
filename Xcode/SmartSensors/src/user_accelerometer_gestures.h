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

void setup()
{
    stream.useNormalizer(normalizeADXL335);
    useStream(stream);
    
    pipeline.setClassifier(DTW(false, true, 0.35)); // use scaling, use null rejection, null rejection parameter
    // null rejection parameter is multiplied by the standard deviation to determine
    // the rejection threshold. the higher the number, the looser the filter; the
    // lower the number, the tighter the filter.
    
    // We don't use a ClassLabelTimeoutFilter because it doesn't work
    // properly when replaying saved sensor data (which is stored without
    // timestamps). Instead, filter by number of samples using a
    // ClassLabelFilter.
    //pipeline.addPostProcessingModule(ClassLabelTimeoutFilter(500));
    pipeline.addPostProcessingModule(ClassLabelFilter(1, 25));
    usePipeline(pipeline);
    
    useOStream(oStream);
}