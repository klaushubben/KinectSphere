//
//  Particle.h
//  KinectSphere
//
//  Created by Nick Hubben on 3/4/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "ofMain.h"



class Particle {
    
public:
    void setup();
    void update();
    void draw();
    
    ofPoint coord;
    ofVec3f pos;
    
    ofColor color;
    
};