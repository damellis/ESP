/** @example user_audio_beat.cpp
 * Audio beat detection example. Based on: http://archive.gamedev.net/archive/reference/programming/features/beatdetection/
 */
#include <ESP.h>

//AudioStream stream(1);
AudioFileStream stream("train1.wav", true);
GestureRecognitionPipeline pipeline;

// Audio defaults to 44.1k sampling rate.
uint32_t kFFT_WindowSize = 1024;
uint32_t kFFT_HopSize = 1024;
uint32_t DIM = 1;
uint32_t BINS;
uint32_t BIN_WIDTH_START = 2;
uint32_t BIN_WIDTH_DELTA = 4;
uint32_t HISTORY = 43;

double C = 2.0;

VectorDouble increasingBinWidths(VectorDouble in) {
    VectorDouble out;
    int bin_width = BIN_WIDTH_START;
    int i = 0;
    while (i < in.size()) {
        double val = 0;
        if (i + bin_width >= in.size()) bin_width = in.size() - i;
        for (int j = 0; j < bin_width; j++) {
            val += in[i + j];
        }
        out.push_back(val / bin_width);
        i += bin_width;
        bin_width *= BIN_WIDTH_DELTA;
    }
    return out;
}

int getNumIncreasingBins(int in_size) {
    int bin_width = BIN_WIDTH_START;
    int i = 0, out_size = 0;
    while (i < in_size) {
        out_size++;
        if (i + bin_width >= in_size) bin_width = in_size - i;
        std::cout << "Bin " << out_size << ": band " << i << " (" << 44100.0 / 1024 * i << " Hz)" << " to ";
        i += bin_width;
        std::cout << "band " << (i - 1) << " (" << 44100.0 / 1024 * (i - 1) << " Hz)" << std::endl;
        bin_width *= BIN_WIDTH_DELTA;
    }
    return out_size;
}

VectorDouble meanAndLast(VectorDouble in) {
    VectorDouble out(BINS * 2);
    
    for (int i = 0; i < BINS; i++) {
        double mean = 0;
        for (int j = 0; j < HISTORY; j++) {
            mean += in[i * HISTORY + j];
        }
        mean /= HISTORY;
        out[i * 2] = mean;
        out[i * 2 + 1] = in[i * HISTORY + HISTORY - 1];
    }
    
    return out;
}

VectorDouble detectBeat(VectorDouble in) {
    VectorDouble out(BINS, 0.0);
    
    for (int i = 0; i < BINS; i++) {
        if (in[i * 2 + 1] > in[i * 2] * C) out[i] = 1.0;
    }
    
    return out;
}

void setup() {
    stream.setLabelsForAllDimensions({"audio"});
    useInputStream(stream);
    
//    pipeline.addFeatureExtractionModule(
//        FFT(kFFT_WindowSize, kFFT_HopSize,
//            DIM, FFT::RECTANGULAR_WINDOW, true, false));
    
    BINS = getNumIncreasingBins(kFFT_WindowSize / 2);
    pipeline.addFeatureExtractionModule(FeatureApply(kFFT_WindowSize / 2, BINS, increasingBinWidths));
    pipeline.addFeatureExtractionModule(TimeseriesBuffer(HISTORY, BINS));
    pipeline.addFeatureExtractionModule(FeatureApply(BINS * HISTORY,BINS * 2,meanAndLast));
    pipeline.addFeatureExtractionModule(FeatureApply(BINS * 2, BINS, detectBeat));

    usePipeline(pipeline);
    
    registerTuneable(C, 1.0, 5.0, "Beat Threshold",
        "How many times louder than the average volume (over the last second) "
        "it needs to be to be considered a beat. Applied separately to each "
        "frequency band.");
}
