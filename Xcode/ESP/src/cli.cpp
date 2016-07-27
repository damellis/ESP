#include "MFCC.h"
#include "matplotlibcpp.h"
#include "training-data-manager.h"
#include <GRT/GRT.h>
#include <assert.h>
#include <vector>

using namespace GRT;

constexpr uint32_t kNumMaxLabels = 9;
constexpr char kTrainingDataFilename[] = "TrainingData.grt";
constexpr char kTestDataFilename[] = "TrainingData.grt";
constexpr uint32_t downsample_rate = 2;
constexpr uint32_t sample_rate = 44100 / downsample_rate;
// frame duration = 23 ms (512 samples)
constexpr uint32_t kFFT_WindowSize = 512;
constexpr uint32_t kFFT_HopSize = 256;
constexpr uint32_t DIM = 1;

int main() {
    // Setup Pipeline
    // TODO(benzh) Convert this to user code loading
    GestureRecognitionPipeline pipeline;

    pipeline.addFeatureExtractionModule(
        FFT(kFFT_WindowSize, kFFT_HopSize,
            DIM, FFT::HAMMING_WINDOW, true, false));

    MFCC::Options options;
    options.sample_rate = sample_rate;
    options.fft_size = kFFT_WindowSize / 2;
    options.start_freq = 300;
    options.end_freq = 8000;
    options.num_tri_filter = 26;
    options.num_cepstral_coeff = 12;
    options.lifter_param = 22;
    options.use_vad = true;
    options.noise_level = 30;

    pipeline.addFeatureExtractionModule(MFCC(options));

    pipeline.setClassifier(GMM(16, true, false, 1, 100, 0.001));

    pipeline.addPostProcessingModule(ClassLabelFilter(25, 40));

    TrainingDataManager training_data_manager(kNumMaxLabels);

    if (training_data_manager.load(kTrainingDataFilename)) {
        if (pipeline.train(training_data_manager.getAllData())) {
            std::cout << "Training Successful" << std::endl;;
        } else {
            std::cout << "Failed to train the model" << std::endl;
            return -1;
        }
    }

    GRT::MatrixDouble test_data;
    std::vector<double> x;
    std::vector<double> predicted_labels;
    if (test_data.load(kTestDataFilename)) {
        // Run test data through
        for (uint32_t i = 0; i < test_data.getNumRows(); i++) {
            pipeline.predict(test_data.getRowVector(i));
            uint32_t predicted_label = pipeline.getPredictedClassLabel();
            x.push_back(i);
            predicted_labels.push_back(static_cast<double>(predicted_label));
        }
    }

    // Plotting to reason about the result
    namespace plt = matplotlibcpp;
    plt::plot(x, predicted_labels);
    plt::show();

    return 0;
}
