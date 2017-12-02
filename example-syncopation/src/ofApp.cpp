#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60);
	ofBackground(0);

	//args: port number, buffering bar num
	tidal = new ofxTidalCycles(3333, 3);
}

//--------------------------------------------------------------
void ofApp::update(){
	tidal->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	float marginTop = ofGetWidth() / 32.0;
	float marginLeft = ofGetWidth() / 32.0;
	float w = ofGetWidth() - marginLeft * 2;
	float h = ofGetHeight() / 1.5 - marginTop * 2.0;
	tidal->drawBg(marginLeft, marginTop, w, h);
	tidal->drawGrid(marginLeft, marginTop, w, h);
	tidal->drawNotes(marginLeft, marginTop, w, h);
	tidal->drawInstNames(marginLeft, marginTop, w, h);
	tidal->drawGraph(marginTop + h + 40);
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
