#include "ofxTidalCycles.h"
#include "SyncopationData.h"
#include<iostream>
#include<bitset>

ofxTidalCycles::ofxTidalCycles(int port, int _barBuffer) {
        barBuffer = _barBuffer;
        receiver.setup(port);
        lastBar = 0;
        resolution = 16;
        notes.reserve(max1);
        instBuffer.reserve(64);

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
                                        n.n = m.getArgAsInt(i + 1);
                                        get<1>(n.sound) = n.n;
                                }
                                if (m.getArgAsString(i) == "orbit") {
                                    get<0>(n.orbit) = m.getArgAsInt(i + 1);
                                    get<2>(n.sound) = get<0>(n.orbit);
                    // get orbit index
                                    auto match = find(activeOrbs.begin(), activeOrbs.end(), get<0>(n.orbit));
                                    if( match != activeOrbs.end() ) {
                                        get<1>(n.orbit) = match - activeOrbs.begin();
                                    }
                                    if ( !( find( activeOrbs.begin(), activeOrbs.end(), get<0>(n.orbit) ) != activeOrbs.end() ) ) {
                                        activeOrbs.push_back( get<0>(n.orbit));
                                        ofSort(activeOrbs);
                                    }
                                }
                                if (m.getArgAsString(i) == "s") {
                                        n.s = m.getArgAsString(i + 1);
                                        get<0>(n.sound) = n.s;
                                        n.instNum = 0;
                                        bool newInst = true;
                                        for (int i = 0; i < instBuffer.size(); i++) {
                                                if ( n.sound == instBuffer[i] ) {
                                                        newInst = false;
                                                        n.instNum = i;
                                                }
                                        }
                                        if ( !( find( begin( minmax[get<0>(n.orbit)] ),end( minmax[get<0>(n.orbit)] ), n.n ) != end( minmax[get<0>(n.orbit)] ) ) ) {
                                            transform( begin( orbSounds[get<0>(n.orbit)] ), end( orbSounds[ get<0>(n.orbit) ] ), back_inserter( minmax[get<0>(n.orbit)] ),
                                                    [](auto const& pair) { return pair.second; } );
                                        }
                                        if (newInst) {
                                                instBuffer.push_back(n.sound);
                                                ofSort(instBuffer);
                                                n.instNum = instBuffer.size() - 1;
                                        }
                                }
                                if (m.getArgAsString(i) == "cps") {
                                        float cps = m.getArgAsFloat(i + 1);
                                        n.cps = cps;
                                }


                                //erace unused inst
                                // 'zz' comes at the end of the OSC msg
                                if (m.getArgAsString(i) == "zz" && instBuffer.size() > 1 ) {
                                    //erace unused inst
                                    for ( auto thisInst : instBuffer ) {
                                        bool instExist = false;

                                        for ( auto thisNote : notes ) {
                                            if ( thisNote.bar > notes[notes.size() - 1].bar - maxBar * 2) {
                                                if( thisInst == thisNote.sound )
                                                    instExist = true;
                                            }
                                        }

                                        if ( instExist == false ) {

                                            minmax[get<0>(n.orbit)].erase(
                                                        remove( begin( minmax[get<0>(n.orbit)] ),
                                                        end( minmax[get<0>(n.orbit)] ), get<1>(thisInst) ),
                                                    end( minmax[get<0>(n.orbit)] ) );

                                            orbSounds[get<2>(thisInst)].erase( { get<0>(thisInst),
                                                                                  get<1>(thisInst) } );

                                            get<2>(n.orbit) = orbSounds[get<0>(n.orbit)].size();

                                            instBuffer.erase(
                                                        remove( instBuffer.begin(), instBuffer.end(), thisInst ),
                                                        instBuffer.end() );
                                            ofSort(instBuffer);

                                            if ( !( find( begin( minmax[get<0>(n.orbit)] ),end( minmax[get<0>(n.orbit)] ),
                                                          n.n ) != end( minmax[get<0>(n.orbit)] ) ) )
                                            {
                                                transform( begin( orbSounds[get<0>(n.orbit)] ),
                                                        end( orbSounds[ get<0>(n.orbit) ] ),
                                                        back_inserter( minmax[get<0>(n.orbit)] ),
                                                        [](auto const& pair) { return pair.second; }
                                                );
                                            }
                                        }

                                    }
                                    for ( auto thisOrb : activeOrbs ) {
                                        bool orbExist = false;
                                        for ( auto thisNote : notes ) {
                                            if ( thisNote.bar > notes[notes.size() - 1].bar - maxBar * 2 ) {
                                                if ( thisOrb == get<0>(thisNote.orbit) ) {
                                                    orbExist = true;
                                                }
                                            }
                                        }
                                        if ( orbExist == false ) {
                                            activeOrbs.erase(
                                                        remove(activeOrbs.begin(), activeOrbs.end(), thisOrb),
                                                        activeOrbs.end() );
                                            ofSort(activeOrbs);
                                            get<1>(n.orbit) = activeOrbs.size() - 1;

                                            auto match = find(
                                                        activeOrbs.begin(), activeOrbs.end(),  get<0>(n.orbit));
                                            if( match != activeOrbs.end() ) {
                                                get<1>(n.orbit) = match - activeOrbs.begin();
                                            }
                                        }
                                    }

                                    const auto min_iter = min_element(begin( minmax[get<0>(n.orbit)] ),
                                            end( minmax[get<0>(n.orbit)] ) );
                                    const auto max_iter = max_element(begin( minmax[get<0>(n.orbit)] ),
                                            end( minmax[get<0>(n.orbit)] ));
                                    if ( min_iter == minmax[get<0>(n.orbit)].end()) {
                                    } else {
                                        auto const min = *min_iter;
                                        get<3>(n.orbit) = min;
                                    }

                                    if ( max_iter == minmax[get<0>(n.orbit)].end()) {
                                    } else {
                                        auto const max = *max_iter;
                                        get<4>(n.orbit) = max;
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
        if (instBuffer.size() > 0) {
                float h, y, w = width / 128.0;
                for ( auto n : notes ) {
                        int bar = notes[notes.size() - 1].bar - notes[i].bar;
                        float x = ofMap(bar - notes[i].fract, -1, maxBar, width+left, left);
                        h = orbCellHeight / get<2>(n.orbit);
                        y = ofMap(n.n, get<3>(n.orbit), get<4>(n.orbit), orbCellHeight * get<1>(n.orbit),  orbCellHeight * get<1>(n.orbit) + orbCellHeight - h ) + top;
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
        for (size_t i = 0; i < activeOrbs.size(); i++) {
                    orbCellHeight = height / activeOrbs.size();
                    orbCellY = orbCellHeight * i;
                    ofDrawRectangle(left, orbCellY + top, width, orbCellHeight);
        }
        ofFill();
}

void ofxTidalCycles::drawBg(float left, float top, float width, float height) {
        ofDrawRectangle(left, top, width, height);
        for (int i = 0; i < instBuffer.size(); i++) {
                float bg;
                if (bgAlpha[i] > 255) {
                        bg = 255;
                }
                else {
                        bg = bgAlpha[i];
                }
                ofSetColor(bg);
                float h = (height / instBuffer.size());
                float y = h * i + top;
                ofDrawRectangle(left, y, width, h);
        }
        ofFill();
}

void ofxTidalCycles::drawInstNames(float left, float top, float width, float height) {
        ofSetColor(255);
        for (int i = 0; i < instBuffer.size(); i++) {
                float y = (height / instBuffer.size()) * i + top + 15;
                ofDrawBitmapStringHighlight(get<0>(instBuffer[i]), left + 5, y);
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
        int instNumMax = instBuffer.size();
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
        int instNumMax = instBuffer.size();
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
        int instNumMax = instBuffer.size();

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
                ofDrawBitmapString(get<0>(instBuffer[i]), 0, 10);
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
                ofDrawBitmapString(get<0>(instBuffer[i]), 0, 10);
                ofDrawBitmapString(ofToString(noteNum[i]), graphX + 5, 10);
        }
}
