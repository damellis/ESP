#pragma once

#include "ofxGrt.h"

// The plotter class extends ofxGrtTimeseriesPlot and manages user-input to
// support interactive operations over time-series data.
class Plotter : public ofxGrtTimeseriesPlot {
  public:
    Plotter() : x_start_(0), x_end_(0), range_selected_callback_(nullptr) {
    }

    bool draw(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
        x_ = x;
        y_ = y;
        w_ = w;
        h_ = h;
        ofxGrtTimeseriesPlot::draw(x, y, w, h);
    }

    struct CallbackArgs {
        uint32_t start;
        uint32_t end;
        void* data;
    };

    typedef std::function<void(CallbackArgs)> onRangeSelectedCallback;

    void onRangeSelected(const onRangeSelectedCallback& cb, void* data) {
        range_selected_callback_ = cb;
        callback_data_ = data;
        ofAddListener(ofEvents().mousePressed, this, &Plotter::startSelection);
        ofAddListener(ofEvents().mouseReleased, this, &Plotter::endSelection);
    }

    template<typename T1, typename arg, class T>
    void onRangeSelected(T1* owner, void (T::*listenerMethod)(arg), void* data) {
        using namespace std::placeholders;
        onRangeSelected(std::bind(listenerMethod, owner, _1), data);
    }

  private:
    void startSelection(ofMouseEventArgs& arg) {
        if (x_ <= arg.x && arg.x <= x_ + w_ &&
            y_ <= arg.y && arg.y <= y_ + h_) {
            x_start_ = arg.x;
        }
    }

    void endSelection(ofMouseEventArgs& arg) {
        if (x_ <= arg.x && arg.x <= x_ + w_
            && y_ <= arg.y && arg.y <= y_ + h_) {
            x_end_ = arg.x;
            ofLog() << "Area selected: [" << x_start_ << ", " << x_end_ << "]";
            if (range_selected_callback_ != nullptr) {
                CallbackArgs args {
                    .start = x_start_,
                    .end = x_end_,
                    .data = callback_data_,
                };
                range_selected_callback_(args);
            }
        }
    }

    uint32_t x_;
    uint32_t y_;
    uint32_t w_;
    uint32_t h_;
    uint32_t x_start_;
    uint32_t x_end_;

    onRangeSelectedCallback range_selected_callback_;
    void* callback_data_;
};
