#include <SmartSensors.h>

AudioStream stream(10);
GestureRecognitionPipeline pipeline;
MacOSKeyboardOStream o_stream(3, 'j', 'd', '\0');

// Audio defaults to 44.1k sampling rate. With a downsample of 10, it's 4.41k.
uint32_t kFFT_WindowSize = 512;
uint32_t kFFT_HopSize = 128;
uint32_t DIM = 1;

void setup() {
    stream.setLabelsForAllDimensions({"audio"});

    pipeline.addFeatureExtractionModule(
        FFT(kFFT_WindowSize, kFFT_HopSize,
            DIM, FFT::RECTANGULAR_WINDOW, true, false));

    pipeline.setClassifier(
        SVM(SVM::LINEAR_KERNEL, SVM::C_SVC, true, true));

    pipeline.addPostProcessingModule(ClassLabelFilter(35, 50));

    useStream(stream);
    usePipeline(pipeline);
    useOStream(o_stream);
}
