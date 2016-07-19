#ifndef ESP_MFCC_H_
#define ESP_MFCC_H_

#include "GRT/CoreModules/FeatureExtraction.h"

#include <math.h>
#include <stdint.h>
#include <vector>

namespace GRT {

using std::vector;

// TriFilterBanks contains the matrix that would perform the filter operation.
// Specifically, the multiplication will take the following form:
//
//   [  filter bank 1  ]     |----|
//   [  filter bank 2  ]
//   [   ...........   ]      fft
//   [   ...........   ]
//   [  filter bank N  ]     |____|
class TriFilterBanks {
  public:
    TriFilterBanks();
    ~TriFilterBanks();

    void initialize(uint32_t num_filter, uint32_t filter_size);
    void setFilter(uint32_t idx, double left, double middle, double right,
                   uint32_t fs);

    static inline double toMelScale(double freq) {
        return 1127.0f * log(1.0f + freq / 700.0f);
    }

    static inline double fromMelScale(double mel_freq) {
        return 700.0f * (exp(mel_freq / 1127.0f) - 1.0f);
    }

    inline uint32_t getNumFilters() const {
        return num_filter_;
    }

    void filter(const vector<double>& input, vector<double>& output);

  private:
    bool initialized_;
    double* filter_;
    uint32_t num_filter_;
    uint32_t filter_size_;
};

/* @brief MFCC class implements a variant of the Mel Frequency Cepstral
 * Coefficient algorithm. Typically MFCC would include pre-emphasis and FFT in
 * its own; in GRT these two steps can be achieved with a filter pre-processing
 * module and an FFT feature extraction module. Therefore, this MFCC
 * implementation assumes the input data is FFT (only one side, magnitude only
 * data). A typical parameter settings with GRT::FFT is the following:
 *
 *  GRT::FFT fft(512, 128, 1, GRT::FFT::HAMMING_WINDOW, true, false)`
 *
 * To use this class, create an MFCC::Options struct and fill in the desired
 * parameter. Below is an example that works for 16k audio and using the FFT
 * parameters above.
 *
 *    GRT::MFCC::Options options;
 *    options.sample_rate = 16000;
 *    options.fft_size = 512 / 2;
 *    options.start_freq = 300;
 *    options.end_freq = 8000;
 *    options.num_tri_filter = 26;
 *    options.num_cepstral_coeff = 12;
 *    options.lifter_param = 22;
 *    options.use_vad = true;
 *    GRT::MFCC mfcc(options);
 *
 * For more information about MFCC, please refer to the HTK Book [1]. This
 * implementation closely follows that's presented in the book and cross verfied
 * by the Matlab implementation.
 *
 * Note: This class has been optimized to use BLAS for matrix/vector
 * multiplication.
 *
 * [1] Young, S., Evermann, G., Gales, M., Hain, T., Kershaw, D., Liu, X.,
 *     Moore, G., Odell, J., Ollason, D., Povey, D., Valtchev, V., Woodland, P.,
 *     2006. The HTK Book (for HTK Version 3.4.1). Engineering Department,
 *     Cambridge University.  (see also: http://htk.eng.cam.ac.uk)
*/

class MFCC : public FeatureExtraction {
  public:
    struct Options {
        uint32_t sample_rate;        // The sampling frequency (Hz)
        uint32_t fft_size;           // The window size of FFT
        double start_freq;           // Higher frequency (Hz)
        double end_freq;             // Upper frequency (Hz)
        uint32_t num_tri_filter;     // Number of filter banks
        uint32_t num_cepstral_coeff; // Number of coefficient produced
        uint32_t lifter_param;       // Sinusoidal Lifter parameter
        bool use_vad;                // Voice Activity Detector
        double noise_level;          // Simple threshold for VAD
        Options()
            : sample_rate(0), fft_size(0), start_freq(-1), end_freq(-1),
              num_tri_filter(0), num_cepstral_coeff(0), lifter_param(0),
              use_vad(false), noise_level(0) {
        }

        bool operator==(const Options& rhs) {
            return this->sample_rate == rhs.sample_rate &&
                   this->fft_size == rhs.fft_size &&
                   this->start_freq == rhs.start_freq &&
                   this->end_freq == rhs.end_freq &&
                   this->num_tri_filter == rhs.num_tri_filter &&
                   this->num_cepstral_coeff == rhs.num_cepstral_coeff &&
                   this->lifter_param == rhs.lifter_param &&
                   this->use_vad == rhs.use_vad &&
                   this->noise_level == rhs.noise_level;
        }
    };

    MFCC(struct Options options = Options());

    MFCC(const MFCC& rhs);
    MFCC& operator=(const MFCC& rhs);
    bool deepCopyFrom(const FeatureExtraction* featureExtraction) override;
    ~MFCC() override {
        delete[] dct_matrix_;
    }

    void initialize();

    bool computeFeatures(const VectorDouble& inputVector) override;
    bool reset() override;

    // Save and Load from file
    bool saveModelToFile(string filename) const override;
    bool loadModelFromFile(string filename) override;
    bool saveModelToFile(fstream &file) const override;
    bool loadModelFromFile(fstream &file) override;

    struct Options getOptions() const {
        return options_;
    }
    TriFilterBanks getFilters() const {
        return filters_;
    }

  public:
    void computeLFBE(const vector<double>& fft, vector<double>& lfbe);
    void computeCC(const vector<double>& lfbe, vector<double>& cc);
    vector<double> getCC(const vector<double>& lfbe);
    vector<double> lifterCC(const vector<double>& cc);

  protected:
    bool initialized_;
    Options options_;

    // The information below can be generated with options_. We fill them during
    // the initialize() function.
    double* dct_matrix_;
    TriFilterBanks filters_;

    vector<double> tmp_lfbe_;
    vector<double> tmp_cc_;

    static RegisterFeatureExtractionModule<MFCC> registerModule;
};

} // namespace GRT

#endif // ESP_MFCC_H_
