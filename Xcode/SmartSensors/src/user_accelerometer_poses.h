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
    stream.setLabelsForAllDimensions({"x", "y", "z"});
    useStream(stream);
    
    pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 3, false, true, true, false, false));
    pipeline.setClassifier(ANBC(false, true, 10.0)); // use scaling, use null rejection, null rejection parameter
    // null rejection parameter is multiplied by the standard deviation to determine
    // the rejection threshold. the higher the number, the looser the filter; the
    // lower the number, the tighter the filter.

    usePipeline(pipeline);
    
    useOStream(oStream);
}