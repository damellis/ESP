// Using algorithm 3 from "A Comparitive Study of Speech Detection Methods"
// by Stefaan Van Gerven and Fei Xie
// http://www.mirlab.org/conference_papers/International_Conference/Eurospeech%201997/pdf/tab/a0199.pdf

#include <ESP.h>
#include <Filter.h>
#include <ThresholdDetection.h>

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

AudioStream stream(100);
GestureRecognitionPipeline pipeline;

double alpha = 4.0, beta = 1.2;

void setup() {
    useInputStream(stream);
    pipeline.addPreProcessingModule(LogEnergy(5, 1));
    pipeline.addFeatureExtractionModule(ThresholdDetection(100,1,alpha,beta));
    usePipeline(pipeline);
    
    registerTuneable(alpha, 1.0, 10.0, "Loudness Threshold",
        "How loud (relative to background noise) a sound has to be to count "
        "as speech / foreground audio.");
    registerTuneable(beta, 1.0, 10.0, "Quietness Threshold",
        "How quiest (relative to background noise) the speech / foreground "
        "audio has to get to be considered finished.");
}