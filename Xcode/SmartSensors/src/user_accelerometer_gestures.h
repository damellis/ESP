#include <SmartSensors.h>

ASCIISerialStream stream(9600, 3);
GestureRecognitionPipeline pipeline;

void setup()
{
    stream.useUSBPort(0);
    useStream(stream);
    
    pipeline.setClassifier(DTW(false, true, 0.5)); // disable scaling, enable null rejection w/ threshold of 0.5
    pipeline.addPostProcessingModule(ClassLabelTimeoutFilter(500));
    usePipeline(pipeline);
}