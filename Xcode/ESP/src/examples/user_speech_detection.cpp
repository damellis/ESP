#include <ESP.h>
#include <Filter.h>

class LogEnergy : public Filter {
public:
    LogEnergy(UINT filterSize = 5,UINT numDimensions = 1)
        : Filter("LogEnergy", filterSize, numDimensions)
    {}
    
    double computeFilter(const VectorDouble &buf) {
        double energy = 0;
        for (int i = 0; i < buf.size(); i++) {
            energy += buf[i] * buf[i];
        }
        return log(energy);
    }
    
private:
    static RegisterPreProcessingModule< LogEnergy > registerModule;
};

RegisterPreProcessingModule< LogEnergy > LogEnergy::registerModule("LogEnergy");

class SpeechDetection : public Filter {
public:
    SpeechDetection() : Filter("SpeechDetection", 1, 1), inSpeech(false), noiseMean(0.0)
    {}
    
    double computeFilter(const VectorDouble &buf) {
        double val = buf[0];
        if (!inSpeech) {
            double delta = val - noiseMean;
            
            noiseMean -= 0.01 * noiseMean;
            noiseMean += 0.01 * val;
            
            noiseM2 += delta * (val - noiseMean);
            noiseSigma = sqrt(noiseM2 * 0.01);
        }
        
        return noiseMean;
    }

private:
    bool inSpeech;
    double noiseMean, noiseM2, noiseSigma;
    static RegisterPreProcessingModule< SpeechDetection > registerModule;
};

RegisterPreProcessingModule< SpeechDetection > SpeechDetection::registerModule("SpeechDetection");

AudioStream stream(100);
GestureRecognitionPipeline pipeline;

void setup() {
    useInputStream(stream);
    pipeline.addPreProcessingModule(LogEnergy(5, 1));
    pipeline.addPreProcessingModule(SpeechDetection());
    usePipeline(pipeline);
}