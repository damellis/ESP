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

vector<double> normalizeColor(vector<double> input) {
    if (input.size() == 4) {
        vector<double> output;
        output.push_back(input[0] / input[3]);
        output.push_back(input[1] / input[3]);
        output.push_back(input[2] / input[3]);
        return output;
    } else {
        return input;
    }
}

void setupInputStream(IStream& is) {
    //is.useUSBPort(0);
    //is.useAnalogPin(0);
    //is.useNormalizer(normalizeColor);
}

void setupPipeline(GestureRecognitionPipeline& pipeline) {
    pipeline.addPreProcessingModule(MovingAverageFilter(5, 2));
    pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 2, false, true, true, false, false));
    pipeline.setClassifier(ANBC());
}
