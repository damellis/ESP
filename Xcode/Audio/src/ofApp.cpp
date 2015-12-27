#include "ofApp.h"

#include <algorithm>
#include <math.h>

#include "user.h"

//--------------------------------------------------------------
void ofApp::setup() {
    buffer_size_ = 256;
    is_recording_ = false;

    istream_.reset(new AudioStream());
    istream_->onDataReadyEvent(this, &ofApp::onDataIn);

    // setupPipeline is a user-defined function.
    pipeline_ = setupPipeline();

    plot_inputs_.setup(buffer_size_, 1, "Input");
    plot_inputs_.setDrawGrid(true);
    plot_inputs_.setDrawInfoText(true);

    size_t num_pre_processing = pipeline_.getNumPreProcessingModules();
    PreProcessing* pp = pipeline_.getPreProcessingModule(0);
    plot_pre_processed_.setup(buffer_size_, pp->getNumOutputDimensions(),
                              "PreProcessing");
    plot_pre_processed_.setDrawGrid(true);
    plot_pre_processed_.setDrawInfoText(true);

    size_t num_feature_modules = pipeline_.getNumFeatureExtractionModules();
    FeatureExtraction* fe = pipeline_.getFeatureExtractionModule(1);
    plot_features_.setup(buffer_size_, fe->getNumOutputDimensions(),
                         "Features");
    plot_features_.setDrawGrid(true);
    plot_features_.setDrawInfoText(true);

    training_data_.setNumDimensions(1);
    training_data_.setDatasetName("Audio");
    training_data_.setInfoText("This data contains audio data");

    ofBackground(54, 54, 54);
}

//--------------------------------------------------------------
void ofApp::update() {
    for (int i = 0; i < input_data_.size(); i++){
        vector<double> data_point = {
            input_data_[i]
        };

        plot_inputs_.update(data_point);

        if (istream_->hasStarted()) {
            if (!pipeline_.preProcessData(data_point)) {
                ofLog(OF_LOG_ERROR) << "ERROR: Failed to compute features!";
            }

            vector<double> pre_processed_data = pipeline_.getPreProcessedData();
            plot_pre_processed_.update(pre_processed_data);

            // The feature vector might be of arbitrary size depending
            // on the feature selected. But each one could simply be a
            // time-series.
            vector<double> feature = pipeline_.getFeatureExtractionData();

            plot_features_.update(feature);
            // feature_data_.push_back(feature);
        }

        if (is_recording_) {
            training_data_.addSample(label_, data_point);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetColor(255);

    int plotX = 10;
    int plotY = 30;
    int plotW = ofGetWidth() - plotX * 2;
    int plotH = 200;
    int margin = 10;

    ofPushStyle();
    ofPushMatrix();
    {
        ofDrawBitmapString("Input:", plotX, plotY - margin);
        plot_inputs_.draw(plotX, plotY, plotW, plotH);
        plotY += plotH + 3 * margin;
    }
    ofPopStyle();
    ofPopMatrix();

    ofPushStyle();
    ofPushMatrix();
    {
        ofDrawBitmapString("PreProcessed:", plotX, plotY - margin);
        plot_pre_processed_.draw(plotX, plotY, plotW, plotH);
        plotY += plotH + 3 * margin;
    }
    ofPopStyle();
    ofPopMatrix();

    ofPushStyle();
    ofPushMatrix();
    {
        ofDrawBitmapString("Feature:", plotX, plotY - margin);
        plot_features_.draw(plotX, plotY, plotW, plotH);
        plotY += plotH + 3 * margin;
    }
    ofPopStyle();
    ofPopMatrix();
}

void ofApp::exit() {
    istream_->stop();
}

void ofApp::onDataIn(vector<double> input) {
    std::lock_guard<std::mutex> guard(input_data_mutex_);
    input_data_ = input;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key >= '0' && key <= '9') {
        is_recording_ = true;
        label_ = key - '0';
    }

    switch (key) {
        case 't':
            pipeline_.train(training_data_);
            break;
        case 's':
            istream_->start();
            break;
        case 'e':
            istream_->stop();
            input_data_.clear();
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    is_recording_ = false;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
