#include <SmartSensors.h>

uint32_t DIM = 1;
// for 44.1k sampling rate, 0.1-second will yield 4410 samples
// For a hopsize of 128, this creates 34.4 features.
// We use a post processing of ClassLabelFilter with 50 buffers, 35 occurrance.
uint32_t kFFT_WindowSize = 512;
uint32_t kFFT_HopSize = 128;

AudioStream stream;
GestureRecognitionPipeline pipeline;
MacOSKeyboardOStream o_stream(3, '\0', 'a', 'd');

void setup() {
    pipeline.addFeatureExtractionModule(
        FFT(kFFT_WindowSize, kFFT_HopSize,
            DIM, FFT::RECTANGULAR_WINDOW, true, false));

    RandomForests forest;
    forest.setForestSize(10);
    forest.setNumRandomSplits(100);
    forest.setMaxDepth(10);
    forest.setMinNumSamplesPerNode(10);
    pipeline.setClassifier(forest);

    pipeline.addPostProcessingModule(ClassLabelFilter(35, 50));

    useStream(stream);
    usePipeline(pipeline);
    useOStream(o_stream);
}
