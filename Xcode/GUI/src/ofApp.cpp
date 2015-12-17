#include "ofApp.h"
#include "GRT/GRT.h"

using namespace GRT;

#include "user.h"

GestureRecognitionPipeline pipeline;

ofColor indexToColor(int i, int n) {
	return ofColor::fromHsb(255 / n * i, 255, 255);
}

//--------------------------------------------------------------
void ofApp::setup(){
	serial.listDevices();
	serial.setup(0, 115200);
	setupPipeline();
	trainingData.setNumDimensions(DIM);
}

//--------------------------------------------------------------
void ofApp::update(){
	while (serial.available() > 0) {
		int c = serial.readByte();
		if (c == OF_SERIAL_ERROR) {
			ofLog(OF_LOG_WARNING, "Error reading from serial port.");
			return;
		}
		if (c == OF_SERIAL_NO_DATA) {
			ofLog(OF_LOG_WARNING, "No data from serial port.");
			return;
		}
		serialBuffer.push_back(c);
		if (serialBuffer.find_first_of('\n') != string::npos) {
			serialBuffer.erase(serialBuffer.find_last_not_of(" \n\r\t")+1);
			istringstream iss(serialBuffer);
			vector<double> point;
			
			while (!iss.eof()) {
				double d;
				iss >> d;
				if (!iss) {
					ofLog(OF_LOG_WARNING, "Invalid data from serial port: '%s'.", serialBuffer.c_str());
					serialBuffer.clear();
					return;
				}
				point.push_back(d);
			};
			
			serialBuffer.clear();
			
			if (point.size() != DIM) {
				ofLog(OF_LOG_WARNING, "Data point dimension not equal to %d: '%s'.", DIM, serialBuffer.c_str());
				return;
			}
			
			incomingData.push_back(point);
			
			pipeline.preProcessData(point, true);
			featureData.push_back(pipeline.getFeatureExtractionData());
			
			if (recording) trainingData.addSample(trainingClassLabel, point);
			
			if (pipeline.getTrained()) {
				pipeline.predict(point);
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	int trainingHeight = 250;
	ofBackground(0);
	for (int i = incomingData.size() - 1; i > 0 && i > incomingData.size() - (ofGetHeight() - trainingHeight); i--) {
		for (int j = 0; j < incomingData[i].size(); j++) {
			int val1 = incomingData[i - 1][j], val2 = incomingData[i][j];
			val1 = ofMap(val1, 0, 1023, 0, ofGetWidth() / 3);
			val2 = ofMap(val2, 0, 1023, 0, ofGetWidth() / 3);
			ofSetColor(indexToColor(j, incomingData[i].size()));
			ofDrawLine(val1, ofGetHeight() - incomingData.size() + i - 1 - trainingHeight, val2, ofGetHeight() - incomingData.size() + i - trainingHeight);
		}
		ofSetColor(255, 255, 255);
		ofDrawLine(ofGetWidth() / 3, 0, ofGetWidth() / 3, ofGetHeight());
		for (int j = 0; j < featureData[i].size(); j++) {
			int val1 = featureData[i - 1][j], val2 = featureData[i][j];
			val1 = ofMap(val1, 0, 1023, 0, ofGetWidth() / 3);
			val2 = ofMap(val2, 0, 1023, 0, ofGetWidth() / 3);
			ofSetColor(indexToColor(j, featureData[i].size()));
			ofDrawLine(ofGetWidth() / 3 + val1, ofGetHeight() - featureData.size() + i - 1 - trainingHeight, ofGetWidth() / 3 + val2, ofGetHeight() - featureData.size() + i - trainingHeight);
		}
	}
	MatrixDouble means = trainingData.getClassMean();
	MatrixDouble stddevs = trainingData.getClassStdDev();
	for (int i = 0; i < means.getNumRows(); i++) {
		ofSetColor(255, 255, 255);
		ofDrawLine(0, ofGetHeight() - trainingHeight + (i * means.getNumCols()) * 20, ofGetWidth() / 3, ofGetHeight() - trainingHeight + (i * means.getNumCols()) * 20);
		for (int j = 0; j < means.getNumCols(); j++) {
			int mean = ofMap(means[i][j], 0, 1023, 0, ofGetWidth() / 3);
			int stddev = ofMap(stddevs[i][j], 0, 1023, 0, ofGetWidth() / 3);
			ofSetColor(indexToColor(j, means.getNumCols()));
			ofDrawLine(mean - stddev, ofGetHeight() - trainingHeight + (i * means.getNumCols() + j) * 20 + 10, mean + stddev, ofGetHeight() - trainingHeight + (i * means.getNumCols() + j) * 20 + 10);
			ofDrawLine(mean, ofGetHeight() - trainingHeight + (i * means.getNumCols() + j) * 20 + 5, mean, ofGetHeight() - trainingHeight + (i * means.getNumCols() + j) * 20 + 15);
		}
	}
	if (recording) {
		ofSetColor(255, 255, 255);
		ofDrawBitmapString(string("Class ") + ofToString(trainingClassLabel), ofGetWidth() * 2 / 3, 20);
	}
	if (pipeline.getTrained()) {
		ofSetColor(255, 255, 255);
		ofDrawBitmapString(string("Class ") + ofToString(pipeline.getPredictedClassLabel()), ofGetWidth() * 2 / 3, 20);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key >= '0' && key <= '9') {
		recording = true;
		trainingClassLabel = key - '0';
	}
	
	if (key == 't') pipeline.train(trainingData);
	
	if (key == 's') trainingData.saveDatasetToFile("data.txt");
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key >= '0' && key <= '9') {
		recording = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
