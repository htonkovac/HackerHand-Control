#include "ofApp.h"
#include "ofxModifierKeys.h"
#include "BYBGui.h"
#include "loPass.h"
//--------------------------------------------------------------
void ofApp::setup(){
    currentClassifier = NULL;
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofXml xml;
	string lang = "en";
	if (xml.load(ofToDataPath("lang.xml"))) {
		if(xml.exists("//language")){
			lang = xml.getValue("//language");
		}else cout << "lang.xml error" << endl;
	}

	gui->setup(lang);
	classifierSVM.setup(lang, gui);
	classifierEuc.setup(lang, gui);
    originalData.resize(NUM_GRAPHS);
    loPassData.resize(NUM_GRAPHS);
	for (int i = 0; i < NUM_GRAPHS; i++) {
		originalData[i].resize(ofGetWidth());
        loPassData[i].resize(ofGetWidth());
	}
	serial.setup();
	
	ofAddListener(serial.newDataEvent, this, &ofApp::newSerialData);
    setClassifier(0);
  /*  ofAddListener(classifier.fingerMoved, &serial, &serialManager::moveFinger);
    ofAddListener(classifier.fingerReleased, &serial, &serialManager::releaseFinger);
    ofAddListener(classifier.fingerMoved, gui.get(), &BYBGui::moveFinger);
    ofAddListener(classifier.fingerReleased, gui.get(), &BYBGui::releaseFinger);
    //*/

}
//--------------------------------------------------------------
void ofApp::setClassifier(int i){
    BaseFingersClassifier * nextClass = NULL;
    switch (i) {
        case 0:
            nextClass = &classifierSVM;
            break;
        case 1:
            nextClass = &classifierEuc;
            break;
    }
    if (nextClass) {
    if (currentClassifier) {
        ofRemoveListener(currentClassifier->fingerMoved, &serial, &serialManager::moveFinger);
        ofRemoveListener(currentClassifier->fingerReleased, &serial, &serialManager::releaseFinger);
        ofRemoveListener(currentClassifier->fingerMoved, gui.get(), &BYBGui::moveFinger);
        ofRemoveListener(currentClassifier->fingerReleased, gui.get(), &BYBGui::releaseFinger);
    }
        if (nextClass != currentClassifier) {
            nextClass->copyCalibratioFrom(currentClassifier);
            currentClassifier = nextClass;
            ofAddListener(currentClassifier->fingerMoved, &serial, &serialManager::moveFinger);
            ofAddListener(currentClassifier->fingerReleased, &serial, &serialManager::releaseFinger);
            ofAddListener(currentClassifier->fingerMoved, gui.get(), &BYBGui::moveFinger);
            ofAddListener(currentClassifier->fingerReleased, gui.get(), &BYBGui::releaseFinger);
        }
    }
}
//--------------------------------------------------------------
BaseFingersClassifier* ofApp::getClassifier(){
    return currentClassifier;
}
//--------------------------------------------------------------
void ofApp::newSerialData(vector<unsigned int> & d){
	vector<float>lp;
	lp.resize(NUM_GRAPHS, 0.0f);
   // cout << "loPassData Size: " << loPassData[0].size() <<endl;
	for (int i = 0; i < d.size() && i < NUM_GRAPHS; i++) {
		memcpy(originalData[i].data(), originalData[i].data()+1, (originalData[i].size()-1)*sizeof(float));
		originalData[i].back() = d[i];
        memcpy(loPassData[i].data(), loPassData[i].data()+1, (loPassData[i].size()-1)*sizeof(float));
        loPassData[i].back() = loPass(originalData[i], gui->lopassSize);
        lp[i] = loPassData[i].back();
	}
    if (currentClassifier) {
        currentClassifier->update(loPassData, lp, gui->update(lp), gui->peakDetSize  ,gui->getLastPeak());
    }
//    if(gui->update(lp)){//if has peaks
//		classifier.updatePeak(gui->getLastPeak());
//	}
}
//--------------------------------------------------------------
void ofApp::startCalibration(){
    if (currentClassifier) {
        currentClassifier->startCalibration();
    }
}
//--------------------------------------------------------------
void ofApp::update(){
	serial.update();
}
//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
	gui->draw();
}
//--------------------------------------------------------------
void ofApp::saveFingerProfile(){
	ofFileDialogResult res = ofSystemLoadDialog("Choose folder for saving profile", true, ofToDataPath("profiles"), true);
	if (res.bSuccess) {
        if (currentClassifier) {
            currentClassifier->save(res.getPath());
        }
	}
}
//--------------------------------------------------------------
void ofApp::loadFingerProfile(){
	ofFileDialogResult res = ofSystemLoadDialog("Select profile folder", true, ofToDataPath("profiles"));//, false);
	if (res.bSuccess) {
		ofFile f(res.getPath());
		if (f.isDirectory()) {
            if (currentClassifier) {
                currentClassifier->load(res.getPath());
            }
		}
	}
}



