#include "istream.h"
#include "ofxGrt.h"

using namespace GRT;

float analogInputToVoltage(float val) {
    return val / 1024.0 * 5.0;
}

float voltageToAcceleration(float val) {
    return (val - 1.65) / 1.65 * 3.0;
}

float analogInputToAcceleration(float val) {
    return voltageToAcceleration(analogInputToVoltage(val));
}

void setupInputStream(IStream& is) {
    is.useUSBPort(0);
    is.useAnalogPin(0);
    is.useNormalizer(analogInputToAcceleration);
}

void setupPipeline(GestureRecognitionPipeline& pipeline) {
    pipeline.addPreProcessingModule(MovingAverageFilter(5, 1));
    pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 1, false, true, true, false, false));
    pipeline.setClassifier(ANBC());
}
