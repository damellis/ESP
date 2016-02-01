#include "ofApp.h"

#include <algorithm>

#include "user.h"

//--------------------------------------------------------------
void ofApp::setup() {
    is_recording_ = false;

    istream_.reset(new AudioStream());
    istream_->onDataReadyEvent(this, &ofApp::onDataIn);

    // setupInputStream and setupPipeline are user-defined functions.
    setupInputStream(*(istream_.get()));
    setupPipeline(pipeline_);

    plot_inputs_.setup(kBufferSize_, 2, "Input");
    plot_inputs_.setDrawGrid(true);
    plot_inputs_.setDrawInfoText(true);

    // Below is just proof-of-concept. The setup is quite hard-coded and we
    // should enrich this once the UI design is ready.
    size_t num_pre_processing = pipeline_.getNumPreProcessingModules();
    if (num_pre_processing > 0) {
        PreProcessing* pp = pipeline_.getPreProcessingModule(0);
        plot_pre_processed_.setup(kBufferSize_, pp->getNumOutputDimensions(),
                                  "PreProcessing");
        plot_pre_processed_.setDrawGrid(true);
        plot_pre_processed_.setDrawInfoText(true);
    }

    size_t num_feature_modules = pipeline_.getNumFeatureExtractionModules();
    if (num_feature_modules > 0) {
        FeatureExtraction* fe = pipeline_.getFeatureExtractionModule(
            num_feature_modules - 1);
        for (int i = 0; i < fe->getNumOutputDimensions(); i++) {
            ofxGrtTimeseriesPlot plot;
            plot.setup(kBufferSize_, 1, "Feature");
            plot.setDrawInfoText(true);
            plot_features_.push_back(plot);
        }
    }

    for (int i = 0; i < kNumMaxLabels_; i++) {
        ofxGrtTimeseriesPlot plot;
        plot.setup(kBufferSize_, 1, "Label");
        plot.setDrawInfoText(true);
        plot.setRanges(-1, 1, true);
        plot_samples_.push_back(plot);
        plot_samples_info_.push_back("");
    }

    training_data_.setNumDimensions(1);
    training_data_.setDatasetName("Audio");
    training_data_.setInfoText("This data contains audio data");
    predicted_label_ = 0;

    gui_.setup("", "", ofGetWidth() - 200, 0);
    gui_hide_ = true;
    gui_.add(save_pipeline_button_.setup("Save Pipeline", 200, 30));
    gui_.add(load_pipeline_button_.setup("Load Pipeline", 200, 30));
    save_pipeline_button_.addListener(this, &ofApp::savePipeline);
    load_pipeline_button_.addListener(this, &ofApp::loadPipeline);

    ofBackground(54, 54, 54);
}

void ofApp::savePipeline() {
    if (!pipeline_.save("pipeline.grt")) {
        ofLog(OF_LOG_ERROR) << "Failed to save the pipeline";
    }
    
    if (!pipeline_.getClassifier()->save("classifier.grt")) {
        ofLog(OF_LOG_ERROR) << "Failed to save the classifier";
    }
}

void ofApp::loadPipeline() {
    GRT::GestureRecognitionPipeline pipeline;
    if (!pipeline.load("pipeline.grt")) {
        ofLog(OF_LOG_ERROR) << "Failed to load the pipeline";
    }

    // TODO(benzh) Compare the two pipelines and warn the user if the
    // loaded one is different from his.
    pipeline_ = pipeline;
}

//--------------------------------------------------------------
void ofApp::update() {
    std::lock_guard<std::mutex> guard(input_data_mutex_);
    for (int i = 0; i < input_data_.getNumRows(); i++){
        vector<double> data_point = input_data_.getRowVector(i);
        
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

            for (int i = 0; i < feature.size(); i++) {
                vector<double> v = { feature[i] };
                plot_features_[i].update(v);
            }
        }

        if (is_recording_) {
            sample_data_.push_back(data_point);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetColor(255);

    int plotX = 10;
    int plotY = 30;
    int plotW = ofGetWidth() - plotX * 2;
    int plotH = 150;
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
    ofDrawBitmapString("Feature:", plotX, plotY - margin);

    if (plot_features_.size() > 0) {
        int width = plotW / plot_features_.size();
        for (int i = 0; i < plot_features_.size(); i++) {
            plot_features_[i].draw(plotX + i * width, plotY, width, plotH);
        }
    }

    plotY += plotH + 3 * margin;

    ofPopStyle();
    ofPopMatrix();

    // Training samples management
    ofPushStyle();
    ofPushMatrix();
    ofDrawBitmapString("Training Samples:", plotX, plotY - margin);

    // Currently we support kNumMaxLabels_ labels
    int width = plotW / kNumMaxLabels_;
    for (int i = 0; i < kNumMaxLabels_; i++) {
        int x = plotX + i * width;
        plot_samples_[i].draw(x, plotY, width, plotH - 3 * margin);
        ofDrawBitmapString(plot_samples_info_[i], x, plotY + plotH - margin);

    }

    plotY += plotH + 3 * margin;

    ofPopStyle();
    ofPopMatrix();

    // Instructions
    ofPushStyle();
    ofPushMatrix();

    ofDrawBitmapString(std::to_string(sample_data_.getNumRows()) +
                       " data points\t" +
                       "label: " + std::to_string(predicted_label_),
                       plotX, plotY - 2.5 * margin);

    ofDrawBitmapString("Instructions: "
                       "`s` - start; `e` - pause; 1-9 training samples;"
                       "`t` - train; `p` - predict; `h` - panel",
                       plotX, plotY - margin);

    ofPopStyle();
    ofPopMatrix();

    if (!gui_hide_) {
        gui_.draw();
    }
}

void ofApp::exit() {
    if (training_thread_.joinable()) {
        training_thread_.join();
    }
    istream_->stop();

    // Clear all listeners.
    save_pipeline_button_.removeListener(this, &ofApp::savePipeline);
    load_pipeline_button_.removeListener(this, &ofApp::loadPipeline);
}

void ofApp::onDataIn(GRT::MatrixDouble input) {
    std::lock_guard<std::mutex> guard(input_data_mutex_);
    input_data_ = input;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key >= '1' && key <= '9') {
        if (!is_recording_) {
            is_recording_ = true;
            label_ = key - '0';
            sample_data_.clear();
        }
    }

    switch (key) {
        case 't': {
            // If prior training has not finished, we wait.
            if (training_thread_.joinable()) {
                training_thread_.join();
            }

            GRT::ClassificationData data_copy = training_data_;
            auto training_func = [this, data_copy]() mutable {
                ofLog() << "Training started";
                if (pipeline_.train(data_copy)) {
                    ofLog() << "Training is successful";
                } else {
                    ofLog() << "Failed to train the model";
                }
            };
            training_thread_ = std::thread(training_func);
            break;
        }

        case 'h':
            gui_hide_ = !gui_hide_;
        case 's':
            istream_->start();
            break;
        case 'e':
            istream_->stop();
            input_data_.clear();
            break;
        case 'p':
            sample_data_.clear();
            is_recording_ = true;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    is_recording_ = false;
    if (key >= '1' && key <= '9') {
        for (int i = 0; i < sample_data_.getNumRows(); i++) {
            training_data_.addSample(label_, sample_data_.getRowVector(i));
        }

        plot_samples_[label_ - 1].setData(sample_data_);
        plot_samples_info_[label_ - 1] =
                std::to_string(sample_data_.getNumRows()) + " points";

    } else if (key == 'p') {
        for (int i = 0; i < sample_data_.getNumRows(); i++) {
            pipeline_.predict(sample_data_.getRowVector(i));
        }
        predicted_label_ = pipeline_.getPredictedClassLabel();
    }
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
