//----------------------------------------------------------------------------------------------------------------------------
//
//  This app creates a 3D scroller-like scenario where noise-generated terrain and an array of randomly generated rotating
//  boxes apparently move past the viewport.
//
//  Author:     Brian Fallon
//  Date:       August 2017
//
//  Terrain generation based on Noise Workshops code for Resonate by Andreas Muller
//  see:        https://github.com/andreasmuller/NoiseWorkshopScratchpad/tree/master/NoiseTerrain
//
//  see also:   https://github.com/andreasmuller/NoiseWorkshop
//              https://github.com/andreasmuller/NoiseWorkshopScratchpad/
//
//----------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "ofMain.h"
#include "ofxPostProcessing.h"
#include "ofxOsc.h"
#include "Math/MathUtils.h"
#include "ofxGui.h"
#include "myBox.h"

#define IN_PORT 9001
//#define OUT_PORT 9002 - UNUSED

class ofApp : public ofBaseApp
{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    // Box initialization & drawing functions
    void newBox();
    void drawBoxes();
    
    // Terrain generation function
    ofVec3f getTerrain(float x, float y, float frequency, float magnitude);
    
    // Screen dimension variables
    float screenWidth;
    float screenHeight;
    
    // Post processing object
    ofxPostProcessing post;
    
    // GUI stuff
    ofxPanel gui;
    ofParameter<float> colour;
    ofParameter<float> lighting;
    ofParameter<float> movement;
    
    // OSC stuff
    ofxOscReceiver OSC_in;
    //ofxOscSender OSC_out;
    
    // Light object and background colour
    ofLight pointLight;
    ofColor backgroundColour;
    
    // Box array and attributes
    vector<myBox> myBoxes;
    float numBoxes = 50.0;
    float rotationIncrement = 0.5;
    ofMaterial material;

    // Terrain z-position
    float z_pos = 0.0;
    
    // Key buffer just in case
    bool keyBuf[512];
};
