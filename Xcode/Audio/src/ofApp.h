#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxGrt.h"

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
    // Callback used for user interaction (dropdown menu).
    void onDropdownSelected(ofxDatGuiDropdownEvent e);

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

    // Visuals
    GRT::ofxGrtTimeseriesPlot plot_inputs_;
    GRT::ofxGrtTimeseriesPlot plot_pre_processed_;
    GRT::ofxGrtTimeseriesPlot plot_features_;
};
