#include "GRT/GRT.h"
#include "istream.h"

using namespace GRT;

ASCIISerialStream stream(115200, 3);
GestureRecognitionPipeline pipeline;

void setup()
{
    stream.useUSBPort(0);
    useStream(stream);
    
    pipeline.addPreProcessingModule(MovingAverageFilter(5, 3));
    pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 3, false, true, true, false, false));
    pipeline.setClassifier(ANBC(false, true, 10.0));
    pipeline.addPostProcessingModule(ClassLabelFilter(3, 5));
    usePipeline(pipeline);
}