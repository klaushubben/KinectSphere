//
//  RadialPath.h
//  KinectSphere
//
//  Created by Nick Hubben on 2/28/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"


class RadialPath : public ofPolyline {
    
    
public:
    
    bool isDone;
    
    ofVec3f pos;
    ofVec3f center;
    ofVec3f end;
    ofVec3f start;
    ofVec3f vel;
    
    float rate;
    
    float radius;
    int step;
    
    
    void setup( ofVec3f _start, ofVec3f _end, float _rad ){
        
        isDone = false;
        step = 0;
        
        radius = _rad;
        start = _start * radius;
        end = _end * radius;
        
        
        
        rate = 0.01f;
        
        pos.set( start );
        addVertex( start );
        
        //cout << "Path:: " << start << endl;
        // cout << "  end: " << end << endl;
    }
    
    void update(){
        
        if( !isDone ) {
            
            step++;
            
            vel.set( end );
            vel -= pos;
            
            float distance = vel.length();
            if( distance < 1.f ) isDone = true;
            
            vel.normalize();
            pos += vel;
            
            distance = pos.distance( ofVec3f( 0,0,0 ));
            pos += ( radius - distance );
            
            addVertex( pos );
            
            //cout << "  :: " << pos << endl;
            
        }
    }
    
    void updateLoop() {
        
        step++;
        
        
        
    }
    
    
    
};