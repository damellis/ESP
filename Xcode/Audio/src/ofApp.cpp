#include "ofApp.h"

#include <algorithm>
#include <math.h>

#include "user.h"

//--------------------------------------------------------------
void ofApp::setup() {
    buffer_size_ = 256;
    is_recording_ = false;

    istream_.reset(new IStream());
    vector<string> inputs = istream_->getIStreamList();
    istream_->onDataReadyEvent(this, &ofApp::onDataIn);
    // For now, use whichever will always use built-in audio.
    istream_->useIStream(0);

    gui_.reset(new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT));
    dropdown_input_ = gui_->addDropdown("select input device", inputs);
    gui_->onDropdownEvent(this, &ofApp::onDropdownSelected);

    plot_input_.setup(10 * buffer_size_, 1, "Input");
    plot_input_.setDrawGrid(true);
    plot_input_.setDrawInfoText(true);

    plot_filtered_.setup(10 * buffer_size_, 1, "Filtered");
    plot_filtered_.setDrawGrid(true);
    plot_filtered_.setDrawInfoText(true);

    training_data_.setNumDimensions(1);
    training_data_.setDatasetName("Audio");
    training_data_.setInfoText("This data contains audio data");

    pipeline_ = setupPipeline();

    ofBackground(54, 54, 54);
}

//--------------------------------------------------------------
void ofApp::update() {
    for (int i = 0; i < input_data_.size(); i++){
        vector<double> data_point = {
            input_data_[i]
        };
        plot_input_.update(data_point);

        if (istream_->hasStarted()) {
            pipeline_.preProcessData(data_point);
            vector<double> feature = pipeline_.getFeatureExtractionData();
            feature_data_.push_back(feature);

            if (is_recording_) {
                training_data_.addSample(label_, data_point);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetColor(255);

    int plotX = 20;
    int plotY = 100;
    int plotW = ofGetWidth() - plotX * 2;
    int plotH = 200;

    int margin = 10;

    ofPushStyle();
    ofPushMatrix();
    {
        ofDrawBitmapString("Input:", plotX, plotY - margin);
        plot_input_.draw(plotX, plotY, plotW, plotH);
        plotY += plotH + 3 * margin;
    }
    ofPopStyle();
    ofPopMatrix();

    ofPushStyle();
    ofPushMatrix();
    {
        ofDrawBitmapString("Filtered:", plotX, plotY - margin);
        plot_filtered_.draw(plotX, plotY, plotW, plotH);
        plotY += plotH + 3 * margin;
    }
    ofPopStyle();
    ofPopMatrix();
}

void ofApp::exit() {
    istream_->stop();
}

void ofApp::onDropdownSelected(ofxDatGuiDropdownEvent e) {
    if (e.target == dropdown_input_) {
        istream_->useIStream(e.child);
        return;
    }

    ofLog(OF_LOG_ERROR) << "Unknown Dropdown selection event";
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
