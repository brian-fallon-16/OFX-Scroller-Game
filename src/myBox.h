#pragma once

#include "ofMain.h"

// Box class to hold attributes of each box - no actual box objects
// These box attributes are used when ofDrawBox() is called
// Avoids the drawing errors that result from deleting elements of an ofBoxPrimitive array

class myBox
{
    
public:
    // Box attributes
    ofVec3f dims;
    ofVec3f position;
    ofVec3f rotAxes;
    float rotation;
    
    // Constructors
    myBox();
    myBox(ofVec3f size, ofVec3f pos, ofVec3f rot_axes);
    
    // Set functions
    void setSize(ofVec3f size);
    void setPos(ofVec3f pos);
    void setRotAxes(ofVec3f rot_axes);
    
    // Movement functions
    void move(float z);
    void rotate(float rot);
};
