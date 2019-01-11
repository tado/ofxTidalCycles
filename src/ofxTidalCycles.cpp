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
		bgAlpha[i] = 0;
		for (int j = 0; j < max2; j++) {
			noteMatrix[i][j] = 0;
		}
	}
}

void ofxTidalCycles::update() {
	ofSetColor(255);
	int beatCount;
	while (receiver.hasWaitingMessages()) {
		ofxOscMessage m;
		receiver.getNextMessage(m);
		if (m.getAddress() == "/play2") {
			TidalNote n;
			n.timeStamp = ofGetElapsedTimef();
			int instN = 0;
			for (int i = 0; i < m.getNumArgs(); i += 2) {
				if (m.getArgAsString(i) == "cycle") {
					float cycle = m.getArgAsFloat(i + 1);
					float bar;
					float fract = modff(cycle, &bar);
					if (notes.size() == 0) {
						startBar = bar;
					}
					n.cycle = cycle;
					n.fract = fract;
					n.bar = bar;
					beatCount = int(fract * resolution);
					if (n.bar > lastBar) {
						//beatMonitor();
						calcStat();
						beatShift();
					}
					lastBar = int(bar);
				}
				if (m.getArgAsString(i) == "n") {
					instN = m.getArgAsInt(i + 1);
				}
				if (m.getArgAsString(i) == "s") {
					string s = m.getArgAsString(i + 1) + ofToString(instN);
					//string s = m.getArgAsString(i + 1);
					n.s = s;
					n.instNum = 0;
					bool newInst = true;
					for (int i = 0; i < instNameBuffer.size(); i++) {
						if (n.s == instNameBuffer[i]) {
							newInst = false;
							n.instNum = i;
						}
					}
					if (newInst) {
						instNameBuffer.push_back(n.s);
						std::sort(instNameBuffer.begin(), instNameBuffer.end());
						n.instNum = instNameBuffer.size() - 1;
					}
				}
				if (m.getArgAsString(i) == "cps") {
					float cps = m.getArgAsFloat(i + 1);
					n.cps = cps;
				}

				
				//erace unused inst
				for (int i = 0; i < instNameBuffer.size(); i++) {
					bool instExist = false;
					for (int j = 0; j < notes.size(); j++) {
						if (notes[j].bar > notes[notes.size() - 1].bar - maxBar * 2) {
							if (notes[j].s == instNameBuffer[i]) {
								instExist = true;
							}
						}
					}
					if (instExist == false) {
						instNameBuffer.erase(instNameBuffer.begin() + i);
					}
				}
			}
			notes.push_back(n);

			//add to note matrix
			for (int i = 0; i < notes.size(); i++) {
				noteMatrix[n.instNum][beatCount + max2 - resolution] = 1;
			}

			if (notes.size() > noteMax) {
				notes.erase(notes.begin());
			}
		}
	}
}

void ofxTidalCycles::drawNotes(float left, float top, float width, float height) {
	if (instNameBuffer.size() > 0) {
		for (int i = 0; i < notes.size(); i++) {
			int bar = notes[notes.size() - 1].bar - notes[i].bar;
			float x = ofMap(bar - notes[i].fract, -1, maxBar, width+left, left);
			float y = ofMap(notes[i].instNum, 0, instNameBuffer.size(), top, height+top);
			float h = height / instNameBuffer.size();
			float w = width / 128.0;
			if (x > left) {
				ofSetColor(255);
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

void ofxTidalCycles::drawBg(float left, float top, float width, float height) {
	ofDrawRectangle(left, top, width, height);
	for (int i = 0; i < instNameBuffer.size(); i++) {
		float bg;
		if (bgAlpha[i] > 255) {
			bg = 255;
		}
		else {
			bg = bgAlpha[i];
		}
		ofSetColor(bg);
		float h = (height / instNameBuffer.size());
		float y = h * i + top;
		ofDrawRectangle(left, y, width, h);
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
	int instNumMax = instNameBuffer.size();
	for (int i = 0; i < instNumMax; i++) {
		//calculate syncopation
		string bitStr;
		for (int j = 0; j < resolution; j++) {
			bitStr += to_string(noteMatrix[i][max2 - resolution + j]);
		}
		int digit = stoi(bitStr, nullptr, 2);
		syncopation[i] = SG[digit];

		/*
		cout << "digit " << i << " : "
			<< bitStr << " : " << digit
			<< " syncopation : " << SG[digit]
			<< endl;
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

void ofxTidalCycles::drawGraph(float top) {
	//draw graph
	float x, y, gwidth, gheight, graphX;
	float graphWidth;
	int instNumMax = instNameBuffer.size();

	x = 20;
	y = top;
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