/** @example user_speaker.cpp
 * Speaker identification.
 */
#include <ESP.h>
#include <MFCC.h>

constexpr uint32_t kDownsample = 5;
constexpr uint32_t kSampleRate = 44100 / 5;  // 8820
constexpr uint32_t kFftWindowSize = 256;    // 256 samples => 30 ms, frame size
constexpr uint32_t kFftHopSize = 128;       // 128 samples => 15 ms, hop size
constexpr uint32_t DIM = 1;

AudioStream stream(kDownsample);
GestureRecognitionPipeline pipeline;
MacOSKeyboardOStream o_stream(3, 'j', 'd', '\0');

int confidence = 30;

void setup() {
    stream.setLabelsForAllDimensions({"audio"});

    pipeline.addFeatureExtractionModule(
        FFT(kFftWindowSize, kFftHopSize,
            DIM, FFT::HAMMING_WINDOW, true, false));

    MFCC::Options options;
    options.sample_rate = kSampleRate;
    options.fft_size = kFftWindowSize / 2;
    options.start_freq = 300;
    options.end_freq = 3700;
    options.num_tri_filter = 26;
    options.num_cepstral_coeff = 12;
    options.lifter_param = 22;
    options.use_vad = true;
    options.noise_level = 5;

    pipeline.addFeatureExtractionModule(MFCC(options));

    pipeline.setClassifier(GMM(16, true, false, 1, 100, 0.001));

    // out of 50 prediction (50 * 15 ms => 750 ms), if the number of correct
    // predictions are larger than `confidence`, it's confirmed as the label
    pipeline.addPostProcessingModule(ClassLabelFilter(confidence, 50));

    auto update_confidence = [](int new_confidence) {
        ClassLabelFilter* filter =
            dynamic_cast<ClassLabelFilter*>(pipeline.getPostProcessingModule(0));
        filter->setMinimumCount(new_confidence);
    };

    registerTuneable(confidence, 0, 50,
                     "Confidence",
                     "The number of the same predictions before it's confirmed"
                     " as a correct prediction",
                     update_confidence);

    useInputStream(stream);
    usePipeline(pipeline);
    useLeaveOneOutScoring(false);
    setGUIBufferSize(kSampleRate);
}
