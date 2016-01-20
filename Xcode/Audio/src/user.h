#include "istream.h"
#include "ofxGrt.h"

using namespace GRT;

void setupInputStream(IStream& is) {
    is.useUSBPort(0);
    is.useAnalogPin(0);
    is.useNormalizer([](int i) { return (i - 86) * 1.0 / 172; });
}

void setupPipeline(GestureRecognitionPipeline& pipeline) {
    pipeline.addPreProcessingModule(MovingAverageFilter(5, 1));
    pipeline.addFeatureExtractionModule(
        FFT(512, 512, 1, FFT::RECTANGULAR_WINDOW, true, false));
    pipeline.addFeatureExtractionModule(FFTFeatures(256));
    pipeline.setClassifier(ANBC());
}
