#include "testApp.h"
#include <math.h>

#define kinectW 640
#define kinectH 480


//--------------------------------------------------------------
void testApp::setup(){
    
    ofBackground(0,0,0);
	ofSetFrameRate(60);   
    ofSetVerticalSync(true);
    glEnable(GL_DEPTH_TEST);   
    
    
    
    blue.setHex( 0x00a6d4 );
    orange.setHex (0xff443a);
    yellow.setHex ( 0xecda41 );
    
    
    
    // ------  KINECT stuff
    
    kinect.init();
    kinect.setRegistration( true );
    kinect.open();
    //grayImage.allocate(kinectW, kinectH);
    farThreshold = 2000;
    cloudPointsMin = 500;
    
    realtimeMesh.setMode( OF_PRIMITIVE_POINTS); // OF_PRIMITIVE_TRIANGLES );
    realtimeMesh.setupIndicesAuto( );
    
    bDrawRTSphere = false;
    
    
    
    
    
    
    // -------  GLOBE stuff
    
    src.loadImage( "Equirectangular-1000.png" );
    
    float w = src.getWidth();
	float h = src.getHeight();
    
    globeRotation.set( 0.1f, 0.5f, 0.1f );
    
    // equidistant points on sphere
    spherePts = pointsOnSphere( 20000 );
    
    
    // map to lat/lon : get color : add to vector
    for( int i = 0 ; i < spherePts.size(); i++ ){
        
        // get Lat/Lon
        ofPoint pt = revertLL( spherePts[i] );
        
        // map to source image dimensions
        float x = ofMap( pt.y, -180.f, 180.f, 0.f, w );
        float y = ofMap( pt.x,  90.f,  -90.f, 0.f, h );
        
        // test source image for color/brightness
        ofColor clr = src.getColor( x, y );
        
        if( clr.getBrightness() > 1 ){
            
            Particle p;
            
            p.coord = pt;
            p.pos = spherePts[i]; //ofVec3f( x, y, 0 ); //
            
            particles.push_back( p );
        }
        
    }
    
    
    
    // -------------------------------  CITY to CITY arcs
    
    City boston;
    boston.latlon = ofPoint( 42.3584, -71.0598 );
    boston.position = convertLL( boston.latlon );
    
    vector<ofPoint> points;
    points.push_back( ofPoint( 37.7749, -122.4194 ) ); // SF
    points.push_back( ofPoint( 51.5171, -0.1062  ) );  // LONDON
    points.push_back( ofPoint( 35.6833, 139.7667) );  // TOKYO
    points.push_back( ofPoint( -33.8683, 151.2086 )); // SYDNEY
    points.push_back( ofPoint( -25.7256, 28.2439 )); // PRETORIA
    
    for( int i=0; i < points.size() ; i++ ){
        
        City city;
        city.latlon = points[i];
        city.position = convertLL( points[i] );
        cities.push_back( city );
        
        
        // create 'radialPath' from boston to city
        RadialPath path;
        path.setup( boston.position, city.position, 100.f );
        radials.push_back( path );
        
    }
    
    
    ///  ---   GUI
    
    pointSize = 2;
    worldScale = 200.f;
    ptSphereScale = 0.3;
    globeRotRateY = 0.5f;  
    lissScale = 275.f;
    kinectDelay = 100;
    kinectStep = 5;
    drawCloudStrings = true;
    showEarth = true;
    
    
    float dim = 16;
    float xSize = (200-OFX_UI_GLOBAL_WIDGET_SPACING)-OFX_UI_GLOBAL_WIDGET_SPACING;
    
    gui = new ofxUICanvas(0, 0, xSize, ofGetHeight());
    gui->setTheme( OFX_UI_THEME_ZOOLANDER );
    //gui->setColorPadded( ofColor(255,255,255));
    
    //gui->addFPSSlider("FPS", xSize, dim*.25, 1000);
    //gui->addSpacer(xSize, 1); 
    

    gui->addWidgetDown( new ofxUIToggle("SHOW EARTH", drawCloudStrings, dim*2, dim*2 ) );
    ofxUIToggle *toggle = (ofxUIToggle *) gui->getWidget("SHOW EARTH");
    toggle->getLabel()->setColorFill( ofColor( 255,255,255) );

    gui->addMinimalSlider("EARTH RotY", 0.0, 2.0, globeRotRateY, xSize, dim);
    ofxUISlider *slider = (ofxUISlider *) gui->getWidget("EARTH RotY");
    slider->getLabel()->setColorFill( ofColor( 255,255,255) );
    
    
    gui->addSpacer(xSize + 26, 1); // --------------
    
    gui->addMinimalSlider("CAPTURE DELAY", 10, 500, kinectDelay, xSize, dim);
    slider = (ofxUISlider *) gui->getWidget("CAPTURE DELAY");
    slider->getLabel()->setColorFill( ofColor( 255,255,255) );
    
    gui->addMinimalSlider("KINECT STEP", 1, 16, kinectStep, xSize, dim);
    slider = (ofxUISlider *) gui->getWidget("KINECT STEP");
    slider->getLabel()->setColorFill( ofColor( 255,255,255) );

    gui->addWidgetDown( new ofxUIToggle("DRAW STRINGS", drawCloudStrings, dim*2, dim*2 ) );
    toggle = (ofxUIToggle *) gui->getWidget("DRAW STRINGS");
    toggle->getLabel()->setColorFill( ofColor( 255,255,255) );
    
    gui->addMinimalSlider("LISS SCALE", 10, 800, lissScale, xSize, dim);
    slider = (ofxUISlider *) gui->getWidget("LISS SCALE");
    slider->getLabel()->setColorFill( ofColor( 255,255,255) );
    

    gui->addSpacer(xSize + 26, 1); // --------------

    
    gui->addMinimalSlider("Point Sphere SCALE", 0.1, 2, ptSphereScale, xSize, dim);
    slider = (ofxUISlider *) gui->getWidget("Point Sphere SCALE");
    slider->getLabel()->setColorFill( ofColor( 255,255,255) );
    
    
    gui->addSpacer(xSize + 26, 1); // --------------
    
    
    gui->addWidgetDown( new ofxUILabel("F: Toggle Fullscreen", OFX_UI_FONT_SMALL  ) );
    gui->addWidgetDown( new ofxUILabel("G: Toggle GUI / CAMERA", OFX_UI_FONT_SMALL  ) );
    gui->addWidgetDown( new ofxUILabel("S: Show/Hide POINT SPHERE", OFX_UI_FONT_SMALL  ) );
   
    gui->disable();
    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    
}





//--------------------------------------------------------------
void testApp::update(){
    ofSetWindowTitle(  ofToString( ofGetFrameRate(), 2 ) );
    
    kinect.update();    
    //    if( kinect.isFrameNew() ) {
    //        grayImage.setFromPixels(kinect.getDepthPixels(), kinectW, kinectH );
    //    }
    
    
    // need to WAIT a few frames for RBG image 
    if( ofGetFrameNum() > 100 ){
        
        // captures cloud snapshot
        if( ( ofGetFrameNum() % kinectDelay ) == 0 ) captureCloud();
        
        // captures realtime point cloud
        if( bDrawRTSphere ) buildRTCloud();
    }
    
    
    int i;
    
    // ARC between two points on the sphere/globe
    for( i=0; i < radials.size() ; i++ ){
        radials[i].update();
    }
    
    // UPDATE all captured point clouds
    for( i = 0; i < clouds.size(); i++ ){
        
        clouds[i].update();
        
        if( clouds[i].isDead ) {
            clouds.erase( clouds.begin() + i );
        }
    }
    
}




// --------------------------   REAL-TIME point cloud
void testApp::buildRTCloud(){
    
    realtimeMesh.clear();
    
    int dist = 0;
    int x, y;
    
    ofVec3f pos;
    
    for( y = 0; y < kinectH; y += kinectStep) {
        for( x = 0; x < kinectW; x += kinectStep) {
            
            dist = kinect.getDistanceAt(x,y);
            
            if(dist > 0 && dist < farThreshold ) {
                
                pos = kinect.getWorldCoordinateAt(x, y);
                pos = spherize( x, y, pos.z * ptSphereScale);
                
                realtimeMesh.addVertex( pos );
                realtimeMesh.addColor( kinect.getColorAt( x, y ) );
            }
        }
    }    
}



// -------------------------   CLOUD
void testApp::captureCloud() {
    
    // test the depth info
    int dist = 0;
    int count = 0;
    int x, y;
    
    // test if we have enough points to care about
    for( y = 0; y < kinectH; y += kinectStep ) {
        for( x = 0; x < kinectW; x += kinectStep) {
            
            dist = kinect.getDistanceAt(x,y);
            
            if(dist > 0 && dist < farThreshold ) count++;
            
            if( count >= cloudPointsMin ){
                y = kinectH;
                break;
            }
        }
    }
    
    // if there's enough point data, use it:
    if( count >= cloudPointsMin ) {
        
        Cloud c;
        ofVec3f pos;
        ofVec3f sph;
        
        for( y = 0; y < kinectH; y += kinectStep) {
            for( x = 0; x < kinectW; x += kinectStep) {
                
                dist = kinect.getDistanceAt(x,y);
                
                if(dist > 0 && dist < farThreshold ) {
                    
                    pos = kinect.getWorldCoordinateAt(x,y);
                    pos.x *= -1;
                    
                    //pos.z *= -1;
                    
                    //sph = spherize( x, y, pos.z * zScale );
                    //sph *= -1.f;
                    
                    //cout << sph << endl;
                    
                    // set target Vec3 for moving to sphere shape
                    //c.addSphereTarg( sph );
                    
                    // one must be true, other must be false (or vice versa)
                    c.addPoint( pos, true );  // if TRUE: velocity will be randomized
                    c.addColor( kinect.getColorAt( x, y ), false );  // if TRUE: velocity will be created from color channels
                }
            }
        }
        
        //        c.mode = 0; // float away/no target
        //        c.mode = 1; // seek spherical point 
        c.mode = 2; // Lissajous mode
        
        c.lissX = lissX;
        c.lissY = lissY;
        c.lissScale = lissScale;
        c.drawAsStrings = drawCloudStrings;
        c.finish();
        clouds.push_back( c );
        
        cout << "captured cloud: " << c.points.size()  << " points // mode: " << c.mode << endl;
    }
    
}





//----------------------------------------------  DRAW
void testApp::draw(){
    
    glPointSize( pointSize ); 
    ofSetColor( blue );
    ofNoFill();
    
    ofEnableBlendMode( OF_BLENDMODE_ADD );
    
    // ofEnableAlphaBlending();
    cam.begin();
    
    
    
    /// ---------------  DRAW Realtime Cloud
    if( realtimeMesh.hasVertices() && bDrawRTSphere ) realtimeMesh.drawVertices();
    
    
    /// -------------- DRAW captured clouds
    
    for( int i = 0; i < clouds.size(); i++ ){
        
        ofPushMatrix();
        
        ofScale( 1, -1, -1);
        
        ofMesh m;
        
        clouds[i].toMesh( m );    
        m.drawVertices();
        
        // ECHO copy 1
        //ofScale( 1, 1, 0.5 );
        //m.drawVertices();
        
        // ECHO copy 2
        //ofScale( 1, 1, 0.5 );
        //m.drawVertices();
        
        ofPopMatrix();
    }
    
    
    if( showEarth ){
    
    // ---------------- DRAW EARTH / POINT SPHERE
    
    ofPushMatrix();
    
    
    //DO IT WITH discrete objects
    /*          
     for( int i = 0; i < particles.size(); i++ ){
     
     ofPushMatrix();
     
     ofVec3f scaled = particles[i].pos * worldScale;
     ofBox( scaled.x, scaled.y, scaled.z, 1 );
     
     ofPopMatrix();
     }
     */
    
    
    
    // DO IT WITH a MESH
    ofScale( worldScale,worldScale,worldScale);
    
    globeRotation.y += globeRotRateY;
    ofRotateY( globeRotation.y ); 
    
    
    ofMesh mesh;
    for( int i = 0; i < particles.size(); i++ ){
        //mesh.addColor( ofColor(255,255,255) );
        mesh.addVertex( particles[i].pos );
    }
    
    mesh.drawVertices();
    
    
    ofPopMatrix();
    
    
    
    
    // ---------------- draw RADIAL PATHS
    
    ofPushMatrix();
    
    ofRotateY( globeRotation.y );  
    ofRotateX( -90 );
    
    float pathScl = worldScale * 0.011;
    ofScale( pathScl, pathScl, pathScl );
    
    ofSetColor( ofColor(255,255,255) );
    for( int i=0; i < radials.size() ; i++ ){
        radials[i].draw();
    }
    
    ofPopMatrix();
    
        
    }// end of showEarth
    
        
    // -----------------
    
        
        
    
    cam.end();
    // ofDisableAlphaBlending();
    ofDisableBlendMode();
    
}



//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    switch( key ){
            
        case 'g':
            if( cam.getMouseInputEnabled() ){
                cam.disableMouseInput();
                gui->enable();
            } else {
                cam.enableMouseInput();
                gui->disable();
            }
            break;  
            
            
        case 'f':
            ofToggleFullscreen();
            break;
            
            
            
        case '.':
            pointSize++;
            break;
            
        case ',':
            pointSize--;
            if( pointSize <= 0 ) pointSize = 1;
            break;
            
            
            
        case 's':
            bDrawRTSphere = !bDrawRTSphere;
            break;
            
    }
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}




//--------------------------------------------------------------  UTIL

// properly maps 2D coordinates to 3D sphere points
//
ofVec3f testApp::spherize( float x, float y, float z ){
    
    float phi = ( ((float)y/kinectH) * 180.f) * PI180;
    float theta = ( ((float)x/kinectW) * 360.f) * PI180;
    
    ofVec3f v;
    v.x = cos(theta) * sin(phi) * z;
    v.z=  sin(theta) * sin(phi) * z; 
    v.y = cos(phi) * z;
    
    return v;
}


// overrotates points, sort of spirally
//
ofVec3f testApp::spherizeCrazy( float x, float y, float z ){
    
    float latAng = y * PI180;
    float lonAng = x * PI180;
    
    ofVec3f v;
    v.x = cos(lonAng) * sin(latAng) * z; 
    v.y = sin(lonAng) * sin(latAng) * z; 
    v.z = cos(latAng) * z; 
    
    return v;
}




// creates vector of equally distributed points around a sphere
//
vector<ofVec3f> testApp::pointsOnSphere ( int numPts ) {
    
    vector<ofVec3f> list;
    
    float inc, off, y, r, phi; 
    int i; 
    
    inc = PI * ( 3.f - (sqrt( 5.f )) );
    off = 2.f / (float)numPts;
    
    i = numPts;
    
    while ( i > -1 ) {
        y = ( (float)i * off ) - 1.f + ( off / 2.f );
        r = sqrt( 1.f - (y*y) );
        phi = (float)i * inc;
        
        list.push_back ( ofVec3f(   cos(phi) * r, 
                                 y, 
                                 sin(phi) * r ) );
        
        i--;
    }
    return list;
}



// ofPoint (lat/lon) --> ofVec3f
ofVec3f testApp::convertLL( ofPoint pt ){
    float latAng = (90 - pt.x ) * PI180; 
    float lonAng = (180 + pt.y ) * PI180; 
    
    ofVec3f v;
    v.x = cos(lonAng) * sin(latAng); 
    v.y = sin(lonAng) * sin(latAng); 
    v.z = cos(latAng); 
    
    return v;
}


// ofVec3f -> ofPoint (lat/lon)
ofPoint testApp::revertLL( ofVec3f vec ){
    
    float lat = 90.f - acos(vec.y) * 180.f / PI;
    float lon = fmodf( (270.f + atan2(vec.x , vec.z) * 180.f / PI) , 360.f) - 180.f;  
    
    return ofPoint( lat, lon );
}






// NOT IMPLEMENTED / WORKING...
void testApp::lookAt(const ofVec3f& input ) {
    
    ofVec3f lookAtPosition( 0,0,0 );
    ofVec3f upVector(0, 1, 0);
    
    ofVec3f zaxis = ( input ).normalized();	
	if (zaxis.length() > 0) {
		ofVec3f xaxis = upVector.getCrossed(zaxis).normalized();	
		ofVec3f yaxis = zaxis.getCrossed(xaxis);
		
		ofMatrix4x4 m;
		m._mat[0].set(xaxis.x, xaxis.y, xaxis.z, 0);
		m._mat[1].set(yaxis.x, yaxis.y, yaxis.z, 0);
		m._mat[2].set(zaxis.x, zaxis.y, zaxis.z, 0);
		
        ofQuaternion q = m.getRotate();
        ofVec3f qaxis; float qangle;  
        q.getRotate(qangle, qaxis);
        
        //ofRotate( qangle, qaxis.x, qaxis.y, qaxis.z );
        
	}
}





//--------------------------------------------------------------  GUI update
void testApp::guiEvent(ofxUIEventArgs &e) {
    
	string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
    
    if( name == "DRAW STRINGS" ){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        drawCloudStrings = (bool)toggle->getValue();
        //cout << "RANDOM? " << randomizeCloud << endl;
    } 
    else 
        if( name == "SHOW EARTH" ){
            
            ofxUIToggle *toggle =( ofxUIToggle*) e.widget;
            showEarth = (bool)toggle->getValue();
        }
       
     else
         if( name == "CAPTURE DELAY"){
             ofxUISlider *slider =( ofxUISlider *) e.widget;
             kinectDelay = (int)slider->getScaledValue();
         }
    else
        if( name == "KINECT STEP" ){
            ofxUISlider *slider = (ofxUISlider *) e.widget; 
            kinectStep = (int)slider->getScaledValue(); 
        } 
        else
            if( name == "LISS SCALE" ){
                ofxUISlider *slider = (ofxUISlider *) e.widget; 
                lissScale = slider->getScaledValue(); 
            } 
            else
                if( name == "EARTH RotY") {
                    ofxUISlider *slider = (ofxUISlider *) e.widget;
                    globeRotRateY = slider->getScaledValue();
                }
                else
                    if( name == "Point Sphere SCALE") {
                        ofxUISlider *slider = (ofxUISlider *) e.widget;
                        ptSphereScale = slider->getScaledValue();
                    }
    
}



/*
 
 //--------------------------------------------------------------
 void testApp::windowResized(int w, int h){
 
 }
 
 //--------------------------------------------------------------
 void testApp::gotMessage(ofMessage msg){
 
 }
 
 //--------------------------------------------------------------
 void testApp::dragEvent(ofDragInfo dragInfo){ 
 
 }
 */