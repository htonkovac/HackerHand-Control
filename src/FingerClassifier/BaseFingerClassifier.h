//
//  AbstractFingerClassifier.h
//  FingersReadings
//
//  Created by Roy Macdonald on 27-05-15.
//
//

#pragma once

#include "ofMain.h"
#include "Fingers.h"
#include "peakData.h"
#include "BYBGui.h"


class BaseFingersClassifier {
public:

	virtual void setup(string language, shared_ptr<BYBGui> g);
	virtual void save(string filename) const;
    virtual void load(string filename);
    virtual void updateCalibration(vector<vector<float> >& data, vector<float>& loPass, peakData& p);
	virtual void updatePeakCalibration(peakData& p);
	virtual unsigned int classify(const vector<float>& sample)=0;
	virtual unsigned int getPrimaryFinger() const =0;
    virtual	void update(vector<vector<float> >& data, vector<float>& loPass, peakData& p);
	
	virtual unsigned int size() const;
    virtual Fingers& getFingers(unsigned int i);
	virtual void setFinger(string description = "", int fingerID = -1);
	virtual void addSample(const vector<float>& sample, int index)=0;
	virtual void reset();
	virtual void startCalibration();
	virtual void endCalibration();
	
    ofEvent<int> fingerMoved, fingerReleased;
	
    string getName(){return name;}
    
	virtual bool isCalibrating(){return bCalibrating;}

    virtual void copyCalibratioFrom(BaseFingersClassifier* orig);
    
    vector<int> getClassifyHist();
    
    string name;
protected:

	vector<Fingers> fingers;

	bool bCalibrating;
	bool bWaitForPeak;
	bool bWaitForAverage;

    
	shared_ptr<BYBGui> gui;
	int currentSampleNum, numSamplesAverage;
    vector<float>maxs, mins;
    float maxAll;
    
protected:
       vector<int> classifyHist;
    unsigned long long prevPeakTime;
    vector<float> average, partialAvg;
    int currentAverageSample;
};
