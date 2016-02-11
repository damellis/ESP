#include <SmartSensors.h>

ASCIISerialStream stream(9600, 3);
GestureRecognitionPipeline pipeline;

void setup()
{
    stream.useUSBPort(0);
    useStream(stream);
    
    pipeline.setClassifier(DTW(false, true, 3.0)); // disable scaling, enable null rejection w/ threshold of 3.0
    pipeline.addPostProcessingModule(ClassLabelTimeoutFilter());
    usePipeline(pipeline);
}