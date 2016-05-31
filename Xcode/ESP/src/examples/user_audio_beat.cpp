/** @example user_audio_beat.cpp
 * Audio beat detection examples.
 */
#include <ESP.h>

// SerialStream stream(0, 115200);   // 5k sampling rate
AudioStream stream(8);
GestureRecognitionPipeline pipeline;
MacOSKeyboardOStream o_stream(3, 'j', 'd', '\0');

// Audio defaults to 44.1k sampling rate. With a downsample of 10, it's 4.41k.
uint32_t kFFT_WindowSize = 512;
uint32_t kFFT_HopSize = 128;
uint32_t DIM = 1;

double bias = 0;
double range = 0;

CalibrateResult backgroundCollected(const MatrixDouble& data) {
    // For audio, it's one dimension matrix
    vector<double> means = data.getMean();
    bias = means[0];
    return CalibrateResult::SUCCESS;
}

CalibrateResult shoutCollected(const MatrixDouble& data) {
    // For audio, it's one dimension matrix
    double min = data.getMinValue();
    double max = data.getMaxValue();
    range = std::max(max - bias, bias - min);
    return CalibrateResult::SUCCESS;
}

double analogReadToSound(double data) {
    // Audio data is one dimension
    return (data - bias) / range;
}

Calibrator calibrator(analogReadToSound);

void setup() {
    stream.setLabelsForAllDimensions({"audio"});

    pipeline.addFeatureExtractionModule(
        FFT(kFFT_WindowSize, kFFT_HopSize,
            DIM, FFT::RECTANGULAR_WINDOW, true, false));

    pipeline.setClassifier(
        SVM(SVM::LINEAR_KERNEL, SVM::C_SVC, true, true));

    pipeline.addPostProcessingModule(ClassLabelFilter(25, 40));

    calibrator
            .addCalibrateProcess("Bias", "Remain silent", backgroundCollected)
            .addCalibrateProcess("Range", "Shout as much as possible", shoutCollected);

    useInputStream(stream);
    useCalibrator(calibrator);
    usePipeline(pipeline);
    // useOutputStream(o_stream);
}
