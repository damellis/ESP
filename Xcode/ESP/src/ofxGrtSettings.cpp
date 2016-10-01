//
//  ofxGrtSettings.h
//
//  Created by luca lolli on 11/08/2016.
// Updated: nickgillian: 26/09/2016
//
#include "ofxGrtSettings.h"

//We need to declare our static non-const variable after being definid in the header
std::shared_ptr<ofxGrtSettings::variables> ofxGrtSettings::mVariables;

ofTrueTypeFont ofxGrtSettings::variables::fontSmall;
ofTrueTypeFont ofxGrtSettings::variables::fontNormal;
ofTrueTypeFont ofxGrtSettings::variables::fontLarge;
std::string ofxGrtSettings::variables::fontFile = "verdana.ttf";
ofColor ofxGrtSettings::variables::activeTextColor = {255,255,255};
ofColor ofxGrtSettings::variables::gridColor = {64,64,64};
ofColor ofxGrtSettings::variables::axisColor = {192,192,192};
ofColor ofxGrtSettings::variables::backgroundColor = {0,0,0};
ofColor ofxGrtSettings::variables::disabledTextColor = {128,128,128};
int ofxGrtSettings::variables::titleTextSpacer = -12;
int ofxGrtSettings::variables::axisTicksSize = 5;
int ofxGrtSettings::variables::info_margin = 15;

