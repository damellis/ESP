#include "ofApp.h"

#include <algorithm>

#include "user.h"

// If the feature output dimension is larger than 32, making the visualization a
// single output will be more visual.
const uint32_t kTooManyFeaturesThreshold = 32;

class Palette {
  public:
    vector<ofColor> generate(uint32_t n) {
        // TODO(benzh) fill instead of re-generate.
        if (n > size) {
            size = n;
            do_generate(size);
        }

        std::vector<ofColor> sliced(colors.begin(), colors.begin() + n);
        return sliced;
    }

    Palette() : size(256) {
        do_generate(size);
    }
  private:
    void do_generate(uint32_t n) {
        uint32_t numDimensions = n;
        // Code snippet from ofxGrtTimeseriesPlot.cpp

        colors.resize(n);
        // Setup the default colors
        if( numDimensions >= 1 ) colors[0] = ofColor(255,0,0); //red
        if( numDimensions >= 2 ) colors[1] = ofColor(0,255,0); //green
        if( numDimensions >= 3 ) colors[2] = ofColor(0,0,255); //blue
        //Randomize the remaining colors
        for(unsigned int n=3; n<numDimensions; n++){
            colors[n][0] = ofRandom(50,255);
            colors[n][1] = ofRandom(50,255);
            colors[n][2] = ofRandom(50,255);
        }
    }

    uint32_t size;
    std::vector<ofColor> colors;
};

void ofApp::useStream(IStream &stream) {
    istream_ = &stream;
}

void ofApp::usePipeline(GRT::GestureRecognitionPipeline &pipeline) {
    pipeline_ = &pipeline;
}

//--------------------------------------------------------------
void ofApp::setup() {
    is_recording_ = false;

    // setup() is a user-defined function.
    ::setup();

    istream_->onDataReadyEvent(this, &ofApp::onDataIn);

    plot_inputs_.setup(kBufferSize_, istream_->getNumOutputDimensions(), "Input");
    plot_inputs_.setDrawGrid(true);
    plot_inputs_.setDrawInfoText(true);

    Palette color_palette;
    // Below is just proof-of-concept. The setup is quite hard-coded and we
    // should enrich this once the UI design is ready.
    size_t num_pre_processing = pipeline_->getNumPreProcessingModules();
    for (int i = 0; i < num_pre_processing; i++) {
        PreProcessing* pp = pipeline_->getPreProcessingModule(i);
        uint32_t dim = pp->getNumOutputDimensions();
        ofxGrtTimeseriesPlot plot;
        plot.setup(kBufferSize_, dim, "PreProcessing");
        plot.setDrawGrid(true);
        plot.setDrawInfoText(true);
        plot.setColorPalette(color_palette.generate(dim));
        plot_pre_processed_.push_back(plot);
    }

    size_t num_feature_modules = pipeline_->getNumFeatureExtractionModules();
    for (int i = 0; i < num_feature_modules; i++) {
        vector<ofxGrtTimeseriesPlot> feature_at_stage_i;

        FeatureExtraction* fe = pipeline_->getFeatureExtractionModule(i);
        uint32_t feature_dim = fe->getNumOutputDimensions();
        if (feature_dim < kTooManyFeaturesThreshold) {
            for (int i = 0; i < feature_dim; i++) {
                ofxGrtTimeseriesPlot plot;
                plot.setup(kBufferSize_, 1, "Feature");
                plot.setDrawInfoText(true);
                plot.setColorPalette(color_palette.generate(feature_dim));
                feature_at_stage_i.push_back(plot);
            }
        } else {
            // We will have only one here.
            ofxGrtTimeseriesPlot plot;
            plot.setup(feature_dim, 1, "Feature");
            plot.setDrawInfoText(true);
            plot.setColorPalette(color_palette.generate(feature_dim));
            feature_at_stage_i.push_back(plot);
        }

        plot_features_.push_back(feature_at_stage_i);
    }

    for (int i = 0; i < kNumMaxLabels_; i++) {
        uint32_t label_dim = istream_->getNumOutputDimensions();
        ofxGrtTimeseriesPlot plot;
        plot.setup(kBufferSize_, label_dim, "Label");
        plot.setDrawInfoText(true);
        plot.setColorPalette(color_palette.generate(label_dim));
        plot.setRanges(-1, 1, true);
        plot_samples_.push_back(plot);
        plot_samples_info_.push_back("");
    }

    training_data_.setNumDimensions(istream_->getNumOutputDimensions());
//    training_data_.setDatasetName("Audio");
//    training_data_.setInfoText("This data contains audio data");
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
    if (!pipeline_->save("pipeline.grt")) {
        ofLog(OF_LOG_ERROR) << "Failed to save the pipeline";
    }

    if (!pipeline_->getClassifier()->save("classifier.grt")) {
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
    (*pipeline_) = pipeline;
}

//--------------------------------------------------------------
void ofApp::update() {
    std::lock_guard<std::mutex> guard(input_data_mutex_);
    for (int i = 0; i < input_data_.getNumRows(); i++){
        vector<double> data_point = input_data_.getRowVector(i);

        plot_inputs_.update(data_point);

        if (istream_->hasStarted()) {
            if (!pipeline_->preProcessData(data_point)) {
                ofLog(OF_LOG_ERROR) << "ERROR: Failed to compute features!";
            }

            for (int j = 0; j < pipeline_->getNumPreProcessingModules(); j++) {
                vector<double> data = pipeline_->getPreProcessedData(j);
                plot_pre_processed_[j].update(data);
            }

            for (int j = 0; j < pipeline_->getNumFeatureExtractionModules(); j++) {
                // Working on j-th stage.
                vector<double> feature = pipeline_->getFeatureExtractionData(j);
                if (feature.size() < kTooManyFeaturesThreshold) {
                    for (int k = 0; k < feature.size(); k++) {
                        vector<double> v = { feature[k] };
                        plot_features_[j][k].update(v);
                    }
                } else {
                    assert(plot_features_.size() == 1);
                    plot_features_[j][0].setData(feature);
                }
            }
        }

        if (is_recording_) {
            sample_data_.push_back(data_point);
        }

        if (pipeline_->getTrained()) {
            pipeline_->predict(data_point);
            predicted_label_ = pipeline_->getPredictedClassLabel();
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

    for (int i = 0; i < pipeline_->getNumPreProcessingModules(); i++) {
        // working on pre-processing stage i.
        ofPushStyle();
        ofPushMatrix();
        {
            ofDrawBitmapString("PreProcessed stage: " + std::to_string(i),
                               plotX, plotY - margin);
            plot_pre_processed_[i].draw(plotX, plotY, plotW, plotH);
            plotY += plotH + 3 * margin;
        }
        ofPopStyle();
        ofPopMatrix();
    }

    for (int i = 0; i < pipeline_->getNumFeatureExtractionModules(); i++) {
        // working on feature extraction stage i.
        ofPushStyle();
        ofPushMatrix();
        {
            ofDrawBitmapString("Feature stage: " + std::to_string(i),
                               plotX, plotY - margin);
            int width = plotW / plot_features_[i].size();
            for (int j = 0; j < plot_features_[i].size(); j++) {
                plot_features_[i][j].draw(plotX + j * width, plotY, width, plotH);
            }
            plotY += plotH + 3 * margin;
        }
        ofPopStyle();
        ofPopMatrix();
    }

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
                       "`t` - train; `h` - panel",
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
                if (pipeline_->train(data_copy)) {
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
