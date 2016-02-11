#include "GRT/GRT.h"
#include "istream.h"

using namespace GRT;

ASCIISerialStream stream(115200, 3);
GestureRecognitionPipeline pipeline;

// Probably need a function that gets called when the calibration data is
// collected, which can compute any intermediate values needed (e.g. the
// mean or std. dev. of the calibration) to make use of the calibration
// data.

// Also need a function that gets called for every incoming sample, that
// adjusts the incoming data based on the calibration data.
// Question: how do you specify this function? For instance, you might
// want one function to handle multiple calibrators, or one function
// for each calibrator.

// Question: how does calibration fit in with normalization? Is it a
// substitute for manual normalization? If not, which should happen
// first / how should that order be specified? (That order probably
// needs to be consistent between the collection and processing of
// calibration data and its use -- i.e. calibration data should either
// always be post-normalization or always pre-normalization.)

// Question: how does the example code pass warnings or error about the
// calibration data to the user (e.g. if the values are very different
// than expected)?


//// accelerometer characteristics to be calculated from calibration
//double zeroG, oneG;
//
//// pass in the calibrator so you can use the same function for multiple
//// calibrators; is that useful?
////
//// return a bool indicating whether or not the calibration data is valid
//// is that too strict?
//bool restingDataCollected(Calibrator calibrator)
//{
//    vector<float> mean(3, 0.0);
//    
//    // assuming that Calibrator.getData() returns a MatrixDouble
//    // is that reasonable?
//    for (int j = 0; j < 3; j++)
//        for (int i = 0; i < calibrator.getData().size(); i++) mean[j] += calibrator.getData()[i][j];
//        mean[j] /= calibrator.getData().size();
//    }
//
//    // TODO: give warning if mean[0] (X acceleration) and mean[1] (Y accleration) are different.
//    zeroG = (mean[0] + mean[1]) / 2; // take average of X and Y acceleration as the zero G value
//    oneG = mean[2]; // use Z acceleration as one G value (due to gravity)
//
//    return true;
//}
//
//vector<double> adjustForRestingData(vector<double> input)
//{
//    vector<double> output;
//    
//    output[0] = (input[0] - zeroG) / oneG;
//    output[1] = (input[1] - zeroG) / oneG;
//    output[2] = (input[2] - zeroG) / oneG;
//    
//    return output;
//}
//
//Calibrator resting("Accelerometer resting flat, w/ z-axis vertical.", restingDataCollected, adjustForRestingData);

void setup()
{
    stream.useUSBPort(0);
    useStream(stream);
    
    pipeline.addPreProcessingModule(MovingAverageFilter(5, 3));
    pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 3, false, true, true, false, false));
    pipeline.setClassifier(ANBC(false, true, 10.0));
    pipeline.addPostProcessingModule(ClassLabelFilter(3, 5));
    usePipeline(pipeline);

    
}