#include "MFCC.h"

#include <cmath>

namespace GRT {
    
RegisterFeatureExtractionModule<MelBankFeatures>
MelBankFeatures::registerModule("MelBankFeatures");

MelBankFeatures::MelBankFeatures(double startFreq, double endFreq,
                                 uint32_t FFTSize, uint32_t sampleRate,
                                 uint32_t numFilterBanks,
                                 uint32_t numLowerFeatures,
                                 bool withDelta)
        : initialized_(false), num_lower_features_(numLowerFeatures), with_delta_(withDelta) {
    classType = "MelBankFeatures";
    featureExtractionType = classType;
    debugLog.setProceedingText("[INFO MelBankFeatures]");
    debugLog.setProceedingText("[DEBUG MelBankFeatures]");
    errorLog.setProceedingText("[ERROR MelBankFeatures]");
    warningLog.setProceedingText("[WARNING MelBankFeatures]");

    if (numFilterBanks <= 0 ||
        startFreq <= 0 || endFreq <= 0 ||
        FFTSize <= 0 || sampleRate <= 0 ||
        numLowerFeatures > numFilterBanks) {
        return;
    }

    numInputDimensions = FFTSize;
    numOutputDimensions = numLowerFeatures;

    vector<double> index(numFilterBanks + 2);

    double mel_start = MelBank::toMelScale(startFreq);
    double mel_end = MelBank::toMelScale(endFreq);
    double mel_step = (mel_end - mel_start) / (numFilterBanks + 1);
    for (uint32_t i = 0; i < numFilterBanks + 2; i++) {
        double freq = MelBank::fromMelScale(mel_start + i * mel_step);
        index[i] = floor(freq * (FFTSize + 1) / sampleRate);
    }

    for (uint32_t i = 0; i < numFilterBanks; i++) {
        filters_.push_back(
            MelBank(index[i], index[i + 1], index[i + 2], FFTSize));
    }

    initialized_ = true;
}

MelBankFeatures::MelBankFeatures(const MelBankFeatures &rhs) {
    classType = rhs.getClassType();
    featureExtractionType = classType;
    debugLog.setProceedingText("[DEBUG MelBankFeatures]");
    errorLog.setProceedingText("[ERROR MelBankFeatures]");
    warningLog.setProceedingText("[WARNING MelBankFeatures]");

    this->filters_.clear();
    this->num_lower_features_ = rhs.num_lower_features_;
    this->with_delta_ = rhs.with_delta_;
    *this = rhs;
}

MelBankFeatures& MelBankFeatures::operator=(const MelBankFeatures &rhs) {
    if (this != &rhs) {
        this->classType = rhs.getClassType();
        this->filters_ = rhs.getFilters();
        this->num_lower_features_ = rhs.num_lower_features_;
        this->with_delta_ = rhs.with_delta_;
        this->filters_ = rhs.getFilters();
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}

bool MelBankFeatures::deepCopyFrom(const FeatureExtraction *featureExtraction) {
    if (featureExtraction == NULL) return false;
    if (this->getFeatureExtractionType() ==
        featureExtraction->getFeatureExtractionType() ){
        // Invoke the equals operator to copy the data from the rhs instance to this instance
        *this = *(MelBankFeatures*)featureExtraction;
        return true;
    }

    errorLog << "clone(MelBankFeatures *featureExtraction)"
             << "-  FeatureExtraction Types Do Not Match!"
             << endl;
    return false;
}

bool MelBankFeatures::computeFeatures(const VectorDouble &inputVector) {
    // We assume the input is from a DFT (FFT) transformation.

    // 1. Filter the signal with Mel filters and get logged energies.
    uint32_t M = filters_.size();
    VectorDouble log_energies(M);
    for (uint32_t i = 0; i < M; i++) {
        double energy = filters_[i].filter(inputVector);
        if (energy == 0) {
            // Prevent log_energy goes to -inf...
            log_energies[i] = 0;
        } else {
            log_energies[i] = log(energy);
        }
    }

    // 2. Perform a DCT over logged energies (only get the lower 12 coefficients)
    featureVector.resize(num_lower_features_);
    for (uint32_t i = 0; i < num_lower_features_; i++) {
        featureVector[i] = 0;
        for (uint32_t j = 0; j < M; j++) {
            featureVector[i] += log_energies[j] * cos(PI / M * (j + 0.5) * i);
        }
    }

    return true;
}

bool MelBankFeatures::reset() {
    if (initialized_) { filters_.clear(); }
    return true;
}

}  // namespace GRT
