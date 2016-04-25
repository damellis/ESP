#pragma once

// C System
#include <stdint.h>

// C++ System
#include <thread>

// of System
#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxGui.h"
#include "ofxGrt.h"

// custom
#include "calibrator.h"
#include "istream.h"
#include "plotter.h"
#include "ostream.h"
#include "tuneable.h"

class ofApp : public ofBaseApp {
  public:
    ofApp();
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void registerTuneable(Tuneable* t) {
        tuneable_parameters_.push_back(t);
    }

    void reloadPipelineModules();
  private:
    enum Fragment { CALIBRATION, PIPELINE, TRAINING, ANALYSIS };
    Fragment fragment_;
    void drawCalibration();
    void drawLivePipeline();
    void drawTrainingInfo();
    void drawAnalysis();

    void useCalibrator(Calibrator &calibrator);
    void useStream(IStream &stream);
    void usePipeline(GRT::GestureRecognitionPipeline &pipeline);
    void useOStream(OStream &stream);

    friend void useCalibrator(Calibrator &calibrator);
    friend void useStream(IStream &stream);
    friend void usePipeline(GRT::GestureRecognitionPipeline &pipeline);
    friend void useOStream(OStream &stream);

    uint32_t num_pipeline_stages_;

    // Currently, we support labels (stored in label_) from 1 to 9.
    const uint32_t kNumMaxLabels_ = 9;
    uint8_t label_;

    // kBufferSize_ controls the number of points in the plot. Note: This is not
    // the buffer size used for training/prediction.
    const uint32_t kBufferSize_ = 256;

    // The calibrator that is in use.
    Calibrator *calibrator_;

    // Input stream, a callback should be registered upon data arrival
    IStream *istream_;
    // Callback used for input data stream (istream_)
    void onDataIn(GRT::MatrixDouble in);

    // Input stream, a callback should be registered upon data arrival
    OStream *ostream_;

    // When button 1-9 is pressed, is_recording_ will be set and data will be
    // added to sample_data_.
    bool is_recording_;
    GRT::MatrixDouble sample_data_;

    // input_data_ is written by istream_ thread and read by GUI thread.
    std::mutex input_data_mutex_;
    GRT::MatrixDouble input_data_;

    // Pipeline
    GRT::GestureRecognitionPipeline *pipeline_;
    GRT::TimeSeriesClassificationData training_data_;
    GRT::MatrixDouble test_data_;
    float training_accuracy_;
    int predicted_label_;
    vector<double> predicted_class_distances_;
    vector<double> predicted_class_likelihoods_;
    vector<UINT> predicted_class_labels_;
    vector<UINT> test_data_predicted_class_labels_;

    // Visuals
    ofxGrtTimeseriesPlot plot_raw_;
    ofxGrtTimeseriesPlot plot_inputs_;

    vector<Plotter> plot_calibrators_;

    vector<ofxGrtTimeseriesPlot> plot_pre_processed_;
    vector<vector<ofxGrtTimeseriesPlot>> plot_features_;
    vector<Plotter> plot_samples_;
    vector<std::string> plot_samples_info_;
    // Features associated with each sample.
    vector<vector<Plotter>> plot_sample_features_;
    void toggleFeatureView();
    bool is_in_feature_view_ = false;
    void populateSampleFeatures(uint32_t sample_index);
    vector<pair<double, double>> sample_feature_ranges_;

    ofxGrtTimeseriesPlot plot_prediction_;
    vector<int> plot_sample_indices_; // the index of the currently plotted sample for each class label
    vector<pair<ofRectangle, ofRectangle>> plot_sample_button_locations_;
    void onPlotRangeSelected(Plotter::CallbackArgs arg);
    bool is_final_features_too_many_ = false;

    Plotter plot_testdata_overview_;
    ofxGrtTimeseriesPlot plot_testdata_window_;
    void onTestOverviewPlotSelection(Plotter::CallbackArgs arg);
    void updateTestWindowPlot();
    void runPredictionOnTestData();

    // Panel for storing and loading pipeline.
    ofxDatGui gui_;
    bool gui_hide_;
    ofxButton save_pipeline_button_;
    void savePipeline();
    ofxButton load_pipeline_button_;
    void loadPipeline();
    ofxButton save_training_data_button_;

    ofxButton load_training_data_button_;
    void loadTrainingData();
    void saveTrainingData();

    void trainModel();

    vector<ofxPanel *> training_sample_guis_;
    void renameTrainingSample(int num);
    void renameTrainingSampleDone();
    void deleteTrainingSample(int num);
    void trimTrainingSample(int num);
    void relabelTrainingSample(int num);
    void doRelabelTrainingSample(uint32_t from, uint32_t to);

    // Rename
    bool is_in_renaming_ = false;
    int rename_target_ = -1;
    string rename_title_;
    // Display title is rename_title_ plus a blinking underscore.
    string display_title_;

    // Multithreading to avoid GUI blocked.
    std::thread training_thread_;

    void loadCalibrationData();
    void saveCalibrationData();

    void loadTestData();
    void saveTestData();
    
    // Prompts to ask the user to save the calibration data if changed.
    bool should_save_calibration_data_;
    // Prompts to ask the user to save the training data if changed.
    bool should_save_training_data_;
    // Prompts to ask the user to save the test data if changed.
    bool should_save_test_data_;

    friend class TrainingSampleGuiListener;

    void updateEventReceived(ofEventArgs& arg);
    uint32_t update_counter_ = 0;

    // Relabel
    bool is_in_relabeling_ = false;
    uint32_t relabel_source_;

    // tuneable parameters
    vector<Tuneable*> tuneable_parameters_;
};

class TrainingSampleGuiListener {
  public:
    TrainingSampleGuiListener(ofApp *app, int num) : app(app), num(num) {}
    void renameButtonPressed() { app->renameTrainingSample(num); }
    void deleteButtonPressed() { app->deleteTrainingSample(num); }
    void trimButtonPressed() { app->trimTrainingSample(num); }
    void relabelButtonPressed() { app->relabelTrainingSample(num); }
  private:
    ofApp *app;
    int num;
};
