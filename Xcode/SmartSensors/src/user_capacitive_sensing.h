#include <SmartSensors.h>

ASCIISerialStream stream(9600, 12);
GestureRecognitionPipeline pipeline;

void setup() {
    stream.useUSBPort(0);
    useStream(stream);

    //pipeline.addPreProcessingModule(MovingAverageFilter(5, 3));
    //pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 3, false, true, true, false, false));
    //pipeline.addPreProcessingModule(Derivative(Derivative::FIRST_DERIVATIVE, 0.1, 12));
    pipeline.setClassifier(ANBC(false, true, 10.0)); // don't use scaling; use null rejection
    usePipeline(pipeline);
}
