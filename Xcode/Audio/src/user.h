#include "ofxGrt.h"

using namespace GRT;

GestureRecognitionPipeline setupPipeline() {
    GestureRecognitionPipeline pipeline;

    pipeline.addPreProcessingModule(MovingAverageFilter(5, 1));
    pipeline.addFeatureExtractionModule(
        FFT(512, 1, 1, FFT::RECTANGULAR_WINDOW, true, false));
    pipeline.addFeatureExtractionModule(FFTFeatures(256));
    pipeline.setClassifier(ANBC());

    return pipeline;
}
