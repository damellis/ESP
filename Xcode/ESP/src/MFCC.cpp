#include "MFCC.h"

#include <algorithm>
#include <cmath>
#include <new>
#include <numeric>
#include <vector>

#if __APPLE__
#include <Accelerate/Accelerate.h>
#elif __linux__
#include <cblas.h>
#endif

namespace GRT {

using std::vector;

RegisterFeatureExtractionModule<MFCC> MFCC::registerModule("MFCC");

TriFilterBanks::TriFilterBanks() : initialized_(false) {
}

void TriFilterBanks::initialize(uint32_t num_filter, uint32_t filter_size) {
    num_filter_ = num_filter;
    filter_size_ = filter_size;
    filter_ = new double[num_filter_ * filter_size_];
    initialized_ = true;
}

void TriFilterBanks::setFilter(uint32_t idx, double left, double middle,
                               double right, uint32_t fs) {
    uint32_t size = filter_size_;
    double unit = 1.0f * fs / 2 / (size - 1);
    for (uint32_t i = 0; i < size; i++) {
        double f = unit * i;
        uint32_t ni = i + idx * filter_size_;
        if (f <= left) {
            filter_[ni] = 0;
        } else if (left < f && f <= middle) {
            filter_[ni] = 1.0f * (f - left) / (middle - left);
        } else if (middle < f && f <= right) {
            filter_[ni] = 1.0f * (right - f) / (right - middle);
        } else if (right < f) {
            filter_[ni] = 0;
        } else {
            assert(false &&
                   "TriFilterBanks argument wrong or implementation bug");
        }
    }
}

TriFilterBanks::~TriFilterBanks() {
    if (initialized_) {
        delete filter_;
    }
}

void TriFilterBanks::filter(const vector<double>& input,
                            vector<double>& output) {
    assert(input.size() == filter_size_ &&
           "Dimension mismatch in TriFilterBanks filter");

    // Perform matrix multiplication
    cblas_dgemv(CblasRowMajor, CblasNoTrans, num_filter_, filter_size_, 1.0,
                filter_, filter_size_, input.data(), 1, 1.0, output.data(), 1);
}

MFCC::MFCC(Options options) : initialized_(false), options_(options) {
    classType = "MFCC";
    featureExtractionType = classType;
    debugLog.setProceedingText("[INFO MFCC]");
    debugLog.setProceedingText("[DEBUG MFCC]");
    errorLog.setProceedingText("[ERROR MFCC]");
    warningLog.setProceedingText("[WARNING MFCC]");

    if (options == Options()) { // Default values
        return;
    }

    initialize();
}

void MFCC::initialize() {
    numInputDimensions = options_.fft_size;
    numOutputDimensions = options_.num_cepstral_coeff;

    //---------------------------------------------
    //  Prepare the tribank filter
    //---------------------------------------------
    filters_.initialize(options_.num_tri_filter, options_.fft_size);

    vector<double> freqs(options_.num_tri_filter + 2);
    double mel_start = TriFilterBanks::toMelScale(options_.start_freq);
    double mel_end = TriFilterBanks::toMelScale(options_.end_freq);
    double mel_step = (mel_end - mel_start) / (options_.num_tri_filter + 1);

    for (uint32_t i = 0; i < options_.num_tri_filter + 2; i++) {
        freqs[i] = TriFilterBanks::fromMelScale(mel_start + i * mel_step);
    }

    for (uint32_t i = 0; i < options_.num_tri_filter; i++) {
        filters_.setFilter(i, freqs[i], freqs[i + 1], freqs[i + 2],
                           options_.sample_rate);
    }

    //--------------------------------------------------------------------------
    //  Prepare the dct matrix
    //
    //   [ num_cepstral_coeff rows * options_.num_tri_filter columns ]
    //
    //--------------------------------------------------------------------------
    uint32_t row = options_.num_cepstral_coeff;
    uint32_t col = options_.num_tri_filter;
    dct_matrix_ = new double[row * col];
    for (uint32_t i = 0; i < row; i++) {
        for (uint32_t j = 0; j < col; j++) {
            // In the matlab reference implementation, it's using (j - 0.5),
            // that's because j is 1:M not 0:(M-1). In C++, we use (j + 0.5).
            dct_matrix_[i * col + j] =
                sqrt(2.0 / col) * cos(PI * i / col * (j + 0.5));
        }
    }

    // Vector allocation
    tmp_lfbe_.resize(options_.num_tri_filter);
    tmp_cc_.resize(options_.num_cepstral_coeff);

    initialized_ = true;
}

MFCC::MFCC(const MFCC& rhs) {
    classType = rhs.getClassType();
    featureExtractionType = classType;
    debugLog.setProceedingText("[DEBUG MFCC]");
    errorLog.setProceedingText("[ERROR MFCC]");
    warningLog.setProceedingText("[WARNING MFCC]");

    this->options_ = rhs.options_;
    *this = rhs;
}

MFCC& MFCC::operator=(const MFCC& rhs) {
    if (this != &rhs) {
        this->classType = rhs.getClassType();
        this->options_ = rhs.getOptions();
        this->initialize();
        copyBaseVariables((FeatureExtraction*)&rhs);
    }
    return *this;
}

bool MFCC::deepCopyFrom(const FeatureExtraction* featureExtraction) {
    if (featureExtraction == nullptr) {
        return false;
    }

    if (this->getFeatureExtractionType() ==
        featureExtraction->getFeatureExtractionType()) {
        // Invoke the equals operator to copy the data from the rhs instance to
        // this instance
        *this = *(MFCC*)featureExtraction;
        return true;
    }

    errorLog << "clone(MFCC *featureExtraction)"
             << "-  FeatureExtraction Types Do Not Match!" << std::endl;
    return false;
}

void MFCC::computeLFBE(const vector<double>& fft, vector<double>& lfbe) {
    assert(lfbe.size() == options_.num_tri_filter &&
           "Dimension mismatch for LFBE computation");

    uint32_t M = options_.num_tri_filter;
    filters_.filter(fft, lfbe);

    for (uint32_t i = 0; i < M; i++) {
        if (lfbe[i] != 0) {
            lfbe[i] = log(lfbe[i]);
        }
    }
}

void MFCC::computeCC(const vector<double>& lfbe, vector<double>& cc) {
    cblas_dgemv(CblasRowMajor, CblasNoTrans, options_.num_cepstral_coeff,
                options_.num_tri_filter, 1.0, dct_matrix_,
                options_.num_tri_filter, lfbe.data(), 1, 1.0, cc.data(), 1);
}

vector<double> MFCC::getCC(const vector<double>& lfbe) {
    uint32_t M = options_.num_tri_filter;

    vector<double> cc(options_.num_cepstral_coeff);
    for (uint32_t i = 0; i < options_.num_cepstral_coeff; i++) {
        for (uint32_t j = 0; j < M; j++) {
            // [1] j is 1:M not 0:(M-1), so we change (j - 0.5) to (j + 0.5)
            cc[i] += sqrt(2.0 / M) * lfbe[j] * cos(PI * i / M * (j + 0.5));
        }
    }
    return cc;
}

vector<double> MFCC::lifterCC(const vector<double>& cc) {
    vector<double> liftered(options_.num_cepstral_coeff);
    uint32_t L = options_.lifter_param;
    for (uint32_t i = 0; i < options_.num_cepstral_coeff; i++) {
        liftered[i] = (1 + 1.0f * L / 2 * sin(PI * i / L)) * cc[i];
    }
    return liftered;
}

bool MFCC::computeFeatures(const VectorDouble& inputVector) {
    // The assumed input data is FFT value. We check VAD, if too small (somewhat
    // meaning it's background noise), we return true (data has been processed)
    // but set `featureDataReady` as false. Here it's a super naive VAD: the
    // voice amplitude, or the FFT energy.
    if (options_.use_vad) {
        double sum =
            std::accumulate(inputVector.begin(), inputVector.end(), 0.0);
        if (sum < options_.noise_level) {
            featureDataReady = false;
            return true;
        }
    }

    // Clear the memory (if not, garbage memory will cause us issue. This should
    // be faster than allocating a vector every time (maybe?)
    std::fill(tmp_lfbe_.begin(), tmp_lfbe_.end(), 0);
    std::fill(tmp_cc_.begin(), tmp_cc_.end(), 0);

    // We assume the input is from a DFT (FFT) transformation.
    computeLFBE(inputVector, tmp_lfbe_);
    computeCC(tmp_lfbe_, tmp_cc_);
    featureVector = lifterCC(tmp_cc_);
    featureDataReady = true;
    return true;
}

bool MFCC::saveModelToFile(string filename) const{
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);

    return saveModelToFile(file);
}

bool MFCC::loadModelFromFile(string filename) {
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);

    return loadModelFromFile(file);
}

bool MFCC::saveModelToFile(fstream &file) const {
    if (!file.is_open()){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }

    // Write the file header
    file << "GRT_FFT_FEATURES_FILE_V1.0" << endl;

    // Save the base settings to the file
    if (!saveFeatureExtractionSettingsToFile(file)) {
        errorLog << "saveFeatureExtractionSettingsToFile(fstream &file)"
                 << " - Failed to save base feature extraction settings to file!"
                 << std::endl;
        return false;
    }

    // Write the MFCC Options
    file << "SampleRate: " << options_.sample_rate << std::endl;
    file << "FFTSize: " << options_.fft_size << std::endl;
    file << "StartFrequency: " << options_.start_freq << std::endl;
    file << "EndFrequency: " << options_.end_freq << std::endl;
    file << "NumTriFilter: " << options_.num_tri_filter << std::endl;
    file << "LifterParam: " << options_.lifter_param << std::endl;
    file << "UseVad: " << options_.use_vad << std::endl;
    file << "NoiseLevel: " << options_.noise_level << std::endl;

    return true;
}

bool MFCC::loadModelFromFile(fstream &file) {
    if (!file.is_open()) {
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!"
                 << std::endl;
        return false;
    }

    string word;

    // Load the header
    file >> word;
    if (word != "GRT_FFT_FEATURES_FILE_V1.0") {
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!"
                 << std::endl;
        return false;
    }

    if (!loadFeatureExtractionSettingsFromFile(file)) {
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) "
                 << "- Failed to load base feature extraction settings from file!"
                 << std::endl;
        return false;
    }

    // Load the Sample Rate
    file >> word;
    if( word != "SampleRate:" ){
        errorLog << "loadModelFromFile(fstream &file) "
                 << "- Failed to read SampleRate header!" << std::endl;
        return false;
    }
    file >> options_.sample_rate;;

    // Load the FFT Size
    file >> word;
    if( word != "FFTSize:" ){
        errorLog << "loadModelFromFile(fstream &file) "
                 << "- Failed to read FFTSize header!" << std::endl;
        return false;
    }
    file >> options_.fft_size;

    // Load the Start Frequency
    file >> word;
    if( word != "StartFrequency:" ){
        errorLog << "loadModelFromFile(fstream &file) "
                 << "- Failed to read StartFrequency header!" << std::endl;
        return false;
    }
    file >> options_.start_freq;

    // Load the End Frequency
    file >> word;
    if( word != "EndFrequency:" ){
        errorLog << "loadModelFromFile(fstream &file) "
                 << "- Failed to read EndFrequency header!" << std::endl;
        return false;
    }
    file >> options_.end_freq;

    // Load the Num Tribank Filter
    file >> word;
    if( word != "NumTriFilter:" ){
        errorLog << "loadModelFromFile(fstream &file) "
                 << "- Failed to read NumTriFilter header!" << std::endl;
        return false;
    }
    file >> options_.num_tri_filter;

    // Load the Lifter Param
    file >> word;
    if( word != "LifterParam:" ){
        errorLog << "loadModelFromFile(fstream &file) "
                 << "- Failed to read LifterParam header!" << std::endl;
        return false;
    }
    file >> options_.lifter_param;

    // Load the Use VAD
    file >> word;
    if( word != "UseVad:" ){
        errorLog << "loadModelFromFile(fstream &file) "
                 << "- Failed to read UseVad header!" << std::endl;
        return false;
    }
    file >> options_.use_vad;

    // Load the Noise Level
    file >> word;
    if( word != "NoiseLevel:" ){
        errorLog << "loadModelFromFile(fstream &file) "
                 << "- Failed to read NoiseLevel header!" << std::endl;
        return false;
    }
    file >> options_.use_vad;

    initialize();
    return true;
}


bool MFCC::reset() {
    return true;
}

}  // namespace GRT
