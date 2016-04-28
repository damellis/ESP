#pragma once

#include <string>

#include "ofxDatGui.h"

using std::string;

class Tuneable {
  public:
    // Set is not implemented, yet.
    enum Type { SET, INT_RANGE, DOUBLE_RANGE, BOOL };

    // Range tuneable (int)
    Tuneable(int* value, int min, int max, const string& title, const string& description)
            : value_ptr_(value), ui_ptr_(NULL),
              type_(INT_RANGE), title_(title), description_(description),
              min_(min), max_(max) {}

    // Range tuneable (double)
    Tuneable(double* value, double min, double max,
             const string& title, const string& description)
            : value_ptr_(value), ui_ptr_(NULL),
              type_(DOUBLE_RANGE), title_(title), description_(description),
              min_(min), max_(max) {}

    // Boolean tuneable
    Tuneable(bool* value, const string& title, const string& description)
            : value_ptr_(value), ui_ptr_(NULL),
              type_(BOOL), title_(title), description_(description) {}

    void addToGUI(ofxDatGui& gui) {
        switch (type_) {
          case INT_RANGE: {
            int* value = static_cast<int*>(value_ptr_);
            ofxDatGuiSlider *slider = gui.addSlider(title_, min_, max_);
            slider->setValue(*value);
            ui_ptr_ = static_cast<void*>(slider);
            gui.onSliderEvent(this, &Tuneable::onSliderEvent);
            gui.addTextBlock(description_);
            break;
          }

          case DOUBLE_RANGE: {
            double* value = static_cast<double*>(value_ptr_);
            ofxDatGuiSlider *slider = gui.addSlider(title_, min_, max_);
            slider->setValue(*value);
            ui_ptr_ = static_cast<void*>(slider);
            gui.onSliderEvent(this, &Tuneable::onSliderEvent);
            gui.addTextBlock(description_);
            break;
          }

          case BOOL: {
            bool* value = static_cast<bool*>(value_ptr_);
            ofxDatGuiToggle *toggle = gui.addToggle(title_, *value);
            ui_ptr_ = static_cast<void*>(toggle);
            gui.onButtonEvent(this, &Tuneable::onToggleEvent);
            gui.addTextBlock(description_);
            break;
          }

          default: break;
        }
    }

    std::string toString() {
        switch (type_) {
          case INT_RANGE: {
            int* value = static_cast<int*>(value_ptr_);
            return "INT " + std::to_string(*value);
          }
          case DOUBLE_RANGE: {
            double* value = static_cast<double*>(value_ptr_);
            return "DOUBLE " + std::to_string(*value);
          }
          case BOOL: {
            bool* value = static_cast<bool*>(value_ptr_);
            return std::string("BOOL ") + (*value ? "true" : "false");
          }
          default: {
            ofLog(OF_LOG_ERROR) << "Unknown type";
            break;
          }
        }
        return "";
    }

    // Return value indicates success or not.
    bool fromString(std::string str) {
        std::istringstream iss(str);
        std::string word;

        iss >> word;
        if (word == "INT") {
            int* p = static_cast<int*>(value_ptr_);
            iss >> *p;
            ofxDatGuiSlider *slider = static_cast<ofxDatGuiSlider*>(ui_ptr_);
            slider->setValue(*p);
        } else if (word == "DOUBLE") {
            double* p = static_cast<double*>(value_ptr_);
            iss >> *p;
            ofxDatGuiSlider *slider = static_cast<ofxDatGuiSlider*>(ui_ptr_);
            slider->setValue(*p);
        } else if (word == "BOOL") {
            bool* p = static_cast<bool*>(value_ptr_);
            iss >> word;
            *p = (word == "true") ? true : false;
            ofxDatGuiToggle *toggle = static_cast<ofxDatGuiToggle*>(ui_ptr_);
            toggle->setEnabled(*p);
        }

        return false;
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
    Type type_;

    string title_;
    string description_;
    double min_;
    double max_;
};

void registerTuneable(int& value, int min, int max,
                      const string& name, const string& description);

void registerTuneable(double& value, double min, double max,
                      const string& name, const string& description);

void registerTuneable(bool& value, const string& name, const string& description);
