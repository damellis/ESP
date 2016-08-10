#include "tuneable.h"

#include <cmath>

#include "ofApp.h"

static std::map<void*, Tuneable*> allTuneables;

void Tuneable::onSliderEvent(ofxDatGuiSliderEvent e) {
    for (const auto& t : allTuneables) {
        void* data_ptr = t.second->getDataAddress();
        void* ui_ptr = t.second->getUIAddress();
        if (e.target == ui_ptr) {
            if (t.second->getType() == Tuneable::INT_RANGE) {
                int* value = static_cast<int*>(data_ptr);
                int set_value = std::round(e.value);
                *value = set_value;

                // Because slider only supports double, we have to manually
                // round it to match integer semantics.
                e.target->setValue(set_value);

                if (t.second->int_cb_ != nullptr) {
                    t.second->int_cb_(*value);
                } else {
                    ((ofApp *) ofGetAppPtr())->reloadPipelineModules();
                }
            } else {
                double* value = static_cast<double*>(data_ptr);
                *value = e.value;

                if (t.second->double_cb_ != nullptr) {
                    t.second->double_cb_(*value);
                } else {
                    ((ofApp *) ofGetAppPtr())->reloadPipelineModules();
                }
            }
        }
    }
}

void Tuneable::onToggleEvent(ofxDatGuiButtonEvent e) {
    for (const auto& t : allTuneables) {
        void* data_ptr = t.second->getDataAddress();
        void* ui_ptr = t.second->getUIAddress();
        if (e.target == ui_ptr) {
            bool* value = static_cast<bool*>(data_ptr);
            *value = e.enabled;
            ((ofApp *) ofGetAppPtr())->reloadPipelineModules();

            if (t.second->bool_cb_ != nullptr) {
                t.second->bool_cb_(*value);
            } else {
                ((ofApp *) ofGetAppPtr())->reloadPipelineModules();
            }
        }
    }
}

void registerTuneable(int& value, int min, int max,
                      const string& title,
                      const string& description,
                      std::function<void(int)> cb) {
    void* address = &value;
    if (allTuneables.find(address) != allTuneables.end()) {
        return;
    }

    Tuneable* t = new Tuneable(&value, min, max, title, description, cb);
    allTuneables[address] = t;
    ((ofApp *) ofGetAppPtr())->registerTuneable(t);
}

void registerTuneable(double& value, double min, double max,
                      const string& title,
                      const string& description,
                      std::function<void(double)> cb) {
    void* address = &value;
    if (allTuneables.find(address) != allTuneables.end()) {
        return;
    }

    Tuneable* t = new Tuneable(&value, min, max, title, description, cb);
    allTuneables[address] = t;
    ((ofApp *) ofGetAppPtr())->registerTuneable(t);
}

void registerTuneable(bool& value,
                      const string& title,
                      const string& description,
                      std::function<void(bool)> cb) {
    void* address = &value;
    if (allTuneables.find(address) != allTuneables.end()) {
        return;
    }

    Tuneable* t = new Tuneable(&value, title, description, cb);
    allTuneables[address] = t;
    ((ofApp *) ofGetAppPtr())->registerTuneable(t);
}
