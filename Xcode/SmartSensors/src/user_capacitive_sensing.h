#include <SmartSensors.h>

ASCIISerialStream stream(0, 9600, 12);
GestureRecognitionPipeline pipeline;

void setup() {
    useStream(stream);

    //pipeline.addPreProcessingModule(MovingAverageFilter(5, 3));
    //pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 3, false, true, true, false, false));
    //pipeline.addPreProcessingModule(Derivative(Derivative::FIRST_DERIVATIVE, 0.1, 12));
    pipeline.setClassifier(ANBC(false, true, 10.0)); // use scaling, use null rejection, null rejection parameter
    // null rejection parameter is multiplied by the standard deviation to determine
    // the rejection threshold. the higher the number, the looser the filter; the
    // lower the number, the tighter the filter.    
    
    usePipeline(pipeline);
}
