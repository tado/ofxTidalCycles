#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//ofSetFrameRate(60);
	ofBackground(0);

	//args: port number, buffering bar num
	tidal = new ofxTidalCycles(3333, 4);
}

//--------------------------------------------------------------
void ofApp::update(){
	tidal->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	float marginTop = ofGetWidth() / 32.0;
	float marginLeft = ofGetWidth() / 32.0;
	tidal->drawGrid(marginLeft, marginTop, ofGetWidth() - marginLeft * 2, ofGetHeight() / 2 - marginTop * 2);
	tidal->drawNotes(marginLeft, marginTop, ofGetWidth() - marginLeft * 2, ofGetHeight() / 2 - marginTop * 2);
	tidal->drawInstNames(marginLeft, marginTop, ofGetWidth() - marginLeft * 2, ofGetHeight() / 2 - marginTop * 2);
	tidal->drawGraph();
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
