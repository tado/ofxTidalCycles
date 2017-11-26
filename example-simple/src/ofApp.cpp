#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60);
	ofBackground(0);

	//args: port number, buffering bar num
	tidal = new ofxTidalCycles(3333, 8);
}

//--------------------------------------------------------------
void ofApp::update(){
	tidal->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	float margin = ofGetWidth() / 16.0;
	tidal->drawGrid(margin, margin, ofGetWidth() - margin * 2, ofGetHeight() - margin * 2);
	tidal->drawNotes(margin, margin, ofGetWidth() - margin * 2, ofGetHeight() - margin * 2);
	tidal->drawInstNames(margin, margin, ofGetWidth() - margin * 2, ofGetHeight() - margin * 2);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
