/** @example user_accelerometer_poses.cpp
 * Pose detection using accelerometers.
 */
#include <ESP.h>

ASCIISerialStream stream(115200, 3);
GestureRecognitionPipeline pipeline;
Calibrator calibrator;
TcpOStream oStream("localhost", 5204);

MatrixDouble uprightData, upsideDownData;
bool haveUprightData = false, haveUpsideDownData = false;
double range;
vector<double> zeroGs(3);

vector<double> processAccelerometerData(vector<double> input)
{
    vector<double> result(3);
    
    for (int i = 0; i < 3; i++) {
        result[i] = (input[i] - zeroGs[i]) / range;
    }
    
    return result;
}

CalibrateResult calibrate(const MatrixDouble& data) {
    CalibrateResult result = CalibrateResult::SUCCESS;
    
    // Run checks on newly collected sample.

    // take average of X and Y acceleration as the zero G value
    double zG = (data.getMean()[0] + data.getMean()[1]) / 2;
    double oG = data.getMean()[2]; // use Z acceleration as one G value
    
    double r = abs(oG - zG);
    vector<double> stddev = data.getStdDev();
    
    if (stddev[0] / r > 0.05 ||
        stddev[1] / r > 0.05 ||
        stddev[2] / r > 0.05)
        result = CalibrateResult(CalibrateResult::WARNING,
            "Accelerometer seemed to be moving; consider recollecting the "
            "calibration sample.");
    
    if (abs(data.getMean()[0] - data.getMean()[1]) / r > 0.1)
        result = CalibrateResult(CalibrateResult::WARNING,
            "X and Y axes differ by " + std::to_string(
            abs(data.getMean()[0] - data.getMean()[1]) / r * 100) +
            " percent. Check that accelerometer is flat.");
    
    // If we have both samples, do the actual calibration.

    if (haveUprightData && haveUpsideDownData) {
        for (int i = 0; i < 3; i++) {
            zeroGs[i] =
                (uprightData.getMean()[i] + upsideDownData.getMean()[i]) / 2;
        }
        
        // use half the difference between the two z-axis values (-1 and +1)
        // as the range
        range = (uprightData.getMean()[2] - upsideDownData.getMean()[2]) / 2;
    }

    return result;
}

CalibrateResult uprightDataCollected(const MatrixDouble& data)
{
    uprightData = data;
    haveUprightData = true;
    return calibrate(data);
}

CalibrateResult upsideDownDataCollected(const MatrixDouble& data)
{
    upsideDownData = data;
    haveUpsideDownData = true;
    return calibrate(data);
}

double null_rej = 5.0;
bool always_pick_something = false;

void updateAlwaysPickSomething(bool new_val) {
    pipeline.getClassifier()->enableNullRejection(!new_val);
}

void updateVariability(double new_val) {
    pipeline.getClassifier()->setNullRejectionCoeff(new_val);
    pipeline.getClassifier()->recomputeNullRejectionThresholds();
}

void setup()
{
    stream.setLabelsForAllDimensions({"x", "y", "z"});
    useStream(stream);
    useOutputStream(oStream);
    //useStream(stream);

    calibrator.setCalibrateFunction(processAccelerometerData);
    calibrator.addCalibrateProcess("Upright",
        "Rest accelerometer upright on flat surface.", uprightDataCollected);
    calibrator.addCalibrateProcess("Upside Down",
        "Rest accelerometer upside down on flat surface.", upsideDownDataCollected);
    useCalibrator(calibrator);

    pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 3, false, true, true, false, false));
    pipeline.setClassifier(ANBC(false, !always_pick_something, null_rej)); // use scaling, use null rejection, null rejection parameter
    // null rejection parameter is multiplied by the standard deviation to determine
    // the rejection threshold. the higher the number, the looser the filter; the
    // lower the number, the tighter the filter.

    usePipeline(pipeline);

    registerTuneable(always_pick_something, "Always Pick Something",
        "Whether to always pick (predict) one of the classes of training data, "
        "even if it's not a very good match. If selected, 'Variability' will "
        "not be used.", updateAlwaysPickSomething);
    registerTuneable(null_rej, 1.0, 25.0, "Variability",
         "How different from the training data a new gesture can be and "
         "still be considered the same gesture. The higher the number, the more "
         "different it can be.", updateVariability);
}
