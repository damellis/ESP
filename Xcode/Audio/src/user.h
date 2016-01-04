#include "ofxGrt.h"

using namespace GRT;

GestureRecognitionPipeline setupPipeline() {
    GestureRecognitionPipeline pipeline;

    // pipeline.addPreProcessingModule(MovingAverageFilter(5, 1));
    // pipeline.addFeatureExtractionModule(
    //         FFT(512, 1, 1, FFT::RECTANGULAR_WINDOW, true, false));
    // pipeline.addFeatureExtractionModule(FFTFeatures(256));
    // pipeline.setClassifier(ANBC());

    DTW dtw;
    dtw.enableTrimTrainingData(true, 0.1, 90);
    pipeline.setClassifier(dtw);

    return pipeline;
}
