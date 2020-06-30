#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( int argc, char* argv[] ){
    
    ofGLFWWindowSettings settings;

    ofLog::setAutoSpace(true);
//
//    settings.setSize(300, 300);
//    settings.setPosition(ofVec2f(300,0));
//    settings.resizable = true;
//    shared_ptr<ofAppBaseWindow> winA = ofCreateWindow(settings);
//    shared_ptr<ofControls> appA(new ofControls);
    
    
    settings.setSize(400, 305);
    settings.setPosition(ofVec2f(0,0));
    settings.resizable = true;
    shared_ptr<ofAppBaseWindow> winB = ofCreateWindow(settings);
    shared_ptr<ofApp> appB(new ofApp);
    
    
    /* Link both */
//
//    appA->videoApp = appB;
//    appA->name = "A";
//    appB->controlsApp = appA;
//    appB->name = "B";
    
//    ofRunApp(winA, appA);
    ofRunApp(winB, appB);
    ofRunMainLoop();

}
