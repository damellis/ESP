#include "ofxGrt.h"

using namespace GRT;

GestureRecognitionPipeline setupPipeline() {
    GestureRecognitionPipeline pipeline;

    pipeline.addPreProcessingModule(MovingAverageFilter(5, 1));
    pipeline.addFeatureExtractionModule(FFT(512));
    pipeline.setClassifier(SVM());

    return pipeline;
}
