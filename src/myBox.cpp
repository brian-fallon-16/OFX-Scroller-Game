#include "myBox.h"

//--------------------------------------------------------------
myBox::myBox()
{
    // Default constructor - set everything to zero
    dims = ofVec3f(0,0,0);
    position = ofVec3f(0,0,0);
    rotAxes = ofVec3f(0, 0, 0);
    rotation = 0;
}

//--------------------------------------------------------------
myBox::myBox(ofVec3f size, ofVec3f pos, ofVec3f rot_axes)
{
    // Other constructor - set variables to args provided
    dims = size;
    position = pos;
    rotAxes = rot_axes;
    rotation = 0;
}

//--------------------------------------------------------------
void myBox::setSize(ofVec3f size)
{
    dims = size;
}

//--------------------------------------------------------------
void myBox::setPos(ofVec3f pos)
{
    position = pos;
}

//--------------------------------------------------------------
void myBox::setRotAxes(ofVec3f rot_axes)
{
    rotAxes = rot_axes;
}

//--------------------------------------------------------------
void myBox::move(float z)
{
    // Increment z-position by supplied value
    position.z +=z;
}

//--------------------------------------------------------------
void myBox::rotate(float rot)
{
    // Increment rotation by supplied value
    rotation += rot;
    
    // Subtract 360 if rotation exceeds 360 to avoid overflow
    if(rotation >= 360)
    {
        rot -= 360;
    }
}

