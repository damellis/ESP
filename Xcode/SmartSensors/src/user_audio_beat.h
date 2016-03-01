#include <SmartSensors.h>

uint32_t DIM = 2;
uint32_t kFFT_WindowSize = 512;
uint32_t kFFT_HopSize = 128;

AudioStream stream;
GestureRecognitionPipeline pipeline;

void setup() {
    useStream(stream);

    pipeline.addFeatureExtractionModule(
        FFT(kFFT_WindowSize, kFFT_HopSize,
            DIM, FFT::RECTANGULAR_WINDOW, true, false));

    RandomForests forest;
    forest.setForestSize(10);
    forest.setNumRandomSplits(100);
    forest.setMaxDepth(10);
    forest.setMinNumSamplesPerNode(10);
    pipeline.setClassifier(forest);

    usePipeline(pipeline);
}
