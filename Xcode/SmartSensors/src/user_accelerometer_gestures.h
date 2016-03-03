#include <SmartSensors.h>

ASCIISerialStream stream(9600, 3);
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
    stream.useUSBPort(0);
    stream.useNormalizer(normalizeADXL335);
    //stream.useNormalizer(normalizeArduino101);
    useStream(stream);
    
    pipeline.setClassifier(DTW(true, true, 0.5)); // enable scaling, enable null rejection w/ specified threshold
    pipeline.addPostProcessingModule(ClassLabelTimeoutFilter(500));
    usePipeline(pipeline);
    
    useOStream(oStream);
}