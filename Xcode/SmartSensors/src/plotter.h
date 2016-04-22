#pragma once

#include "ofMain.h"
#include "ofxGRT.h"

using std::string;

// The plotter class extends ofxGrtTimeseriesPlot and manages user-input to
// support interactive operations over time-series data.
class Plotter {
  public:
    Plotter();

    struct CallbackArgs {
        uint32_t start;
        uint32_t end;
        void* data;
    };

    bool setup(uint32_t num_dimensions, string title, string subtitle = "");
    bool setData(const GRT::MatrixDouble& data);

    bool clearContentModifiedFlag();

    bool push_back(const vector<double>& data_point);

    GRT::MatrixDouble& getData() { return data_; }
    bool setRanges(float minY, float maxY, bool lockRanges = false);

    std::pair<float, float> getRanges();
    bool setColorPalette(const vector<ofColor>& colors);

    bool setTitle(const string& title);

    const string& getTitle() const;

    bool draw(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

    bool reset();

    bool clearData();

    typedef std::function<void(CallbackArgs)> onRangeSelectedCallback;

    void onRangeSelected(const onRangeSelectedCallback& cb, void* data);

    template<typename T1, typename arg, class T>
    void onRangeSelected(T1* owner, void (T::*listenerMethod)(arg), void* data) {
        using namespace std::placeholders;
        onRangeSelected(std::bind(listenerMethod, owner, _1), data);
    }

    std::pair<uint32_t, uint32_t> getSelection();

  private:
    bool initialized_;
    bool is_content_modified_;
    uint32_t num_dimensions_;
    vector<ofColor> colors_;
    string title_;
    string subtitle_;
    bool lock_ranges_;
    float minY_, default_minY_;
    float maxY_, default_maxY_;
    GRT::MatrixDouble data_;
    float x_step_;

    bool contains(uint32_t x, uint32_t y);
    void normalize();
    void startSelection(ofMouseEventArgs& arg);
    void duringSelection(ofMouseEventArgs& arg);
    void endSelection(ofMouseEventArgs& arg);

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
