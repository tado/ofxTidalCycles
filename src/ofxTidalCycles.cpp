#include "ofxTidalCycles.h"

ofxTidalCycles::ofxTidalCycles(int port, int _barBuffer){
	barBuffer = _barBuffer;
	receiver.setup(port);
	ofSetFrameRate(60);
	ofBackground(0);
	lastBar = 0;	
}

void ofxTidalCycles::update() {
	while (receiver.hasWaitingMessages()) {
		ofxOscMessage m;
		receiver.getNextMessage(m);
		float cycle;
		//parse when tidal cycle message comming
		if (m.getAddress() == "/play2") {
			TidalNote note;
			note.timeStamp = ofGetElapsedTimef();
			for (int i = 0; i < m.getNumArgs(); i += 2) {
				//parse cycle
				float bar, fract;
				if (m.getArgAsString(i) == "cycle") {
					cycle = m.getArgAsFloat(i + 1);
					note.cycle = cycle;
					fract = modf(cycle, &bar);
					note.bar = int(bar);
					note.fract = fract;
					lastBar = int(bar);
				}
				//parse inst name
				if (m.getArgAsString(i) == "s") {
					note.instName = m.getArgAsString(i + 1);
					bool newInst = true;
					for (int i = 0; i < instNameBuffer.size(); i++) {
						if (note.instName == instNameBuffer[i]) {
							newInst = false;
							note.instNum = i;
						}
					}
					if (newInst) {
						instNameBuffer.push_back(note.instName);
						std::sort(instNameBuffer.begin(), instNameBuffer.end());
						note.instNum = instNameBuffer.size() - 1;
					}
				}
				//get latency
				if (m.getArgAsString(i) == "latency") {
					note.latency = m.getArgAsFloat(i + 1);
				}
			}
			notes.push_back(note);
		}
	}

	//erace unused inst
	for (int i = 0; i < instNameBuffer.size(); i++){
		bool instExist = false;
		for (int j = 0; j < notes.size(); j++) {
			if (notes[j].bar > lastBar - barBuffer) {
				if (notes[j].instName == instNameBuffer[i]) {
					instExist = true;
				}
			}
		}
		if (instExist == false){
			instNameBuffer.erase(instNameBuffer.begin() + i);
		}
	}
}

void ofxTidalCycles::drawNotes(float left, float top, float width, float height) {
	//draw notes
	ofSetColor(255);
	for (int i = 0; i < notes.size(); i++) {
		if (notes[i].bar > lastBar - barBuffer) {
			float h = height / (instNameBuffer.size());
			float w = width / 32.0 / barBuffer;
			float x = (notes[i].cycle - lastBar + barBuffer - 1) * width / barBuffer + left;
			float y = h * notes[i].instNum + top;
			if (ofGetElapsedTimef() - notes[i].timeStamp > notes[i].latency) {
				ofDrawRectangle(x, y, w, h);
			}
		}
	}
}

void ofxTidalCycles::drawGrid(float left, float top, float width, float height) {
	ofNoFill();
	ofSetColor(65);
	for (int i = 0; i < barBuffer * 8; i++) {
		float x = (width / barBuffer / 8) * i + left;
		ofDrawLine(x, top, x, top + height);
	}
	ofSetColor(100);
	for (int i = 0; i < barBuffer * 4; i++) {
		float x = (width / barBuffer / 4) * i + left;
		ofDrawLine(x, top, x, top + height);
	}
	ofSetColor(200);
	ofDrawRectangle(left, top, width, height);
	for (int i = 0; i < barBuffer; i++) {
		float x = (width / barBuffer) * i + left;
		ofDrawLine(x, top, x, top + height);
	}
	for (int i = 0; i < instNameBuffer.size(); i++) {
		float y = (height / instNameBuffer.size()) * i + top;
		ofDrawLine(left, y, left + width, y);
	}
	ofFill();
}