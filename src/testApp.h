#pragma once

#include "ofMain.h"
#include "RadialPath.h"
#include "Particle.h"
#include "Cloud.h"
#include "ofxUI.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"


#define PI180 0.0174532925



typedef struct {
    
    ofPoint latlon;
    ofVec3f position;
    
} City;



class testApp : public ofBaseApp{
    
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
    //void windowResized(int w, int h);
    //void dragEvent(ofDragInfo dragInfo);
    //void gotMessage(ofMessage msg);
    
    
    // colors
    ofColor blue;
    ofColor orange;
    ofColor yellow;
    
    
    // CITY stuff, lat/lon, draw splines from city to city
    RadialPath path;
    vector<RadialPath> radials;
    vector<City> cities;
    City boston;
    
    
    // utility methods
    ofVec3f spherize( float angX, float angY, float z );
    ofVec3f spherizeCrazy ( float x, float y, float z );
    ofVec3f convertLL( ofPoint pt );
    ofPoint revertLL( ofVec3f vec );
    
    
    // globe sphere
    ofImage src;
    vector<ofVec3f> spherePts;
    vector<ofVec3f> pointsOnSphere ( int numPts );
    
    float globeRotRateY;
    float globeRotRateX;
    float globeRotRateZ;
    
    
    // in progress:
    void lookAt(const ofVec3f& lookAtPosition );
    
    
    // KINECT / openCV
    ofxKinect kinect;
    //ofxCvGrayscaleImage grayImage;
    int farThreshold;
    int cloudPointsMin;
    ofMesh realtimeMesh;
    vector<Cloud> clouds;
    int kinectStep;
    bool bDrawRTSphere;
    float ptSphereScale;
    bool drawCloudStrings;
    
    void buildRTCloud(); // real time cloud
    void captureCloud();
    
    
    
    // LISSAJOUS
    int lissX, lissY;
    float lissScale;
    
    
    
    // drawing
    int pointSize;
    float worldScale;
    ofVec3f globeRotation;
    
    
    
    
    
    vector<Particle> particles;
    
    ofEasyCam cam;
    
    
    // GUI
    ofxUICanvas *gui;
    void guiEvent(ofxUIEventArgs &e);
    
    
    
};