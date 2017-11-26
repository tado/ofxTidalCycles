#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

class TidalNote {
public:
	string instName;
	float cycle;
	int instNum;
	int bar;
	float fract;
	float latency;
	float timeStamp;
};

class ofxTidalCycles{
public:
	ofxTidalCycles(int port, int barBuffer);
	void update();
	ofxOscReceiver receiver;
	vector<TidalNote> notes;
	vector<string> instNameBuffer;
	int lastBar;
	int barBuffer;
};

