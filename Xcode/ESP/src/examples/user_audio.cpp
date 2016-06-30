/** @example user_audio.cpp
 * Audio beat detection examples.
 */
#include <ESP.h>
#include <MFCC.h>

constexpr uint32_t downsample_rate = 5;
constexpr uint32_t sample_rate = 44100 / 5;  // 8820
constexpr uint32_t kFFT_WindowSize = 256;    // 256 samples => 30 ms, frame size
uint32_t kFFT_HopSize = 128;
uint32_t DIM = 1;

AudioStream stream(downsample_rate);
GestureRecognitionPipeline pipeline;
MacOSKeyboardOStream o_stream(3, 'j', 'd', '\0');

double bias = 0;
double range = 0;

CalibrateResult backgroundCollected(const MatrixDouble& data) {
    // For audio, it's one dimension matrix
    vector<double> means = data.getMean();
    bias = means[0];
    if (bias > 0.5) {
        return CalibrateResult(CalibrateResult::FAILURE,
                               "You should remain silent while collecting data");
    } else {
        return CalibrateResult::SUCCESS;
    }
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
            DIM, FFT::HAMMING_WINDOW, true, false));

    pipeline.addFeatureExtractionModule(
        MFCC(sample_rate, kFFT_WindowSize / 2, 300, 8000, 26, 12, 22));

    pipeline.setClassifier(
        SVM(SVM::LINEAR_KERNEL, SVM::C_SVC, true, true));

    pipeline.addPostProcessingModule(ClassLabelFilter(25, 40));

    calibrator
            .addCalibrateProcess("Bias", "Remain silent", backgroundCollected)
            .addCalibrateProcess("Range", "Shout as much as possible", shoutCollected);

    useInputStream(stream);
    // useCalibrator(calibrator);
    usePipeline(pipeline);
    // useOutputStream(o_stream);
}
