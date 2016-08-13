/** @example user_speaker.cpp
 * Speaker identification with MFCC and GMM.
 */
#include <ESP.h>
#include <MFCC.h>

constexpr uint32_t kDownsample = 5;
constexpr uint32_t kSampleRate = 44100 / 5;  // 8820
constexpr uint32_t kFftWindowSize = 256;     // 256 samples => 30 ms, frame size
constexpr uint32_t kFftHopSize = 128;        // 128 samples => 15 ms, hop size
constexpr uint32_t DIM = 1;

AudioStream stream(kDownsample);
GestureRecognitionPipeline pipeline;
TcpOStream oStream("localhost", 5204);

// Tuneable parameters
int post_duration  = 1000;   // ms
double post_ratio  = 0.7f;   // 70%
double noise_level = 5.0f;   // Noise level, the unit is not yet standardized)

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
    options.noise_level = noise_level;

    pipeline.addFeatureExtractionModule(MFCC(options));

    pipeline.setClassifier(SVM());
    // GMM(16, true, false, 1, 100, 0.001));

    // In post processing, we wait #n predicitons. If m out of n predictions are
    // from the same class, we declare the class as the right one.
    //
    // n = (duration * sample_rate) / frame_size
    //   where duration    = post_duration
    //         sample_rate = kSampleRate
    //         frame_size  = kFftHopSize
    // m = n * post_ratio
    int num_predictions = post_duration / 1000 * kSampleRate / kFftHopSize;
    pipeline.addPostProcessingModule(
            ClassLabelFilter(num_predictions * post_ratio, num_predictions));

    auto ratio_updater = [](double new_ratio) {
        ClassLabelFilter* filter =
            dynamic_cast<ClassLabelFilter*>(pipeline.getPostProcessingModule(0));
        // Recalculate num_predictions as post_duration might have been changed
        int num_predictions = post_duration / 1000 * kSampleRate / kFftHopSize;
        filter->setMinimumCount(new_ratio * num_predictions);
    };

    auto duration_updater = [](int new_duration) {
        ClassLabelFilter* filter =
            dynamic_cast<ClassLabelFilter*>(pipeline.getPostProcessingModule(0));
        // Recalculate num_predictions as post_duration might have been changed
        int num_predictions = post_duration / 1000 * kSampleRate / kFftHopSize;
        filter->setBufferSize(num_predictions);
    };

    auto noise_updater = [](int new_noise_level) {
        MFCC *mfcc = dynamic_cast<MFCC*>(pipeline.getFeatureExtractionModule(1));
        mfcc->setNoiseLevel(new_noise_level);
    };

    registerTuneable(noise_level, 0, 20,
                     "Noise Level",
                     "The threshold for the system to distinguish between "
                     "ambient noise and speech/sound",
                     noise_updater);

    registerTuneable(post_duration, 0, 2000,
                     "Duration",
                     "Time (in ms) that is considered as a whole "
                     "for smoothing the prediction",
                     duration_updater);

    registerTuneable(post_ratio, 0.0f, 1.0f,
                     "Ratio",
                     "The portion of time in duration that "
                     "should be from the same class",
                     ratio_updater);

    useInputStream(stream);
    useOutputStream(oStream);
    usePipeline(pipeline);
    useLeaveOneOutScoring(false);
    setGUIBufferSize(kSampleRate);
}
