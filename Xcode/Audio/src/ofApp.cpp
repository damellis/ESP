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

ofApp::ofApp() : fragment_(PIPELINE), num_pipeline_stages_(0) {
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

    // Parse the user supplied pipeline and extract information:
    //  o num_pipeline_stages_

    // 1. Parse pre-processing.
    uint32_t num_pre_processing = pipeline_->getNumPreProcessingModules();
    num_pipeline_stages_ += num_pre_processing;
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

    // 2. Parse pre-processing.
    uint32_t num_feature_modules = pipeline_->getNumFeatureExtractionModules();
    num_pipeline_stages_ += num_feature_modules;
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
            plot.setDrawGrid(true);
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

void ofDrawColoredBitmapString(ofColor color,
                               const string& text,
                               float x, float y) {
    ofPushStyle();
    ofSetColor(color);
    ofDrawBitmapString(text, x, y);
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::draw() {
    // Hacky panel on the top.
    const uint32_t left_margin = 10;
    const uint32_t top_margin = 20;

    ofDrawBitmapString("[P]ipeline\t[T]raining\t[D]ata",
                       left_margin, top_margin);
    switch (fragment_) {
        case PIPELINE:
            ofDrawColoredBitmapString(ofColor(0xFF, 0, 0), "[P]ipeline\t",
                                      left_margin, top_margin);
            drawLivePipeline();
            break;
        case TRAINING:
            ofDrawColoredBitmapString(ofColor(0xFF, 0, 0), "\t\t[T]raining",
                                      left_margin, top_margin);
            drawTrainingInfo();
            break;
        case DATA:
            ofDrawColoredBitmapString(ofColor(0xFF, 0, 0), "\t\t\t\t[D]ata",
                                      left_margin, top_margin);
            ofLog() << "Not Implemented";
            break;
        default:
            ofLog(OF_LOG_ERROR) << "Unknown tag!";
            break;
    }
}

void ofApp::drawLivePipeline() {
    // A Pipeline was parsed in the ofApp::setup function and here we simple
    // draw the pipeline information.
    uint32_t margin = 30;
    uint32_t stage_left = 10;
    uint32_t stage_top = 40;
    uint32_t stage_height =
            (ofGetHeight() - 70) / (num_pipeline_stages_ + 2) - margin;
    uint32_t stage_width = ofGetWidth() - margin;

    // 0. Setup and instructions.
    ofDrawBitmapString("Visualization generated based on your pipeline.\n"
                       "Press 1-9 to record samples and `t` to train a model.",
                       stage_left, stage_top);
    stage_top += margin;

    // 1. Draw Input.
    ofPushStyle();
    plot_inputs_.draw(stage_left, stage_top, stage_width, stage_height);
    ofPopStyle();
    stage_top += stage_height + margin;

    // 2. Draw pre-processing: iterate all stages.
    for (int i = 0; i < pipeline_->getNumPreProcessingModules(); i++) {
        // working on pre-processing stage i.
        ofPushStyle();
        plot_pre_processed_[i].
                draw(stage_left, stage_top, stage_width, stage_height);
        ofPopStyle();
        stage_top += stage_height + margin;
    }

    // 3. Draw features.
    for (int i = 0; i < pipeline_->getNumFeatureExtractionModules(); i++) {
        // working on feature extraction stage i.
        ofPushStyle();
        uint32_t width = stage_width / plot_features_[i].size();
        for (int j = 0; j < plot_features_[i].size(); j++) {
            plot_features_[i][j].
                    draw(stage_left + j * width, stage_top,
                         width, stage_height);
        }
        ofPopStyle();
        stage_top += stage_height + margin;
    }

    // 4. Draw samples
    // Currently we support kNumMaxLabels_ labels
    uint32_t width = stage_width / kNumMaxLabels_;
    for (int i = 0; i < kNumMaxLabels_; i++) {
        int label_x = stage_left + i * width;
        ofPushStyle();
        // Use input's range as a heuristic for the drawing.
        std::pair<float, float> ranges = plot_inputs_.getRanges();
        plot_samples_[i].setRanges(ranges.first, ranges.second);
        plot_samples_[i].draw(label_x, stage_top, width, stage_height);
        ofPopStyle();
        ofDrawBitmapString(plot_samples_info_[i], label_x,
                           stage_top + stage_height + 20);
    }
}

void ofApp::drawTrainingInfo() {
    // Write training summary
    // Draw input
    // Draw live prediction
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
            auto training_func = [this, data_copy]() -> bool {
                ofLog() << "Training started";
                if (pipeline_->train(data_copy)) {
                    ofLog() << "Training is successful";
                    return true;
                } else {
                    ofLog(OF_LOG_ERROR) << "Failed to train the model";
                    return false;
                }
            };

            // TODO(benzh) Fix data race issue later.
            if (training_func()) {
                fragment_ = TRAINING;
            }

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

        case 'P':
            fragment_ = PIPELINE;
            break;
        case 'T':
            fragment_ = TRAINING;
            break;
        case 'D':
            fragment_ = DATA;
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
