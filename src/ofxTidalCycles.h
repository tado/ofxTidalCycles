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

	void drawGrid(float left, float top, float width, float height);
	void drawNotes(float left, float top, float width, float height);

	ofxOscReceiver receiver;
	vector<TidalNote> notes;
	vector<string> instNameBuffer;
	int lastBar;
	int barBuffer;
};

