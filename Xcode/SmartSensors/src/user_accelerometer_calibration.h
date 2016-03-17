#include <SmartSensors.h>

ASCIISerialStream stream(0, 9600, 3);
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
double zeroG, oneG;

// pass in calibrator so this function can call functions on it, e.g.
// to provide a warning to show the user if the data isn't good enough.
//
// return a bool indicating whether or not the calibration data is valid
// is that too strict?
void restingDataCollected(Calibrator &calibrator)
{
    std::vector<float> mean(3, 0.0);
    
    // assuming that Calibrator.getData() returns a MatrixDouble
    // is that reasonable?
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < calibrator.getData().getNumRows(); i++) mean[j] += calibrator.getData()[i][j];
        mean[j] /= calibrator.getData().getNumRows();
    }

    // TODO: give warning if mean[0] (X acceleration) and mean[1] (Y accleration) are different.
    zeroG = (mean[0] + mean[1]) / 2; // take average of X and Y acceleration as the zero G value
    oneG = mean[2]; // use Z acceleration as one G value (due to gravity)
}

void shakingDataCollected(Calibrator &calibrator) {
}

Calibrator resting("Resting", "Rest accelerometer on flat surface, w/ z-axis vertical.", restingDataCollected);
Calibrator shaking("Shaking", "Shake accelerometer vigorously along all axes.", shakingDataCollected);

vector<double> processAccelerometerData(vector<double> input)
{
    if (!resting.isCalibrated() || input.size() < 3) return input;

    vector<double> output(input.size());
    
    output[0] = (input[0] - zeroG) / (oneG - zeroG);
    output[1] = (input[1] - zeroG) / (oneG - zeroG);
    output[2] = (input[2] - zeroG) / (oneG - zeroG);
    
    return output;
}

void setup()
{
    stream.setLabelsForAllDimensions({"x", "y", "z"});
    // can't do this because the normalizer is called before data gets fed to calibrators
    // i.e. when recalibrating, you'd be using data normalized based on the current
    // calibration, as opposed to the raw data.
    stream.useNormalizer(processAccelerometerData);
    useStream(stream);
    
    useCalibrator(resting);
    useCalibrator(shaking);

    pipeline.addPreProcessingModule(MovingAverageFilter(5, 3));
    pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 3, false, true, true, false, false));
    pipeline.setClassifier(ANBC(false, true, 10.0));
    pipeline.addPostProcessingModule(ClassLabelFilter(3, 5));
    usePipeline(pipeline);

    
}