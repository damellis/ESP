/** @example user_color_sensor.cpp
 * Color sensing. See <a href="https://github.com/damellis/ESP/wiki/%5BExample%5D-Color-Detection">documentation on the wiki</a>.
 */
#include <ESP.h>

// Normalize by dividing each dimension by the total magnitude.
// Also add the magnitude as an additional feature.
vector<double> normalize(vector<double> input) {
    double magnitude = 0.0;

    for (int i = 0; i < input.size(); i++) magnitude += (input[i] * input[i]);
    magnitude = sqrt(magnitude);
    for (int i = 0; i < input.size(); i++) input[i] /= magnitude;

    return input;
}

ASCIISerialStream stream(0, 9600, 3);
GestureRecognitionPipeline pipeline;
TcpOStream oStream("localhost", 5204, 3, "l", "r", " ");

bool always_pick_something = false;
double null_rej = 5.0;
bool send_repeated_predictions = false;
int timeout = 100;

void setup() {
    stream.useNormalizer(normalize);
    stream.setLabelsForAllDimensions({"red", "green", "blue"});
    useInputStream(stream);
    useOutputStream(oStream);
    // useStream(stream);

    pipeline.addPreProcessingModule(MovingAverageFilter(5, 3));
    // use scaling, use null rejection, null rejection parameter
    pipeline.setClassifier(ANBC(false, !always_pick_something, null_rej));

    // null rejection parameter is multiplied by the standard deviation to determine
    // the rejection threshold. the higher the number, the looser the filter; the
    // lower the number, the tighter the filter.

    if (send_repeated_predictions)
        pipeline.addPostProcessingModule(ClassLabelTimeoutFilter(timeout));

    usePipeline(pipeline);

    registerTuneable(always_pick_something, "Always Pick Something",
        "Whether to always pick (predict) one of the classes of training data, "
        "even if it's not a very good match. If selected, 'Color Variability' "
        "will not be used.");
    registerTuneable(null_rej, 1.0, 25.0, "Color Variability",
         "How different from the training data a new color reading can be and "
         "still be considered the same color. The higher the number, the more "
         "different it can be.");
    registerTuneable(send_repeated_predictions, "Send Repeated Predictions",
        "Whether to send repeated predictions while a pose is being held. If "
        "not selected, predictions will only be sent on transition from one "
        "pose to another.");
    registerTuneable(timeout, 1, 3000,
        "Timeout",
        "How long (in milliseconds) to wait after recognizing a class before "
        "recognizing a different one. Only used if 'Send Repeated Predictions' "
        "is selected.");
}
