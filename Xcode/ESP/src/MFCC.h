#ifndef ESP_MFCC_H_
#define ESP_MFCC_H_

#include "GRT/CoreModules/FeatureExtraction.h"

namespace GRT {

class MelBank {
  public:
    MelBank(uint32_t left, uint32_t middle, uint32_t right, uint32_t size) {
        filter_.resize(size);
        for (uint32_t i = 0; i < size; i++) {
            if (i <= left) {
                filter_[i] = 0;
            } else if (left < i && i <= middle) {
                filter_[i] = 1.0f * (i - left) / (middle - left);
            } else if (middle < i && i <= right) {
                filter_[i] = 1.0f * (right - i) / (right - middle);
            } else if (right < i) {
                filter_[i] = 0;
            } else {
                assert(false && "MelBank argument wrong or implementation bug");
            }
        }
    }

    double filter(vector<double> input) {
        uint32_t filter_size = filter_.size();
        assert(input.size() == filter_size && "Dimension mismatch in MelBank filter");
        vector<double> result(filter_size);

        double sum = 0;
        for (uint32_t i = 0; i < filter_size; i++) {
            sum += input[i] * filter_[i];
        }
        return sum;
    }

    static inline double toMelScale(double freq) {
        return 1125.0f * log(1.0f + freq / 700.0f);
    }

    static inline double fromMelScale(double mel_freq) {
        return 700.0f * (exp(mel_freq / 1125.0f) - 1.0f);
    }

  private:
    vector<double> filter_;
};

class MelBankFeatures : public FeatureExtraction {
  public:
    MelBankFeatures(uint32_t numFilterBanks = -1,
                    double startFreq = -1,
                    double endFreq = -1,
                    uint32_t FFTSize = -1,
                    uint32_t sampleRate = -1);
    MelBankFeatures(const MelBankFeatures &rhs);
    MelBankFeatures& operator=(const MelBankFeatures &rhs);
    bool deepCopyFrom(const FeatureExtraction *featureExtraction) override;
    ~MelBankFeatures() {}

    virtual bool computeFeatures(const VectorDouble &inputVector) override;
    virtual bool reset() override;

    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;

    vector<MelBank> getFilters() const {
        return filters_;
    }
  protected:
    bool initialized_;
    vector<MelBank> filters_;

    static RegisterFeatureExtractionModule<MelBankFeatures> registerModule;
};

}  //End of namespace GRT

#endif  // ESP_MFCC_H_
