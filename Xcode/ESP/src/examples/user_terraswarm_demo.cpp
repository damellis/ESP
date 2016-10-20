/** @example user_accelerometer_gestures.cpp
 * Gesture detection using accelerometers. See the <a href="https://create.arduino.cc/projecthub/mellis/gesture-recognition-using-accelerometer-and-esp-mac-only-71faa1">associated tutorial</a>.
 */
#include <ESP.h>

ASCIISerialStream stream(0, 115200, 3);
//GDPInputStream stream("edu.berkeley.eecs.bid.mellis.arduino101", 3);
GestureRecognitionPipeline pipeline;

//TcpOStream oStream("localhost", 5204);
//std::map<uint32_t, char> key_mapping = { {1, 0}, {2, 0}, {3, ' '} };
//std::map<uint32_t, uint16_t> key_code_mapping = { {1, 0x7C}, {2, 0x7B} };
//MacOSKeyboardOStream oStream(key_mapping, key_code_mapping);
//MacOSKeyboardOStream oStream(3, 0, 0x7C, 0, 0x7B, ' '); // right, left, space
//MacOSKeyboardOStream oStream(2, 'N', 'P');
MacPowerPointOStream oStream;

GDPOutputStream oStream2("edu.berkeley.eecs.bid.mellis.esp2");


TrainingSampleCheckerResult checkTrainingSample(const MatrixDouble &in)
{
    if (in.getNumRows() < 10)
        return TrainingSampleCheckerResult(TrainingSampleCheckerResult::WARNING,
            "Warning: Sample is short. Did you hold down the key for "
            "the whole time you were making the gesture?");
    VectorDouble stddev = in.getStdDev();
    if (*max_element(stddev.begin(), stddev.end()) < 0.1)
        return TrainingSampleCheckerResult(TrainingSampleCheckerResult::WARNING,
            "Warning: Gesture contains very little movement.");
    return TrainingSampleCheckerResult::SUCCESS;
}

int timeout = 500; // milliseconds
double null_rej = 0.4;

void updateVariability(double new_null_rej) {
    pipeline.getClassifier()->setNullRejectionCoeff(new_null_rej);
    pipeline.getClassifier()->recomputeNullRejectionThresholds();
}

void updateTimeout(int new_timeout) {
    ClassLabelTimeoutFilter *filter =
        dynamic_cast<ClassLabelTimeoutFilter *>
            (pipeline.getPostProcessingModule(0));
    assert(filter != nullptr);
    filter->setTimeoutDuration(new_timeout);
}

void setup()
{
    stream.setLabelsForAllDimensions({"x", "y", "z"});
    useInputStream(stream);
    useOutputStream(oStream);
    useOutputStream(oStream2);

    DTW dtw(false, true, null_rej);
    dtw.enableTrimTrainingData(true, 0.1, 75);

    pipeline.setClassifier(dtw);
    pipeline.addPostProcessingModule(ClassLabelTimeoutFilter(timeout));
    usePipeline(pipeline);

    registerTuneable(null_rej, 0.1, 5.0, "Variability",
         "How different from the training data a new gesture can be and "
         "still be considered the same gesture. The higher the number, the "
         "more different it can be.", updateVariability);
    registerTuneable(timeout, 1, 3000,
        "Timeout",
        "How long (in milliseconds) to wait after recognizing a "
        "gesture before recognizing another one.", updateTimeout);

    useTrainingSampleChecker(checkTrainingSample);
  
    setTruePositiveWarningThreshold(0.60);
    setFalseNegativeWarningThreshold(0.30);
}
