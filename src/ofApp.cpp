#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    // openFrameworks stuff
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);
    
    // GUI stuff - add three parameters
    gui.setup("panel");
    gui.add(movement.set("Movement", 1, 1, 100));
    gui.add(colour.set("Colour", 0, 0, 100));
    gui.add(lighting.set("Lighting", 0, 0, 100));
    
    // OSC stuff - out port not in use
    OSC_in.setup(IN_PORT);
    //OSC_out.setup("127.0.0.1", OUT_PORT);
    
    // Lighting stuff
    ofSetSmoothLighting(true);
    pointLight.setAmbientColor(ofFloatColor(0., 0., 0.));
    pointLight.setDiffuseColor(ofFloatColor(.55, .25, .25) );
    pointLight.setSpecularColor(ofFloatColor(1.f, 1.f, 1.f));
    backgroundColour = ofColor(lighting);
    
    // Make material quite shiny with a white specular reflection
    material.setShininess(100);
    material.setSpecularColor(ofColor(255, 255, 255, 255));
    
    // Post-processing stuff
    post.init(ofGetWidth(), ofGetHeight());
    post.createPass<FxaaPass>();
    post.createPass<BloomPass>();
    post.createPass<GodRaysPass>();
    post.createPass<DofPass>();
    
    // Initialize key buffer
    for(int i = 0; i < sizeof(keyBuf); i++)
    {
        keyBuf[i] = false;
    }
    
    // Intialize screen size variables
    screenWidth = ofGetWidth();
    screenHeight = ofGetHeight();
    
    // Initialize boxes
    for(int i = 0; i < numBoxes; i++)
    {
        newBox();
    }
}

//--------------------------------------------------------------
void ofApp::update()
{
    // Get OSC messages
    while(OSC_in.hasWaitingMessages())
    {
        ofxOscMessage m;
        OSC_in.getNextMessage(m);
        if(m.getAddress() == "/envVar")
        {
            movement = m.getArgAsFloat(0);
            colour = m.getArgAsFloat(1);
            lighting = m.getArgAsFloat(2);
        }
    }
    
    // Move light
    pointLight.setPosition((screenWidth*.5)+ cos(ofGetElapsedTimef()*.5)*(screenWidth*.3), screenHeight/2, 500);
    

    // Remove boxes out of sight
    for(int i = 0; i < myBoxes.size(); i++)
    {
        if(myBoxes[i].position.z > screenWidth/2)
        {
            myBoxes.erase(myBoxes.begin()+i);
            newBox();
        }
    }
    
    // Adjust environment attributes
    
    // Movement
    
    // Move boxes
    // Scale movement variable exponentially from 0-100 to 0.0-1.0 - for more adjustment resolution at lower speeds
    float scaledMovement = 100*(pow(movement/100,2));
    for(unsigned int i = 0; i < myBoxes.size(); i++)
    {
        myBoxes[i].move(scaledMovement);
    }
    
    // Increment z translation for terrain
    z_pos -= scaledMovement;
    
    // Increase rotation increment for boxes (minimum is approx. 0.4)
    rotationIncrement = 0.4+(scaledMovement*0.05);
    
    // Colour
    float colourAdjust = colour/100;
    pointLight.setDiffuseColor(ofFloatColor(1, (1-.55*colourAdjust), (1-.55*colourAdjust)));
    
    // Lighting
    backgroundColour = ofColor(lighting);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    // Surface frequency and x/z-axis mesh resolutions
    float frequency = 0.001;
    int x_res = 50;
    int z_res = 50;

    // Z-offset places the end of the terrain slightly behind the viewport
    float z_offset = screenWidth;
    
    // Area of the terrain - needs to be relative to screen size
    ofVec3f size(screenWidth*2, screenHeight*0.8, screenWidth*5);
    
    // Insantiate a wireframe mesh
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_LINES);
    
    // Create vertices and normals for points within the area
    // Number of points per dimension depends on resolution/size ratio
    for( int z = 0; z < z_res; z++ )
    {
        for( int x = 0; x < x_res; x++ )
        {
            // Get y-coordinates
            ofVec3f p = getTerrain(ofMap(x, 0, x_res, size.x * -0.5f, size.x * 0.5f),
                                   ofMap(z, 0, z_res, z_pos, z_pos-size.z),
                                   frequency, size.y);
            
            // Calculate normal for lighting
            ofVec3f pRight = getTerrain(ofMap(x + 1, 0, x_res, size.x * -0.5f, size.x * 0.5f),
                                        ofMap(z, 0, z_res, z_pos, z_pos-size.z),
                                        frequency, size.y );
            ofVec3f pBot = getTerrain(ofMap(x, 0, x_res, size.x * -0.5f, size.x * 0.5f),
                                      ofMap(z + 1, 0, z_res, z_pos, z_pos-size.z),
                                      frequency, size.y);
            ofVec3f normal = (pBot-p).getCrossed((pRight-p)).getNormalized();
            
            // Keep the terrain in place but calculate y-coordinate for a shifting z-coordinate
            // This gives the impression of moving over the surface while not actually moving the viewpoint
            pBot.z = z_offset+ofMap(z, 0, z_res, 0, -size.z);
            pRight.z = z_offset+ofMap(z, 0, z_res, 0, -size.z);
            p.z = z_offset+ofMap(z, 0, z_res, 0, -size.z);
            
            // Add the vertex and the normal to the mesh
            mesh.addVertex(p);
            mesh.addNormal(normal);
        }
    }
    
    // Add triangles to the mesh according to the specified resolution
    for(int y = 0; y < z_res-1; y++)
    {
        for(int x = 0; x < x_res-1; x++)
        {
            int tmp = (y * x_res) + x;
            mesh.addTriangle(tmp, tmp + 1, tmp + 1 + x_res);
            mesh.addTriangle(tmp, tmp + 1 + x_res, tmp + x_res);
        }
    }
    
    // DRAWING HAPPENS HERE
    // Enable post processing, depth test, lighting
    post.begin();
    ofEnableDepthTest();
    ofBackgroundGradient(backgroundColour, ofColor(0), OF_GRADIENT_CIRCULAR);
    ofEnableLighting();
    pointLight.enable();
    
        // Draw the terrain
        mesh.draw();
    
        // Draw boxes with shiny material
        material.begin();
            drawBoxes();
        material.end();
    
    // End post processing, disable lighting and depth test
    post.end();
    ofDisableLighting();
    ofDisableDepthTest();
    
    // Draw the GUI
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    // Set the corresponding key state to true
    keyBuf[key] = true;
    
    
    switch(key)
    {
        // toggle fullscreen and resize the post-processing FBO
        case 'f':
            ofToggleFullscreen();
            post.init(ofGetWidth(), ofGetHeight());
        break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    // Unset the corresponding key state when released
    keyBuf[key] = false;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
    // Get new screen dimensions
    screenWidth = ofGetWidth();
    screenHeight = ofGetHeight();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
}

//--------------------------------------------------------------
void ofApp::newBox()
{
    // Add a new box to the array
    myBoxes.push_back(myBox());
    int index = myBoxes.size()-1;
    
    // Set its size and position relative to screen size
    myBoxes[index].setSize(ofVec3f(ofRandom(screenWidth/2)*.18,
                                   ofRandom(screenWidth/2) *.18, 2));
    myBoxes[index].setPos(ofVec3f(ofRandom(screenWidth),
                                  ofRandom(screenHeight),
                                  -ofRandom(screenWidth*2)));
    
    // Rotate the box about the x and y axes (same for all boxes - random axes didn't look as good)
    myBoxes[index].setRotAxes(ofVec3f(1, 1, 0));
}

//--------------------------------------------------------------
void ofApp::drawBoxes()
{
    // Draw the entire box array
    for(int i = 0; i < myBoxes.size(); i++)
    {
        // Increase the current box's rotation by the current increment
        myBoxes[i].rotate(rotationIncrement);
        
        // Define coordinate system relative to the current box
        ofPushMatrix();
        
            // Translate then rotate then draw box primitive
            ofTranslate(myBoxes[i].position);
            ofRotate(myBoxes[i].rotation,
                     myBoxes[i].rotAxes.x,
                     myBoxes[i].rotAxes.y,
                     myBoxes[i].rotAxes.z);
            ofDrawBox(0, 0, 0,
                      myBoxes[i].dims.x,
                      myBoxes[i].dims.y,
                      myBoxes[i].dims.z);
        
        // Revert to original coordinate system
        ofPopMatrix();
    }
}

//--------------------------------------------------------------
ofVec3f ofApp::getTerrain(float x, float y, float frequency, float magnitude)
{
    // For a vector with given x and z, set the y-value according to the noise function
    // This gives terrain over many points
    ofVec3f p(x, 0, y);
    
    // Need to translate to centre of screen hence offsets
    p.y = screenHeight/2+MathUtils::fbm(ofVec2f(p.x*frequency, p.z*frequency), 4, 2.0, 0.5)*magnitude;
    p.x += screenWidth/2;
    return p;
}


