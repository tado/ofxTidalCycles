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