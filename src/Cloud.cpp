//
//  Cloud.cpp
//  KinectSphere
//
//  Created by Nick Hubben on 3/1/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "Cloud.h"

/* lissajou targets
 float xPct = (float)(i * mouseX) * 0.0001;
 float yPct = (float)(i * mouseY) * 0.0001;
 float x =  radius * sin(xPct);
 float y =  radius * cos(yPct);
 float z = radius * cos(xPct);
*/




Cloud::Cloud () {
    
    age = 0;
    
    trigger = 82;
    fade = 500;
    death = 300; // added to 'fade' time
    
    
    // I'm not dead yet...
    isDead = false;
    
    mode = (int)ofRandom( 3 );
    
    lissX = lissY = 10;
    lissOffset = 10;
    
    
    //for( int i = 0; i < points.size(); i++ ){
    //  randoms.push_back( i );
    //}
    
    LissRatios.push_back(ofPoint(250,500));
    LissRatios.push_back(ofPoint(300,500));
    LissRatios.push_back(ofPoint(666,500));
    LissRatios.push_back(ofPoint(950,500));
    LissRatios.push_back(ofPoint(309,312));
    
}


Cloud::~Cloud() {
    points.clear();
    colors.clear();
    vels.clear();
}

void Cloud::update(){
    
    age++;
    
    if( age > trigger ){
        
        if( mode == 0 ){
            floatAway();
        }
        else 
            if( mode == 1 ) {
                seekSphere();
            }
            else 
                if( mode == 2 ){             
                    updateLissajous();
                    seekSphere();
                }
                else seek();
        
    }
    
    if( age > fade ){
        fadeColors();
    } 
    
    if( age > fade + death ) {
        isDead = true;
    }      
    
}

/// -------------  ANIMATE

void Cloud::seek( ){
    seek( target );    
}

void Cloud::seek( const ofVec3f& target ){
    
    ofVec3f desiredVel;
    
    float fDist;
    bool arrived = true;
    
    for( int i = 0; i < points.size(); i++ ){
        
        desiredVel.set( target );
        desiredVel -= points[i];
        
        fDist = desiredVel.length();
        if( fDist > 10.f ) arrived = false;
        
        desiredVel.normalize();
        desiredVel *= 6.f;
        
        points[i] += desiredVel;
    }
    
    isDead = arrived;
}


void Cloud::seekSphere(){
    
    ofVec3f desiredVel;
    
    float fDist;
    bool arrived = true;
    
    for( int i = 0; i < points.size(); i++ ){
        
        desiredVel.set( targets[i] );
        desiredVel -= points[i];
        
        if(  mode == 1  ) {
            fDist = desiredVel.length();
            if( fDist > 10.f ) arrived = false;            
        }
        
        desiredVel.normalize();
        desiredVel *= 6.f;
        
        points[i] += desiredVel;
    }
    
    if( mode == 1 ) 
        isDead = arrived;
    
}




void Cloud::floatAway() {
    
    for( int i = 0; i < points.size(); i++ ){
        points[i] += vels[i];
    }
}

void Cloud::fadeColors() {
    
    for( int i = 0; i < colors.size(); i++ ){
        colors[i].a -= 0.1;
    }
}



// ---------------    BUILD POINT DATA

void Cloud::addPoint ( ofVec3f pt, bool makeVel = false ) {
    
    points.push_back(pt);
    
    // add random velocity vec3f
    if( makeVel ) vels.push_back( ofVec3f( ofRandom( 4.f, 2.f), 0, ofRandom(-2.f,2.f) ));
    
    
    //populate and shuffle the 'randoms' vector
    randoms.push_back(  (int)randoms.size() );
    targets.push_back( ofVec3f(0,0,0) );
}

void Cloud::finish() {
    
    if( !drawAsStrings ){
        random_shuffle( randoms.begin(), randoms.end() );
    }  
    
    int ran = ofRandom( LissRatios.size() );
    lissX = LissRatios[ran].x;
    lissY = LissRatios[ran].y;
    
    //cout << points.size() << " :: " << lissX << " :: " << lissY << endl;
    
}



void Cloud::addColor ( ofColor clr, bool makeVel = false ){
    
    colors.push_back(clr);
    
    // convert color to x/y/z velocity
    if( makeVel ){
        float vR = ofMap( clr.r, 0, 255, -6, -3 );
        float vG = ofMap( clr.g, 0, 255, -1, 1 );
        float vB = ofMap( clr.b, 0, 255, -5, 5 );    
        vels.push_back( ofVec3f( vR, vG, vB ) );
    }
}


void Cloud::addSphereTarg( ofVec3f sph ){
    
    targets.push_back ( sph );
    
}


void Cloud::updateLissajous() {
    
    
    lissOffset += 0.3;
    if( lissOffset >= points.size() ){
        lissOffset = 0.0f;
    }
    
    //float dist = 400.f;
    
    float dist;
    
    for( int i = 0; i < points.size(); i++ ){
        
        float xPct = (float)(( (float)i+lissOffset) * lissX ) * 0.0001;
        float yPct = (float)(( (float)i+lissOffset) * lissY ) * 0.0001;
        
        dist = 1.f + ((float)(colors[i].getBrightness()/100.f) * 0.1 );
        //if( i < 30 ) cout << dist << " : " << colors[i].getBrightness() << endl;
        
        
        // this is cylindrical
        //targets[randoms[i]].x = dist * sin(xPct);
        //targets[randoms[i]].y = dist * cos(yPct);
        
        // this is spherical
        targets[randoms[i]].x = dist * lissScale * sin(xPct) * sin(yPct);
        targets[randoms[i]].y = dist * lissScale * cos(yPct) * sin(xPct);
        
        
        // z is the same for both
        targets[randoms[i]].z = dist * lissScale * cos(xPct);
        
        //targets[randoms[i]] *= dist;
        
        //if( i == 0 ) cout << targets[randoms[i]] << endl;
    }
    
    
}




// --------------   ADD TO MESH

void Cloud::toMesh( ofMesh &m ) {
    
    for( int i = 0; i < points.size(); i++ ){
        m.addColor( colors[i] );
        m.addVertex( points[i] );
    }
}




int Cloud::getSize() {
    return points.size();
}
