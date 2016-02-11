#include <SmartSensors.h>

uint32_t DIM = 2;
uint32_t kFFT_WindowSize = 512;
uint32_t kFFT_HopSize = 1;

AudioStream stream;
GestureRecognitionPipeline pipeline;

void setup() {
    useStream(stream);

    pipeline.addPreProcessingModule(
        LowPassFilter(0.1, 1, DIM, 800, 1.0 / kOfSoundStream_SamplingRate));
    pipeline.addFeatureExtractionModule(
        FFT(kFFT_WindowSize, kFFT_HopSize,
            DIM, FFT::RECTANGULAR_WINDOW, true, false));
    pipeline.setClassifier(ANBC());
    usePipeline(pipeline);
}
