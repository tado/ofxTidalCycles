#include "ofxTidalCycles.h"
#include "SyncopationData.h"
#include<iostream>
#include<bitset>

ofxTidalCycles::ofxTidalCycles(int port, int _barBuffer) {
	barBuffer = _barBuffer;
	receiver.setup(port);
	lastBar = 0;
	resolution = 16;

	//init arrays
	for (int i = 0; i < max1; i++) {
		syncopation[i] = 0;
		noteNum[i] = 0;
		for (int j = 0; j < max2; j++) {
			noteMatrix[i][j] = 0;
		}
	}
}

void ofxTidalCycles::update() {
	while (receiver.hasWaitingMessages()) {
		ofxOscMessage m;
		receiver.getNextMessage(m);
		float cycle;
		int beatCount;
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
					beatCount = int(fract * resolution);
					if (note.bar > lastBar) {
						beatMonitor();
						calcStat();
						beatShift();
					}
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
				//get CPS
				if (m.getArgAsString(i) == "cps") {
					note.cps = m.getArgAsFloat(i + 1);
				}
				//get latency
				if (m.getArgAsString(i) == "latency") {
					note.latency = m.getArgAsFloat(i + 1);
				}
			}
			notes.push_back(note);

			//add to note matrix
			for (int i = 0; i < notes.size(); i++) {
				noteMatrix[note.instNum][beatCount + max2 - resolution] = 1;
			}
		}
	}

	//erace unused inst
	for (int i = 0; i < instNameBuffer.size(); i++) {
		bool instExist = false;
		for (int j = 0; j < notes.size(); j++) {
			if (notes[j].bar > lastBar - barBuffer) {
				if (notes[j].instName == instNameBuffer[i]) {
					instExist = true;
				}
			}
		}
		if (instExist == false) {
			instNameBuffer.erase(instNameBuffer.begin() + i);
		}
	}
}

void ofxTidalCycles::drawNotes(float left, float top, float width, float height) {
	//draw notes
	ofSetColor(255);
	for (int i = 0; i < notes.size(); i++) {
		if (ofGetElapsedTimef() - notes[i].timeStamp < 8.0) {
			float h = height / (instNameBuffer.size());
			float w = width / 64.0 / barBuffer;
			//float x = (notes[i].cycle - lastBar + barBuffer - 1) * width / barBuffer + left;
			float x = ofMap(ofGetElapsedTimef() - notes[i].timeStamp - notes[i].latency, 0, barBuffer, left, width);
			float y = h * notes[i].instNum + top;
			if (ofGetElapsedTimef() - notes[i].timeStamp >= notes[i].latency
				&& x < width + left - w
				&& y < height) {
				ofDrawRectangle(x, y, w, h);
			}
		}
	}
}

void ofxTidalCycles::drawGrid(float left, float top, float width, float height) {
	ofNoFill();
	ofSetColor(127);
	ofDrawRectangle(left, top, width, height);
	for (int i = 0; i < instNameBuffer.size(); i++) {
		float y = (height / instNameBuffer.size()) * i + top;
		ofDrawLine(left, y, left + width, y);
	}
	ofFill();
}

void ofxTidalCycles::drawInstNames(float left, float top, float width, float height) {
	ofSetColor(255);
	for (int i = 0; i < instNameBuffer.size(); i++) {
		float y = (height / instNameBuffer.size()) * i + top + 15;
		ofDrawBitmapStringHighlight(instNameBuffer[i], left + 5, y);
	}
}


void ofxTidalCycles::beatShift() {
	for (int i = 0; i < max1; i++) {
		for (int j = resolution; j < max2; j++) {
			noteMatrix[i][j - resolution] = noteMatrix[i][j];
		}
		for (int j = max2 - resolution; j < max2; j++) {
			noteMatrix[i][j] = 0;
		}
	}
}

void ofxTidalCycles::beatMonitor() {
	cout << "-------------------------" << endl;
	int instNumMax = instNameBuffer.size();
	/*
	for (int i = 0; i < notes.size(); i++) {
	noteMatrix[notes[i].instNum][int(notes[i].beatCount) + max2 - resolution] = 1;
	}
	*/
	for (int i = 0; i < instNumMax; i++) {
		cout << "part " << i << " : ";
		for (int j = 0; j < max2; j++) {
			cout << noteMatrix[i][j];
		}
		cout << endl;
	}
	cout << endl;
}

void ofxTidalCycles::calcStat() {
	for (int i = 0; i < max1; i++) {
		syncopation[i] = 0.0;
		noteNum[i] = 0.0;
	}

	/*
	//calclate all part matrix for joint entropy
	int *allVector = (int *)calloc(resolution, sizeof(int));
	for (int i = 0; i < instNumMax; i++) {
		for (int j = 0; j < resolution; j++) {
			allVector[j] += int(noteMatrix[i][max2 - resolution + j]);
			if (allVector[j] > 1) {
				allVector[j] = 1;
			}
		}
	}
	*/

	int instNumMax = instNameBuffer.size();
	for (int i = 0; i < instNumMax; i++) {
		//calculate syncopation
		string bitStr;
		for (int j = 0; j < resolution; j++) {
			bitStr += to_string(noteMatrix[i][max2 - resolution + j]);
		}
		int digit = stoi(bitStr, nullptr, 2);
		syncopation[i] = SG[digit];

		cout << "digit " << i << " : "
			<< bitStr << " : " << digit
			<< " syncopation : " << SG[digit]
			<< endl;

		/*
		//calc entropy
		uint *currentVector = (uint *) calloc(resolution,sizeof(uint));
		for (int j = 0; j < resolution; j++) {
		currentVector[j] = uint(noteMatrix[i][max2 - resolution + j]);
		}
		entropy[i] = calcEntropy(currentVector, resolution);
		*/

		//calc note count
		int num = 0;
		for (int j = 0; j < resolution; j++) {
			if (noteMatrix[i][max2 - resolution + j] == 1) {
				num++;
			}
		}
		noteNum[i] = num;
	}
}

void ofxTidalCycles::drawGraph() {
	//draw graph
	float x, y, gwidth, gheight, graphX;
	float graphWidth;
	int instNumMax = instNameBuffer.size();

	x = 20;
	y = ofGetHeight() / 2 + 20;
	graphX = 70;
	gwidth = ofGetWidth() - 40 - graphX;
	gheight = 10;

	ofTranslate(x, y);
	ofPushMatrix();
	ofSetColor(255);
	ofDrawBitmapString("Syncopation Degree", 0, 0);
	ofTranslate(0, -8);
	for (int i = 0; i < instNumMax; i++) {
		ofTranslate(0, 14);
		graphWidth = ofMap(syncopation[i], 0, 20, 0, gwidth);
		ofSetColor(63);
		ofDrawRectangle(graphX, 0, gwidth, gheight);
		ofSetColor(63, 127, 255);
		ofDrawRectangle(graphX, 0, graphWidth, gheight);
		ofSetColor(255);
		ofDrawBitmapString(instNameBuffer[i], 0, 10);
		ofDrawBitmapString(ofToString(syncopation[i]), graphX + 5, 10);
	}
	ofTranslate(0, 40);
	ofDrawBitmapString("Note Number", 0, 0);
	ofTranslate(0, -8);
	for (int i = 0; i < instNumMax; i++) {
		ofTranslate(0, 14);
		graphWidth = ofMap(noteNum[i], 0, 32, 0, gwidth);
		ofSetColor(63);
		ofDrawRectangle(graphX, 0, gwidth, gheight);
		ofSetColor(63, 127, 255);
		ofDrawRectangle(graphX, 0, graphWidth, gheight);
		ofSetColor(255);
		//ofDrawBitmapString("E"
		//                   + ofToString(i) + ":"
		//                   + ofToString(app->tidal->entropy[i], 2), 0, 10);
		ofDrawBitmapString(instNameBuffer[i], 0, 10);
		ofDrawBitmapString(ofToString(noteNum[i]), graphX + 5, 10);
	}
}