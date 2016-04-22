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

void setup() {
    stream.useNormalizer(normalize);
    stream.setLabelsForAllDimensions({"red", "green", "blue"});
    useStream(stream);

    pipeline.addPreProcessingModule(MovingAverageFilter(5, 3));
    // use scaling, use null rejection, null rejection parameter
    pipeline.setClassifier(ANBC(false, !always_pick_something, null_rej));

    // null rejection parameter is multiplied by the standard deviation to determine
    // the rejection threshold. the higher the number, the looser the filter; the
    // lower the number, the tighter the filter.

    usePipeline(pipeline);
    useOStream(oStream);

    registerTuneable(always_pick_something, "Always Pick Something",
        "Whether to always pick (predict) one of the classes of training data, "
        "even if it's not a very good match. If selected, 'Color Variability' "
        "will not be used.");

    registerTuneable(null_rej, 1.0, 25.0, "Color Variability",
         "How different from the training data a new color reading can be and "
         "still be considered the same color. The higher the number, the more "
         "different it can be.");
}
