/** @example user_accelerometer_walk_detection.cpp
 * Detect walking using an accelerometer.
 */
#include <ESP.h>

ASCIISerialStream stream(9600, 3);
GestureRecognitionPipeline pipeline;
Calibrator calibrator;

double zeroG = 0, oneG = 0;

double processAccelerometerData(double input)
{
    return (input - zeroG) / (oneG - zeroG);
}

CalibrateResult restingDataCollected(const MatrixDouble& data)
{
    // take average of X and Y acceleration as the zero G value
    zeroG = (data.getMean()[0] + data.getMean()[1]) / 2;
    oneG = data.getMean()[2]; // use Z acceleration as one G value
    
    double range = abs(oneG - zeroG);
    vector<double> stddev = data.getStdDev();
    
    if (stddev[0] / range > 0.05 ||
        stddev[1] / range > 0.05 ||
        stddev[2] / range > 0.05)
        return CalibrateResult(CalibrateResult::WARNING,
            "Accelerometer seemed to be moving; consider recollecting the "
            "calibration sample.");
    
    if (abs(data.getMean()[0] - data.getMean()[1]) / range > 0.1)
        return CalibrateResult(CalibrateResult::WARNING,
            "X and Y axes differ by " + std::to_string(
            abs(data.getMean()[0] - data.getMean()[1]) / range * 100) +
            " percent. Check that accelerometer is flat.");

    return CalibrateResult::SUCCESS;
}

VectorDouble dotProduct(VectorDouble in)
{
    VectorDouble out(1, 0);
    
    for (int i = 0; i < in.size(); i++)
        out[0] += in[i] * in[i];
    
    return out;
}

VectorDouble stddev(VectorDouble v) {
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / v.size();

    double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / v.size() - mean * mean);
    
    return VectorDouble(1, stdev);
}

double t = 0.6;
VectorDouble threshold(VectorDouble in) {
    VectorDouble out(in.size());
    for (int i = 0; i < in.size(); i++)
        out[i] = (in[i] > t) ? 1.0 : 0.0;
    return out;
}

void setup()
{
    stream.setLabelsForAllDimensions({"x", "y", "z"});
    useStream(stream);

    calibrator.setCalibrateFunction(processAccelerometerData);
    calibrator.addCalibrateProcess("Resting",
        "Rest accelerometer on flat surface.", restingDataCollected);
    useCalibrator(calibrator);

    pipeline.addFeatureExtractionModule(FeatureApply(3, 1, dotProduct));
    pipeline.addFeatureExtractionModule(TimeseriesBuffer(80, 1));
    pipeline.addFeatureExtractionModule(FeatureApply(80, 1, stddev));
    pipeline.addFeatureExtractionModule(FeatureApply(1, 1, threshold));
    usePipeline(pipeline);
    
    registerTuneable(t, 0, 1.0, "Walking Threshold",
        "How much the accelerometer data needs to be "
        "changing to be considered walking.");
}
