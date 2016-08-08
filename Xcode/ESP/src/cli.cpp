#include "MFCC.h"
#include "matplotlibcpp.h"
#include "training-data-manager.h"
#include <GRT/GRT.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <vector>

namespace plt = matplotlibcpp;

constexpr uint32_t kNumMaxLabels = 9;
constexpr char kTrainingDataFilename[] = "TrainingData.grt";
constexpr char kTestDataFilename[] = "TestData.grt";
constexpr char kPipelineFilename[] = "Pipeline.grt";
constexpr uint32_t downsample_rate = 5;
constexpr uint32_t sample_rate = 44100 / downsample_rate;
// frame duration = 23 ms (512 samples)
constexpr uint32_t kFFT_WindowSize = 256;
constexpr uint32_t kFFT_HopSize = 128;
constexpr uint32_t DIM = 1;

int main(int argc, char* argv[]) {
    bool draw_sample = false;
    bool load_pipeline = false;
    char c;
    opterr = 0;
    while ((c = getopt(argc, argv, "dl")) != -1) {
        switch (c) {
            case 'd': draw_sample = true; break;
            case 'l': load_pipeline = true; break;
            default: abort();
        }
    }

    // Setup Pipeline
    // TODO(benzh) Convert this to user code loading
    GRT::GestureRecognitionPipeline pipeline;

    pipeline.addFeatureExtractionModule(
        GRT::FFT(kFFT_WindowSize, kFFT_HopSize,
                 DIM, GRT::FFT::HAMMING_WINDOW, true, false));

    GRT::MFCC::Options options;
    options.sample_rate = sample_rate;
    options.fft_size = kFFT_WindowSize / 2;
    options.start_freq = 300;
    options.end_freq = 8000;
    options.num_tri_filter = 26;
    options.num_cepstral_coeff = 12;
    options.lifter_param = 22;
    options.use_vad = true;
    options.noise_level = 5;

    pipeline.addFeatureExtractionModule(GRT::MFCC(options));
    pipeline.setClassifier(GRT::GMM(16, true, false, 1, 100, 0.001));
    pipeline.addPostProcessingModule(GRT::ClassLabelFilter(25, 40));

    TrainingDataManager training_data_manager(kNumMaxLabels);

    if (!load_pipeline) {
        // We load the training data and train the model
        if (training_data_manager.load(kTrainingDataFilename)) {
            auto d = training_data_manager.getSample(1, 2);

            if (draw_sample) {
                plt::plot(training_data_manager.getSample(1, 2).getColVector(0));
                plt::save("./sample.png");
            }

            if (pipeline.train(training_data_manager.getAllData())) {
                std::cout << "Training Successful" << std::endl;;
                pipeline.save(kPipelineFilename);
            } else {
                std::cout << "Failed to train the model" << std::endl;
                return -1;
            }
        }
    } else {
        pipeline.load(kPipelineFilename);
    }

    GRT::MatrixDouble test_data;
    if (!test_data.load(kTestDataFilename)) {
        std::cout << "Failed to load test data from " << kTestDataFilename
                  << std::endl;
        return -1;
    }

    size_t vec_size = test_data.getNumRows();
    std::vector<double> x(vec_size, 0);
    std::vector<double> preds(vec_size, 0);
    std::vector<double> audio(vec_size, 0);
    std::vector<double> distances1(vec_size, 0);
    std::vector<double> distances2(vec_size, 0);

    std::cout << "Running testing" << std::endl;

    // Run test data through
    for (uint32_t i = 0; i < test_data.getNumRows(); i++) {
        auto td = test_data.getRowVector(i);
        pipeline.predict(td);
        uint32_t predicted_label = pipeline.getPredictedClassLabel();
        auto ds = pipeline.getClassDistances();

        audio[i] = td[0];
        x[i] = i;
        preds[i] = static_cast<double>(predicted_label);
        distances1[i] = ds[0];
        distances2[i] = ds[1];
    }

    // std::cout << "Test finished, plotting ... ";
    // plt::plot(x, audio, "b-");
    // plt::save("./audio.png");

    plt::plot(x, distances1, "r.", x, distances2, "g.");
    plt::save("./prediction.png");
    std::cout << "Done" << std::endl;

    return 0;
}
