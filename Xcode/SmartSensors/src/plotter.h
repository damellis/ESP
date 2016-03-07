#pragma once

#include "ofMain.h"

// The plotter class extends ofxGrtTimeseriesPlot and manages user-input to
// support interactive operations over time-series data.
class Plotter {
  public:
    Plotter() : initialized_(false), lock_ranges_(false), minY_(0), maxY_(0),
            x_start_(0), x_end_(0),
            is_tracking_mouse_(false), range_selected_callback_(nullptr) {
    }

    struct CallbackArgs {
        uint32_t start;
        uint32_t end;
        void* data;
    };

    bool setup(uint32_t num_dimensions, std::string title) {
        num_dimensions_ = num_dimensions;
        title_ = title;

        colors_.resize(num_dimensions_);
        // Setup the default colors_
        if (num_dimensions >= 1) colors_[0] = ofColor(255, 0, 0); // red
        if (num_dimensions >= 2) colors_[1] = ofColor(0, 255, 0); // green
        if (num_dimensions >= 3) colors_[2] = ofColor(0, 0, 255); // blue

        // Randomize the remaining colors_
        for(uint32_t n = 3; n < num_dimensions_; n++){
            colors_[n][0] = ofRandom(50, 255);
            colors_[n][1] = ofRandom(50, 255);
            colors_[n][2] = ofRandom(50, 255);
        }

        initialized_ = true;
        return true;
    }

    bool setData(const GRT::MatrixDouble& data) {
        x_start_ = 0;
        x_end_ = 0;
        data_ = data;
        return true;
    }

    bool push_back(const vector<double>& data_point) {
        data_.push_back(data_point);
        for (double d : data_point) {
            if (d > maxY_) { maxY_ = d; }
            if (d < minY_) { minY_ = d; }
        }
        return true;
    }

    GRT::MatrixDouble& getData() {
        return data_;
    }

    bool setRanges(float minY, float maxY, bool lockRanges = false) {
        default_minY_ = minY;
        default_maxY_ = maxY;
        lock_ranges_ = lockRanges;
        return true;
    }

    std::pair<float, float> getRanges() { return std::make_pair(minY_, maxY_); }

    bool setColorPalette(const vector<ofColor>& colors) {
        if (colors.size() == num_dimensions_) {
            colors_ = colors;
            return true;
        } else {
            return false;
        }
    }

    bool setTitle(const string& title) {
        title_ = title;
        return true;
    }

    bool draw(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
        x_ = x;
        y_ = y;
        w_ = w;
        h_ = h;

        if (!initialized_) return false;

        ofPushMatrix();
        ofPushStyle();

        ofEnableAlphaBlending();
        ofTranslate(x, y);

        // Draw the background
        ofFill();
        ofSetColor(0, 0, 0);
        ofDrawRectangle(0, 0, w, h);

        // Draw the selection.
        ofSetColor(0x1F, 0x1F, 0x1F);
        if (x_start_ > 0 && x_end_ > x_start_) {
            ofDrawRectangle(x_start_, 0, x_end_ - x_start_, h);
        }

        // Draw the axis lines
        ofSetColor(255, 255, 255);
        ofDrawLine(-5, h, w+5, h); // X Axis
        ofDrawLine(0, -5, 0, h+5); // Y Axis

        // Draw the timeseries
        float xPos = 0;
        x_step_ = 1.0 * w_ / data_.getNumRows();

        uint32_t index = 0;
        ofNoFill();
        for(uint32_t n = 0; n < num_dimensions_; n++){
            xPos = 0;
            index = 0;
            ofSetColor(colors_[n][0], colors_[n][1], colors_[n][2]);
            ofBeginShape();
            for(uint32_t i = 0; i < data_.getNumRows(); i++){
                float min = lock_ranges_ ? default_minY_ : minY_;
                float max = lock_ranges_ ? default_maxY_ : maxY_;
                ofVertex(xPos, ofMap(data_[i][n], min, max, h, 0, true));
                xPos += x_step_;
            }
            ofEndShape(false);
        }

        // Draw the title
        int ofBitmapFontHeight = 14;
        int textX = 10;
        int textY = ofBitmapFontHeight + 5;
        if (title_ != ""){
            ofSetColor(0xFF, 0xFF, 0xFF);
            ofDrawBitmapString(title_, textX, textY);
        }

        ofPopStyle();
        ofPopMatrix();
        return true;
    }

    bool reset() {
        if (!initialized_) return false;
        x_start_ = 0;
        x_end_ = 0;
        minY_ = 0;
        maxY_ = 0;
        data_.clear();
        return true;
    }

    typedef std::function<void(CallbackArgs)> onRangeSelectedCallback;

    void onRangeSelected(const onRangeSelectedCallback& cb, void* data) {
        range_selected_callback_ = cb;
        callback_data_ = data;
        ofAddListener(ofEvents().mousePressed, this, &Plotter::startSelection);
        ofAddListener(ofEvents().mouseDragged, this, &Plotter::duringSelection);
        ofAddListener(ofEvents().mouseReleased, this, &Plotter::endSelection);
    }

    template<typename T1, typename arg, class T>
    void onRangeSelected(T1* owner, void (T::*listenerMethod)(arg), void* data) {
        using namespace std::placeholders;
        onRangeSelected(std::bind(listenerMethod, owner, _1), data);
    }

    std::pair<uint32_t, uint32_t> getSelection() {
        return std::make_pair(x_start_ / x_step_, x_end_ / x_step_);
    }

  private:
    bool initialized_;
    uint32_t num_dimensions_;
    vector<ofColor> colors_;
    std::string title_;
    bool lock_ranges_;
    float minY_, default_minY_;
    float maxY_, default_maxY_;
    GRT::MatrixDouble data_;
    float x_step_;

    bool contains(uint32_t x, uint32_t y) {
        if (x_ <= x && x <= x_ + w_ && y_ <= y && y <= y_ + h_) {
            return true;
        } else {
            return false;
        }
    }

    void normalize() {
        pair<uint32_t, uint32_t> sel = std::minmax(x_click_, x_release_);
        x_start_ = sel.first;
        x_end_ = sel.second;
    }

    void startSelection(ofMouseEventArgs& arg) {
        // Only tracks if point is inside and data_ has rows.
        if (contains(arg.x, arg.y) && data_.getNumRows() > 0) {
            x_click_ = arg.x - x_;
            is_tracking_mouse_ = true;
        }
    }

    void duringSelection(ofMouseEventArgs& arg) {
        if (is_tracking_mouse_) {
            if (contains(arg.x, arg.y)) {
                x_release_ = arg.x - x_;
                normalize();
            }
        }
    }

    void endSelection(ofMouseEventArgs& arg) {
        if (is_tracking_mouse_) {
            if (contains(arg.x, arg.y)) {
                x_release_ = arg.x - x_;
                normalize();
            }

            if (range_selected_callback_ != nullptr) {
                CallbackArgs args {
                    .start = x_start_,
                    .end = x_end_,
                    .data = callback_data_,
                };
                range_selected_callback_(args);
            }
        }
        is_tracking_mouse_ = false;
    }

    uint32_t x_;
    uint32_t y_;
    uint32_t w_;
    uint32_t h_;

    // x_click_ and x_release_ keeps track of where the mouse is clicked and
    // releasesd. Their values are normalized by subtracting x_.
    uint32_t x_click_;
    uint32_t x_release_;

    // x_start_ and x_end_ are values that are reported to users of this class
    // for selections. When selection happens, x_start_ is always smaller than
    // x_end_.
    uint32_t x_start_;
    uint32_t x_end_;

    // To avoid accidental tracking (such as out of range or when no data).
    bool is_tracking_mouse_;

    onRangeSelectedCallback range_selected_callback_;
    void* callback_data_;
};
