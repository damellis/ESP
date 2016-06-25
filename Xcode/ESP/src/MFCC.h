#ifndef ESP_MFCC_H_
#define ESP_MFCC_H_

#include "GRT/CoreModules/FeatureExtraction.h"

#include <stdint.h>
#include <math.h>
#include <vector>

namespace GRT {

class TriFilterBank {
  public:
    TriFilterBank(double left, double middle, double right, uint32_t fs, uint32_t size);

    static inline double toMelScale(double freq) {
        return 1127.0f * log(1.0f + freq / 700.0f);
    }

    static inline double fromMelScale(double mel_freq) {
        return 700.0f * (exp(mel_freq / 1127.0f) - 1.0f);
    }

    inline vector<double>& getFilter() { return filter_;  }

    double filter(vector<double> input);

  private:
    vector<double> filter_;
};

class MFCC : public FeatureExtraction {
  public:
    MFCC(uint32_t sampleRate = -1, uint32_t FFTSize = -1,
         double startFreq = -1, double endFreq = -1,
         uint32_t numFilterbankChannel = -1,
         uint32_t numCepstralCoeff = -1,
         uint32_t lifterParam = -1);

    MFCC(const MFCC &rhs);
    MFCC& operator=(const MFCC &rhs);
    bool deepCopyFrom(const FeatureExtraction *featureExtraction) override;
    ~MFCC() {}

    virtual bool computeFeatures(const VectorDouble &inputVector) override;
    virtual bool reset() override;

    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;

    vector<TriFilterBank> getFilters() const {
        return filters_;
    }
  protected:
    vector<double> getLFBE(const vector<double>& fft);
    vector<double> getCC(const vector<double>& lfbe);
    vector<double> lifterCC(const vector<double>& cc);

    bool initialized_;
    uint32_t num_cc_;
    uint32_t lifter_param_;

    vector<TriFilterBank> filters_;

    static RegisterFeatureExtractionModule<MFCC> registerModule;
};

}  // namespace GRT

#endif  // ESP_MFCC_H_
