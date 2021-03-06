#include "mainAppController.h"
#include "ofxModifierKeys.h"
#include "BYBGui.h"
#include "loPass.h"

//--------------------------------------------------------------
void mainAppController::setup(){
    
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
    
   
    
    originalData.resize(NUM_GRAPHS);
    graphData.resize(NUM_GRAPHS);
    loPassData.resize(NUM_GRAPHS);
    for (int i = 0; i < NUM_GRAPHS; i++) {
        originalData[i].resize(ofGetWidth());
        loPassData[i].resize(ofGetWidth());
        graphData[i] = shared_ptr<vector<float> >(new vector<float>);
        graphData[i]->resize(ofGetWidth());
        gui->graphs[i].data = graphData[i];

    }
    gui->gui.add(peakDet.parameters);
    gui->setup(lang);
    classifierSVM.setup(lang, gui);
    classifierEuc.setup(lang, gui);
    serial.setup();
    
    ofAddListener(serial.newDataEvent, this, &mainAppController::newSerialData);
    setClassifier(0);
    
    peakDet.setup();

}
//--------------------------------------------------------------
void mainAppController::setClassifier(int i){
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
            cout << "current classifier set " << currentClassifier->name << endl;
            ofAddListener(currentClassifier->fingerMoved, &serial, &serialManager::moveFinger);
            ofAddListener(currentClassifier->fingerReleased, &serial, &serialManager::releaseFinger);
            ofAddListener(currentClassifier->fingerMoved, gui.get(), &BYBGui::moveFinger);
            ofAddListener(currentClassifier->fingerReleased, gui.get(), &BYBGui::releaseFinger);
        }
    }
}
//--------------------------------------------------------------
BaseFingersClassifier* mainAppController::getClassifier(){
    return currentClassifier;
}
//--------------------------------------------------------------
void mainAppController::newSerialData(vector<unsigned int> & d){
    vector<float>lp;
    lp.resize(NUM_GRAPHS, 0.0f);
    // cout << "loPassData Size: " << loPassData[0].size() <<endl;
    for (int i = 0; i < d.size() && i < NUM_GRAPHS; i++) {
        memcpy(originalData[i].data(), originalData[i].data()+1, (originalData[i].size()-1)*sizeof(float));
        originalData[i].back() = d[i];
        memcpy(loPassData[i].data(), loPassData[i].data()+1, (loPassData[i].size()-1)*sizeof(float));
        loPassData[i].back() = loPass(originalData[i], gui->lopassSize);

        memcpy(graphData[i]->data(), loPassData[i].data(), loPassData[i].size()*sizeof(float));

        lp[i] = loPassData[i].back();
    }
    if (currentClassifier) {
        peakData p = peakDet.update(loPassData);
        gui->update(lp, p);
        currentClassifier->update(loPassData, lp,p);

    }
}
//--------------------------------------------------------------
void mainAppController::startCalibration(){
    if (currentClassifier) {
        currentClassifier->startCalibration();
    }
}
//--------------------------------------------------------------
void mainAppController::update(){
    serial.update();
}
//--------------------------------------------------------------
void mainAppController::draw(){
    ofBackground(0);
    gui->draw();
}
//--------------------------------------------------------------
void mainAppController::saveFingerProfile(){
//    ofFileDialogResult res = ofSystemLoadDialog("Choose folder for saving profile", true, ofToDataPath("profiles"), true);
    ofFileDialogResult res = ofSystemSaveDialog("fingers.xml", "Choose where to save your profile");  //("Choose folder for saving profile", true, ofToDataPath("profiles"));
    if (res.bSuccess) {
        if (currentClassifier) {
            currentClassifier->save(res.getPath());
        }
    }
}
//--------------------------------------------------------------
void mainAppController::loadFingerProfile(){
    ofFileDialogResult res = ofSystemLoadDialog("Select profile", false, ofToDataPath("profiles"));//, false);
    if (res.bSuccess) {
        if (currentClassifier) {
            currentClassifier->load(res.getPath());
        }
    }
}



