#pragma once

#include "ofMain.h"
#include "Video.h"

class ofxStandardPaths
{
public:
    static const std::string DesktopLocation;
    static const std::string DocumentsLocation;
    static const std::string FontsLocation;
    static const std::string ApplicationsLocation;
    static const std::string MusicLocation;
    static const std::string MoviesLocation;
    static const std::string PicturesLocation;
    static const std::string TempLocation;
    static const std::string HomeLocation;
    static const std::string DataLocation;
    static const std::string CacheLocation;
    static const std::string GenericDataLocation;
    static const std::string RuntimeDataLocation;
    static const std::string ConfigLocation;
    static const std::string GenericConfigLocation;
    static const std::string DownloadLocation;
    static const std::string GenericCacheLocation;
    static const std::string AppDataLocation;
    static const std::string AppLocalDataLocation;
    static const std::string AppConfigLocation;
};


class ofApp;


struct VideoRatio {
    string t;
    float m;
    char c;
    VideoRatio() {
        
    }
    VideoRatio( string t_, float m_ , char c_) {
        t = t_;
        m = m_;
        c = c_;
    }
};

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void setVideoPositionFromMouse( int x, int y );
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    string getTimeString( float t );
    
    ofImage img;
    string currentName;
    ofxFlex::Video * video;
    ofSoundPlayer blip;

    vector<VideoRatio> ratios;
    bool crop;
    int uiHeight;
    VideoRatio currentRatio;
    float flashStamp;
    ofFbo fbo;
    bool showHelp;
    stringstream hilfe;
    
		
};

