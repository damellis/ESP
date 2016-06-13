#include "MFCC.h"

namespace GRT {

RegisterFeatureExtractionModule<MelBankFeatures>
MelBankFeatures::registerModule("MelBankFeatures");

MelBankFeatures::MelBankFeatures(uint32_t numFilterBanks,
                                 double startFreq, double endFreq,
                                 uint32_t FFTSize, uint32_t sampleRate)
        : initialized_(false) {
    classType = "MelBankFeatures";
    featureExtractionType = classType;
    debugLog.setProceedingText("[INFO MelBankFeatures]");
    debugLog.setProceedingText("[DEBUG MelBankFeatures]");
    errorLog.setProceedingText("[ERROR MelBankFeatures]");
    warningLog.setProceedingText("[WARNING MelBankFeatures]");

    if (numFilterBanks <= 0 ||
        startFreq <= 0 || endFreq <= 0 ||
        FFTSize <= 0 || sampleRate <= 0) {
        return;
    }

    numInputDimensions = FFTSize;
    numOutputDimensions = numFilterBanks;

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
    *this = rhs;
}

MelBankFeatures& MelBankFeatures::operator=(const MelBankFeatures &rhs) {
    if (this != &rhs) {
        this->classType = rhs.getClassType();
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
    uint32_t size = filters_.size();
    featureVector.resize(size);
    for (uint32_t i = 0; i < size; i++) {
        featureVector[i] = filters_[i].filter(inputVector);
    }
    return true;
}

bool MelBankFeatures::reset() {
    if (initialized_) { filters_.clear(); }
    return true;
}

}  // namespace GRT
