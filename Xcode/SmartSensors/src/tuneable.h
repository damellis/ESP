#pragma once

#include <string>

#include "ofxDatGui.h"

using std::string;

class Tuneable {
  public:
    enum Type { SET, INT_RANGE, DOUBLE_RANGE, BOOL };

    // Range tuneable (int)
    Tuneable(int* value, int min, int max, const string& description)
            : value_ptr_(value), ui_ptr_(NULL),
              type_(INT_RANGE), description_(description),
              min_(min), max_(max) {}

    // Range tuneable (double)
    Tuneable(double* value, double min, double max, const string& description)
            : value_ptr_(value), ui_ptr_(NULL),
              type_(DOUBLE_RANGE), description_(description),
              min_(min), max_(max) {}

    // Boolean tuneable
    Tuneable(bool* value, const string& description)
            : value_ptr_(value), ui_ptr_(NULL),
              type_(BOOL), description_(description) {}

    void addToGUI(ofxDatGui& gui) {
        switch (type_) {
          case INT_RANGE: {
            int* value = static_cast<int*>(value_ptr_);
            ofxDatGuiSlider *slider = gui.addSlider(description_, min_, max_);
            slider->setValue(*value);
            ui_ptr_ = static_cast<void*>(slider);
            gui.onSliderEvent(this, &Tuneable::onSliderEvent);
            break;
          }

          case DOUBLE_RANGE: {
            double* value = static_cast<double*>(value_ptr_);
            ofxDatGuiSlider *slider = gui.addSlider(description_, min_, max_);
            slider->setValue(*value);
            ui_ptr_ = static_cast<void*>(slider);
            gui.onSliderEvent(this, &Tuneable::onSliderEvent);
            break;
          }

          case BOOL: {
            bool* value = static_cast<bool*>(value_ptr_);
            ofxDatGuiToggle *toggle = gui.addToggle(description_, *value);
            ui_ptr_ = static_cast<void*>(toggle);
            gui.onButtonEvent(this, &Tuneable::onToggleEvent);
            break;
          }

          default: break;
        }
    }

    void* getUIAddress() const {
        return ui_ptr_;
    }

    void* getDataAddress() const {
        return value_ptr_;
    }

    Type getType() const {
        return type_;
    }
  private:
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onToggleEvent(ofxDatGuiButtonEvent e);

    void* value_ptr_;
    void* ui_ptr_;

    string description_;
    Type type_;
    double min_;
    double max_;
};

void registerTuneable(int& value, int min, int max, const string& description);
void registerTuneable(double& value, int min, int max, const string& description);
void registerTuneable(bool& value, const string& description);
