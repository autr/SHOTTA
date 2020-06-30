#include "ofApp.h"
#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <pwd.h>
#include <unistd.h>
#endif


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetDataPathRoot("../Resources/data");
    ofSetFrameRate(60);
    blip.load("blip.wav");
    blip.setVolume(0.8);
    video = new ofxFlex::Video();
    uiHeight = 5;
    
    ofSetWindowTitle( "shotta");
    
    showHelp = false;
    
    ratios.push_back( VideoRatio("1:1", 1, '1') );
    ratios.push_back( VideoRatio("2:3", 480.0/720.0, '2') );
    ratios.push_back( VideoRatio("4:3", 3.0/4.0, '3') );
    ratios.push_back( VideoRatio("5:4", 576.0/720.0, '4') );
    ratios.push_back( VideoRatio("16:9", 9.0/16.0, '5') );
    ratios.push_back( VideoRatio("1.85:1", 1.0/1.85, '6') );
    ratios.push_back( VideoRatio("2.35:1", 1.0/2.35, '7') );
    ratios.push_back( VideoRatio("2.39:1", 1.0/2.39, '8') );
    
    currentRatio = ratios[2];
    
    ofFile lastOpened;
    lastOpened.open("last-opened.txt");
    
    if (lastOpened.exists()) {
        
        // read last opened video url...
        
        string line;
        ifstream data;
        data.open(ofToDataPath("last-opened.txt"));
        
        while(!data.eof()){
            getline(data, line);
            string str = line;
            video->init(str);
            video->load();
            currentName = ofFilePath::getFileName(str);
            currentName = ofFilePath::removeExt(currentName);
        }
        data.close();
        
    }
    
    int i = 1;
    for (auto & r : ratios) {
        hilfe << i++ << " = use ratio " << r.t << endl;
    }
    hilfe << "c = capture screenshot" << endl;
    hilfe << "f = toggle fullscreen" << endl;
    hilfe << "r = random position" << endl;
    hilfe << "TAB = toggle crop / fit mode" << endl;
    hilfe << "SPACE = play / pause" << endl;
    hilfe << "-> = forward 1 frame" << endl;
    hilfe << "<- = back 1 frame" << endl;
    hilfe << "-> + SHIFT = forward 1 second" << endl;
    hilfe << "<- + SHIFT = back 1 second" << endl;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    video->update();
}

string ofApp::getTimeString( float t ) {
    
    float hours = t/60;
    float seconds = fmod(t, 60);
    float millis = (fmod(seconds, 1)*100);
    string human = "";
    if (hours < 10) human += "0";
    human += ofToString( (int)hours );
    human += ":";
    if (seconds < 10) human += "0";
    human += ofToString( (int)seconds );
    human += ":";
    if (millis < 10) human += "0";
    human += ofToString( (int)millis );
    return human;
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(10);
    ofSetColor(255);
    ofDrawBitmapStringHighlight( "drop movie here\npress h for help" , ( ofGetWidth() / 2 ) - 60, ( ofGetHeight() / 2 ) - 10 );
    
    
    
    if (!video->isLoaded()) return;
    
    ofRectangle win = ofGetWindowRect();
    win.height -= uiHeight;
    ofRectangle r(0,0,video->getWidth(), video->getHeight());
    r.scaleTo( win, OF_SCALEMODE_FILL );
    if (!crop) r = win;
    video->draw( r.x, r.y, r.width, r.height );
    
    
    ofPushMatrix();
    ofTranslate( 2, ofGetHeight() - 18);
    if (!video->isPlaying()) ofDrawTriangle(0,0, 7,4, 0, 7);
    if (video->isPlaying()) ofDrawRectangle(1,1, 6,6);
    ofPopMatrix();
    ofSetColor(10);
    ofDrawRectangle( 0, ofGetHeight() - uiHeight, ofGetWidth(), uiHeight);
    ofSetColor(255);
    ofDrawRectangle( 0, ofGetHeight() - uiHeight, video->getPositionAsSeconds()/video->getDuration() * ofGetWidth(), uiHeight);
    
    
    
    
    float a = 0.2;
    float b = 1;
    float c = 1.2;
    int ww = ofGetWidth();
    int hh = ofGetHeight();
    
    if ( ofGetElapsedTimef() >= flashStamp && ofGetElapsedTimef() < flashStamp + c ) {
        ofPushMatrix();
        float scale = ofGetWidth() / img.getWidth();
        scale *= 0.15;
        ofTranslate( ww - (img.getWidth() * scale) - 5, hh - 25  - (img.getHeight() * scale));
        ofScale( scale, scale );
        
        ofSetColor(255);
        
        ofSetColor(255,255,255, ofMap(ofGetElapsedTimef(), flashStamp + b, flashStamp + c, 255, 0, true));
        img.draw( 0,0 );
        ofNoFill();
        ofDrawRectangle(0,0,img.getWidth(),img.getHeight());
        ofFill();
        ofPopMatrix();
        
    }
    if ( ofGetElapsedTimef() >= flashStamp && ofGetElapsedTimef() < flashStamp + a ) {
        ofSetColor(255,255,255, ofMap(ofGetElapsedTimef(), flashStamp, flashStamp + a, 255, 0));
        ofDrawRectangle(0,0,ofGetWidth(),ofGetHeight());
    }
    
    std::stringstream msg;
    msg << (((bool)crop) ? "[crop=" : "[fit=") << currentRatio.t << "]";
    msg << "["  << getTimeString(video->playhead ) << "] " << currentName;
    string msg_ = msg.str();
    ofDrawBitmapString( msg_ , 12, ofGetHeight() - 10 );
    msg.str().clear();
    
    if (showHelp) ofDrawBitmapStringHighlight(hilfe.str(), 0, 15);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    
//    ofLog() << "hello world";
    
    
    if (key == 'h') showHelp = !showHelp;
    
    for (auto & r : ratios ) {
        if (r.c == key) {
            currentRatio = r;
            float w = ofGetWidth();
            ofSetWindowShape(w, (w * r.m) + uiHeight );
        }
    }
    
    if (!video->isLoaded()) return;
    
    
    if (key == 'f' ) ofToggleFullscreen();
    if (key == 'r' ) {
        video->playhead = ofRandom(0, video->getDuration());
    }
    if (key == OF_KEY_TAB) {
        crop = !crop;
    }
    if (key == ' ') {
        video->playing = !video->playing;
    }
    if (key == OF_KEY_RIGHT && ofGetKeyPressed(OF_KEY_SHIFT)) {
        video->playhead += 0.5;
    }
    if (key == OF_KEY_LEFT && ofGetKeyPressed(OF_KEY_SHIFT) ) {
        video->playhead += -0.5;
    }
    if (key == OF_KEY_RIGHT && !ofGetKeyPressed(OF_KEY_SHIFT)) {
        float a = video->getCurrentFrame() + 2;
        while (a >= video->getTotalNumFrames()) a = 0;
        video->setPositionAsFrame( a );
    }
    if (key == OF_KEY_LEFT && !ofGetKeyPressed(OF_KEY_SHIFT) ) {
        float a = video->getCurrentFrame() - 2;
        while (a < 0) a = video->getTotalNumFrames() - 1;
        video->setPositionAsFrame( a );
    }
    
    
    if (key == 'c' ) {
        blip.play();

        float ww = ofGetWidth();
        float hh = ofGetHeight();
        int vw = video->getWidth();
        int vh = video->getHeight();
        
        float winRatio = ww/ (hh-uiHeight);
        float vidRatio = (float)vw/(float)vh;
        
        ofRectangle fboRect;
        if (vidRatio > winRatio) {
            fboRect.width = vw;
            fboRect.height = vw/winRatio;
        } else {
            fboRect.height = vh;
            fboRect.width = vh*winRatio;
            
        }
        
        ofPixels pix;
        ofRectangle r(0,0,video->getWidth(), video->getHeight());
        r.scaleTo( fboRect, OF_SCALEMODE_FILL );
        if (!crop) r = fboRect;
        if (fboRect.width != fbo.getWidth() || fboRect.height != fbo.getHeight() ) {
            fbo.allocate( fboRect.width, fboRect.height, GL_RGB, 8);
            pix.allocate( fboRect.width, fboRect.height, OF_IMAGE_COLOR);
        }
        
        fbo.begin();
        ofClear(0);
        ofFill();
        ofSetColor(255);
        video->draw(  r.x , r.y, r.width, r.height );
        fbo.end();
        
        
        fbo.readToPixels( pix );
        img.setFromPixels( pix );
        
        
        
        
        int p = video->getPositionAsSeconds() * 10000;
        float pp = (float)p/(float)100.0;
        string name = currentName + " "+getTimeString(video->playhead ) + ".jpg";
        string finalPath = ofxStandardPaths::DesktopLocation + "/shotta/" + currentName + "/" + name;
        img.save(finalPath, OF_IMAGE_QUALITY_BEST);
        flashStamp = ofGetElapsedTimef();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

void ofApp::setVideoPositionFromMouse( int x, int y ) {
    float pos = ofMap( x , 0 , ofGetWidth(), 0 , 1);
    video->setPositionAsPercent( pos);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    setVideoPositionFromMouse( x, y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    setVideoPositionFromMouse( x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    currentRatio.t = "custom";
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    if ( dragInfo.files.size() > 0 ) {
        ofLogNotice("ofApp") << "adding file" << dragInfo.files[0];
        
        video->unload();
        video->init( dragInfo.files[0] );
        video->load();
        
        currentName = ofFilePath::getFileName(dragInfo.files[0]);
        currentName = ofFilePath::removeExt(currentName);
        
        ofBuffer dataBuffer;
        dataBuffer.append(dragInfo.files[0]);
        bool fileWritten = ofBufferToFile("last-opened.txt", dataBuffer);
        
    }
}

const string ofxStandardPaths::HomeLocation = ofFilePath::getUserHomeDir();

#ifdef TARGET_OSX
const string ofxStandardPaths::DesktopLocation = ofFilePath::getUserHomeDir()+"/Desktop";
const string ofxStandardPaths::DocumentsLocation = ofFilePath::getUserHomeDir()+"/Documents";
const string ofxStandardPaths::FontsLocation = "/System/Library/Fonts";
const string ofxStandardPaths::ApplicationsLocation = "/Applications";
const string ofxStandardPaths::MusicLocation = ofFilePath::getUserHomeDir()+"/Music";
const string ofxStandardPaths::MoviesLocation = ofFilePath::getUserHomeDir()+"/Movies";
const string ofxStandardPaths::PicturesLocation = ofFilePath::getUserHomeDir()+"/Pictures";
const string ofxStandardPaths::TempLocation = "";
const string ofxStandardPaths::DataLocation = "";
const string ofxStandardPaths::CacheLocation = "";
const string ofxStandardPaths::GenericDataLocation = "/Library/Application\ Support";
const string ofxStandardPaths::RuntimeDataLocation = ofFilePath::getUserHomeDir()+"/Library/Application\ Support";
const string ofxStandardPaths::ConfigLocation = ofFilePath::getUserHomeDir()+"/Library/Preferences";
const string ofxStandardPaths::GenericConfigLocation = ofFilePath::getUserHomeDir()+"/Library/Preferences";
const string ofxStandardPaths::DownloadLocation = ofFilePath::getUserHomeDir()+"/Downloads";
const string ofxStandardPaths::GenericCacheLocation = "/Library/Caches";
const string ofxStandardPaths::AppDataLocation = "";
const string ofxStandardPaths::AppLocalDataLocation = "";
const string ofxStandardPaths::AppConfigLocation = "";
#elif defined(TARGET_LINUX)
const string ofxStandardPaths::DesktopLocation = ofFilePath::getUserHomeDir()+"/Desktop";
const string ofxStandardPaths::DocumentsLocation = ofFilePath::getUserHomeDir()+"/Documents";
const string ofxStandardPaths::FontsLocation = ofFilePath::getUserHomeDir()+"/.fonts";
const string ofxStandardPaths::ApplicationsLocation = "/usr/local/share/applications";
const string ofxStandardPaths::MusicLocation = ofFilePath::getUserHomeDir()+"/Music";
const string ofxStandardPaths::MoviesLocation = ofFilePath::getUserHomeDir()+"/Videos";
const string ofxStandardPaths::PicturesLocation = ofFilePath::getUserHomeDir()+"/Pictures";
const string ofxStandardPaths::TempLocation = "/tmp";
const string ofxStandardPaths::DataLocation = "";
const string ofxStandardPaths::CacheLocation = "";
const string ofxStandardPaths::GenericDataLocation = "/usr/local/share";
const string ofxStandardPaths::RuntimeDataLocation = "";
const string ofxStandardPaths::ConfigLocation = ofFilePath::getUserHomeDir()+"/.config";
const string ofxStandardPaths::GenericConfigLocation = ofFilePath::getUserHomeDir()+"/.config";
const string ofxStandardPaths::DownloadLocation = ofFilePath::getUserHomeDir()+"/Downloads";
const string ofxStandardPaths::GenericCacheLocation = ofFilePath::getUserHomeDir()+"/.cache";
const string ofxStandardPaths::AppDataLocation = "";
const string ofxStandardPaths::AppLocalDataLocation = "";
const string ofxStandardPaths::AppConfigLocation = "";
#elif defined(TARGET_WIN32)
const string ofxStandardPaths::DesktopLocation = ofFilePath::getUserHomeDir()+"/Desktop";
const string ofxStandardPaths::DocumentsLocation = ofFilePath::getUserHomeDir()+"/Documents";
const string ofxStandardPaths::FontsLocation = "";
const string ofxStandardPaths::ApplicationsLocation = "";
const string ofxStandardPaths::MusicLocation = ofFilePath::getUserHomeDir()+"/Music";;
const string ofxStandardPaths::MoviesLocation = ofFilePath::getUserHomeDir()+"/Videos";
const string ofxStandardPaths::PicturesLocation = ofFilePath::getUserHomeDir()+"/Pictures";
const string ofxStandardPaths::TempLocation = ofFilePath::getUserHomeDir+"/AppData/Local/Temp";
const string ofxStandardPaths::DataLocation = "";
const string ofxStandardPaths::CacheLocation = "";
const string ofxStandardPaths::GenericDataLocation = "";
const string ofxStandardPaths::RuntimeDataLocation = "";
const string ofxStandardPaths::ConfigLocation = "";
const string ofxStandardPaths::GenericConfigLocation = "";
const string ofxStandardPaths::DownloadLocation = ofFilePath::getUserHomeDir()+"/Downloads";
const string ofxStandardPaths::GenericCacheLocation = "";
const string ofxStandardPaths::AppDataLocation = "";
const string ofxStandardPaths::AppLocalDataLocation = "";
const string ofxStandardPaths::AppConfigLocation = "";
#elif defined(TARGET_ANDROID)
#elif defined(TARGET_IOS)
#endif
