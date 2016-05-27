/** @example user_audio_beat.cpp
 * Audio beat detection examples.
 */
#include <ESP.h>

AudioStream stream(1);
GestureRecognitionPipeline pipeline;

// Audio defaults to 44.1k sampling rate.
uint32_t kFFT_WindowSize = 1024;
uint32_t kFFT_HopSize = 1024;
uint32_t DIM = 1;
uint32_t BINS = 32;
uint32_t HISTORY = 43;

double C = 2.5;

VectorDouble binFFT(VectorDouble in) {
    VectorDouble out(BINS, 0.0);
    uint32_t M = in.size() / BINS;
    for (int i = 0; i < BINS; i++) {
        for (int j = 0; j < M; j++) {
            out[i] += in[i * M + j];
        }
    }
    return out;
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

    pipeline.addFeatureExtractionModule(
        FFT(kFFT_WindowSize, kFFT_HopSize,
            DIM, FFT::RECTANGULAR_WINDOW, true, false));
    pipeline.addFeatureExtractionModule(FeatureApply(kFFT_WindowSize / 2, BINS, binFFT));
    pipeline.addFeatureExtractionModule(TimeseriesBuffer(HISTORY, BINS));
    pipeline.addFeatureExtractionModule(FeatureApply(BINS * HISTORY,BINS * 2,meanAndLast));
    pipeline.addFeatureExtractionModule(FeatureApply(BINS * 2, BINS, detectBeat));

    usePipeline(pipeline);
}
