#pragma once

#include "ofMain.h"
#include "FingersClassifierSimple.h"
#include "FingerClassifierSVM.h"
#include "serialManager.h"

class BYBGui;

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
		
	void loadFingerProfile();
	void saveFingerProfile();

	void startCalibration();
	
	const vector<vector<float> >& getOriginalData(){return originalData;}
	vector<vector<float> > originalData;
	
	void newSerialData(vector<unsigned int> & d);
	
	//FingersClassifierSimple classifier;
    FingersClassifierSVM classifier;
	
	serialManager serial;

	shared_ptr<BYBGui> gui;
	
};

