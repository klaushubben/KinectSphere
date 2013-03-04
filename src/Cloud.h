//
//  Cloud.h
//  KinectSphere
//
//  Created by Nick Hubben on 3/1/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//



#pragma once
#include "ofMain.h"


class Cloud {
    
public:
    Cloud();
    ~Cloud();
    
    //void setup();
    void update();
    //void draw();
    
    void addPoint( ofVec3f pt , bool makeVel );
    void addColor( ofColor clr, bool makeVel );
    void addSphereTarg( ofVec3f sph );
    
    
    void toMesh( ofMesh &m );
    int getSize();
    
    void seek();
    void seek( const ofVec3f& target );
    void seekSphere();
    
    void floatAway();
    void fadeColors();
    
    // 
    int age;
    int trigger;
    int fade;
    int death;
    bool isDead;
    
    vector<ofVec3f> points;
    vector<ofColor> colors;
    
    vector<ofVec3f> targets;
    
    //when random
    vector<ofVec3f> vels;
    
    // when moving towards a target
    ofVec3f target;
    ofVec3f velocity;
    
    ofVec3f sphereTarg;
    
    
    // Lissajous
    int lissX, lissY;
    float lissOffset;
    void updateLissajous();
    vector<int> randoms;
    float lissScale;
    
    bool drawAsStrings;
    
    void finish();
    
    vector<ofPoint> LissRatios;
    
    
    int mode;
    
};