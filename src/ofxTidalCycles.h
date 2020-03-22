#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

class TidalNote {
public:
        string s;
        int n;
        tuple< string, int, int > sound; // <s, n, orbit>
        tuple<int, int, int, int, int> orbit; // num, ind, size, min
        int instNum;
        float cycle;
        float cps;
        int startBar;
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
        void drawBg(float left, float top, float width, float height);
        void drawNotes(float left, float top, float width, float height);
        void drawInstNames(float left, float top, float width, float height);
        void drawGraph(float top);

        void beatShift();
        void beatMonitor();
        void calcStat();

        ofxOscReceiver receiver;
        vector<TidalNote> notes;
        vector< tuple<string, int, int> > instBuffer; // s, n, orbit
        map< int, set< pair<string, int> > > orbSounds; // orbit, <s, n>
        map< int, vector<int> > minmax;
        vector<int> activeOrbs;
        float orbCellHeight;

        float startBar;
        static const int maxBar = 4;

        int lastBar;
        int barBuffer;
        static const int noteMax = 1024 * 8;

        int resolution;
        static const int max1 = 128;
        static const int max2 = 64;
        int noteMatrix[max1][max2];
        float syncopation[max1];
        //double entropy[max1];
        //double jointEntropy[max1];
        //double mutualInformation[max1];
        int noteNum[max1];
        int bgAlpha[max1];
};

