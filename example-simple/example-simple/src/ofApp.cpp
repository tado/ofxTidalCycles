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
	//draw notes
	for (int i = 0; i < tidal->notes.size(); i++) {
		float height = ofGetHeight() / (tidal->instNameBuffer.size());
		float width = ofGetWidth() / 32.0 / tidal->barBuffer;
		float x = (tidal->notes[i].cycle - tidal->lastBar + tidal->barBuffer - 1) * ofGetWidth() / tidal->barBuffer;
		float y = height * tidal->notes[i].instNum;
		if (ofGetElapsedTimef() - tidal->notes[i].timeStamp > tidal->notes[i].latency) {
			ofDrawRectangle(x, y, width, height);
		}
	}
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
