#pragma once

#include "ofMain.h"
#include "GRT/GRT.h"

using namespace GRT;

extern GestureRecognitionPipeline pipeline;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	
		ClassificationData trainingData;
		int trainingClassLabel;
		bool recording;
	
		ofSerial serial;
		string serialBuffer;
	
		vector<vector<double>> incomingData;
		vector<vector<double>> featureData;
};
