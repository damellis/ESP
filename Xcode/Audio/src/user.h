#include "GRT/GRT.h"
#include "istream.h"

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

ASCIISerialStream stream(9600);
GestureRecognitionPipeline pipeline;

void setup() {
    stream.useUSBPort(0);
    stream.useNormalizer(normalizeColor);
    useStream(stream);
    
    pipeline.addPreProcessingModule(MovingAverageFilter(5, 3));
    pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 3, false, true, true, false, false));
    pipeline.setClassifier(ANBC());
    usePipeline(pipeline);
}
