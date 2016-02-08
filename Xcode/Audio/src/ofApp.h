#pragma once

// C System
#include <stdint.h>

// C++ System
#include <thread>

// of System
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxGrt.h"

// custom
#include "istream.h"

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

  private:
    enum Fragment { PIPELINE, TRAINING, DATA };
    Fragment fragment_;
    void drawLivePipeline();
    void drawTrainingInfo();

    void loadTrainingData();

    void useStream(IStream &stream);
    void usePipeline(GRT::GestureRecognitionPipeline &pipeline);

    friend void useStream(IStream &stream);
    friend void usePipeline(GRT::GestureRecognitionPipeline &pipeline);

    uint32_t num_pipeline_stages_;

    // Currently, we support labels (stored in label_) from 1 to 9.
    const uint32_t kNumMaxLabels_ = 9;
    uint8_t label_;

    // kBufferSize_ controls the number of points in the plot. Note: This is not
    // the buffer size used for training/prediction.
    const uint32_t kBufferSize_ = 256;

    // Input stream, a callback should be registered upon data arrival
    IStream *istream_;
    // Callback used for input data stream (istream_)
    void onDataIn(GRT::MatrixDouble in);

    // When button 1-9 is pressed, is_recording_ will be set and data will be
    // added to sample_data_.
    bool is_recording_;
    GRT::MatrixDouble sample_data_;

    // input_data_ is written by istream_ thread and read by GUI thread.
    std::mutex input_data_mutex_;
    GRT::MatrixDouble input_data_;

    // Pipeline
    GRT::GestureRecognitionPipeline *pipeline_;
    GRT::ClassificationData training_data_;
    GRT::ClassificationData test_data_;
    float training_accuracy_;
    int predicted_label_;

    // Visuals
    ofxGrtTimeseriesPlot plot_inputs_;
    vector<ofxGrtTimeseriesPlot> plot_pre_processed_;
    vector<vector<ofxGrtTimeseriesPlot>> plot_features_;
    vector<ofxGrtTimeseriesPlot> plot_samples_;
    vector<std::string> plot_samples_info_;
    ofxGrtTimeseriesPlot plot_prediction_;

    // Panel for storing and loading pipeline.
    ofxPanel gui_;
    bool gui_hide_;
    ofxButton save_pipeline_button_;
    void savePipeline();
    ofxButton load_pipeline_button_;
    void loadPipeline();

    // Multithreading to avoid GUI blocked.
    std::thread training_thread_;
};
