/** @example user_accelerometer_gestures_osc.cpp
 */
#include <ESP.h>

OscInputStream stream(8001, "/gyrosc/accel", 3);
GestureRecognitionPipeline pipeline;

int timeout = 500; // milliseconds
double null_rej = 0.4;

void setup() {
    stream.setLabelsForAllDimensions({"x", "y", "z"});
    useInputStream(stream);

    DTW dtw(false, true, null_rej);
    dtw.enableTrimTrainingData(true, 0.1, 75);

    pipeline.setClassifier(dtw);
    pipeline.addPostProcessingModule(ClassLabelTimeoutFilter(timeout));
    usePipeline(pipeline);

    registerTuneable(
        null_rej, 0.1, 5.0, "Variability",
        "How different from the training data a new gesture can be and "
        "still be considered the same gesture. The higher the number, the "
        "more different it can be.",
        [](double new_null_rej) {
            pipeline.getClassifier()->setNullRejectionCoeff(new_null_rej);
            pipeline.getClassifier()->recomputeNullRejectionThresholds();
        });

    registerTuneable(
        timeout, 1, 3000, "Timeout",
        "How long (in milliseconds) to wait after recognizing a "
        "gesture before recognizing another one.",
        [](double new_timeout) {
            ClassLabelTimeoutFilter* filter =
                dynamic_cast<ClassLabelTimeoutFilter*>(
                    pipeline.getPostProcessingModule(0));
            assert(filter != nullptr);
            filter->setTimeoutDuration(new_timeout);
        });
}
