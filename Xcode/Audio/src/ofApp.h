#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxGrt.h"

#include <thread>

#include "istream.h"

class ofApp : public ofBaseApp {
  public:
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
    const size_t kNumMaxLabels_ = 9;

    // Input stream, a callback should be registered upon data arrival
    unique_ptr<IStream> istream_;
    // Callback used for input data stream (istream_)
    void onDataIn(vector<double> in);

    // Training related variables
    int buffer_size_;
    bool is_recording_;
    int label_;

    // Data and mutex
    std::mutex input_data_mutex_;
    vector<double> input_data_;
    vector<double> features_;

    // Pipeline
    GRT::GestureRecognitionPipeline pipeline_;
    GRT::ClassificationData training_data_;
    vector<vector<double>> feature_data_;
    int predicted_label_;

    // Visuals
    GRT::ofxGrtTimeseriesPlot plot_inputs_;
    GRT::ofxGrtTimeseriesPlot plot_pre_processed_;
    vector<GRT::ofxGrtTimeseriesPlot> plot_features_;
    vector<GRT::ofxGrtTimeseriesPlot> plot_samples_;
    vector<std::string> plot_samples_info_;

    ofxPanel gui_;
    bool gui_hide_;
    ofxButton save_pipeline_button_;
    void savePipeline();
    ofxButton load_pipeline_button_;
    void loadPipeline();

    GRT::MatrixDouble sample_data_;
    std::thread training_thread_;
};
