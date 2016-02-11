#include <SmartSensors.h>

// Normalize by dividing each dimension by the total magnitude.
// Also add the magnitude as an additional feature.
vector<double> normalize(vector<double> input) {
    double magnitude = 0.0;

    for (int i = 0; i < input.size(); i++) magnitude += (input[i] * input[i]);
    magnitude = sqrt(magnitude);
    for (int i = 0; i < input.size(); i++) input[i] /= magnitude;

    //input.push_back(magnitude);

    return input;
}

ASCIISerialStream stream(9600, 3);
GestureRecognitionPipeline pipeline;

void setup() {
    stream.useUSBPort(0);
    //stream.useNormalizer(normalize);
    useStream(stream);

    pipeline.addPreProcessingModule(MovingAverageFilter(5, 3));
    //pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 3, false, true, true, false, false));
    pipeline.setClassifier(ANBC(false, true, 10.0)); // don't use scaling; use null rejection
    usePipeline(pipeline);
}
