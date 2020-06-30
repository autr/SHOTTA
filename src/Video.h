#pragma once

#include "ofMain.h"
#include "Source.h"

#if TARGET_RASPBERRY_PI && defined(OFXOMXPLAYER)
    #define USING_OMX
    #include "ofxOMXPlayer.h"
#elif defined(TARGET_OSX) && defined(OFXADDON_OFXHAPAVPLAYER)
    #define USING_HAP
    #include "ofxHAPAVPlayer.h"
#endif

namespace ofxFlex {

    class Video : public Source {
    private:
        
        void setPositionAsSeconds(float position_) {
            if (source == nullptr) return;
//            ofLog() << "set position as seconds" << position_ << getDuration();
            source->setPosition(1.0/getDuration() * position_);
        }
        bool bSafeToDraw;
        
        /*-- platform-specific players --*/
        
        #if defined(USING_OMX)
        ofxOMXVideoPlayer * source;
        #elif defined(USING_HAP)
        ofxHAPAVPlayer * source;
        #else
        ofVideoPlayer * source;
        #endif
        
        /*-- util --*/
        
        ofFile * file;
        float previousPlayhead;
        
        
    public:
        using Source::Source;
        
        
        /*-- timeline --*/
        
        int timelineWidth = 1280;
        int timelineHeight = 2;
        ofFbo timelineFbo;
        float timelineScrapeStamp = 0;
        
        /*-- timeout --*/
        
        int newPlayheadCount = 0;
        int maxNewPlayheads = 120;
        float timeoutPlayhead = 0;
        
        
        /*-- ofParameters --*/
        
        ofParameterGroup group;
        ofParameter<int> timelineIdx;
        ofParameter<bool> bIsScraping = false;
        ofParameter<float> playhead;
        ofParameter<bool> playing;
        ofParameter<float> volume;
        #if defined(USING_HAP)
        ofParameter<float> speed;
        #endif
        
        Video() {
            
            #if defined(USING_OMX)
            ofLogNotice("ofxFlex::Video") << "loading new omx player";
            source = new ofxOMXVideoPlayer();
            #elif defined(USING_HAP)
            ofLogNotice("ofxFlex::Video") << "loading new hap player";
            source = new ofxHAPAVPlayer();
            speed = 1.0;
            #else
            ofLogNotice("ofxFlex::Video") << "loading new of player";
            source = new ofVideoPlayer();
            #endif
            
            
            bSafeToDraw = false;
            
            group.setName("Video");
            group.add( playhead.set("playhead", 0, 0, 1) );
            group.add( playing.set("playing", true) );
            
            #if defined(USING_HAP)
            ofLogNotice("[ofxFlex::Video]") << "sloadet to OFXHAPAVPLAYER";
            group.add( speed.set("speed", 1, -2, 2) );
            group.add( bIsScraping.set("scrape", false) );
            group.add( timelineIdx.set("scrape position", 0, 0, timelineWidth) );
            #endif
        }
        
        ~Video() {
            
        }


        
        /*-- methods --*/
        
        bool isPlaying() {
            return source->isPlaying();
        }
        bool isPaused() {
            return source->isPaused();
        }
        
        void setLoopState( ofLoopType state_ ) {
            if (source == nullptr) return;
            source->setLoopState( state_ );
        }
        int getLoopState( ) {
            if (source == nullptr) return OF_LOOP_NONE;
            return source->getLoopState();
        }

        void setVolume(float vol) {
//            source->setVolume(vol);
        }
        
        void init( ofFile file_ ) {
            file = new ofFile(file_.getAbsolutePath());
        }
        void init( string filename_ ) {
            file = new ofFile( filename_ );
        }

        bool isLoaded() {
            
            if (file == nullptr) return false;
            if (source == nullptr) return false;
            if (source == NULL) return false;

            #if defined(USING_OMX)
            return source->isOpen();
            #elif defined(USING_HAP)
            return source->isLoaded();
            #else
            return source->isInitialized();
            #endif
        }
        void load() { // x
            
            bSafeToDraw = false;
            source->load( file->getAbsolutePath() );
            previousPlayhead = 0;
            playhead = 0;
            playing = true;
            playhead.setMax( source->getDuration() );
            source->play();
        }
        
        void unload() {
            ofLogNotice("ofxFlex::Video") << "closing";
            if (source != nullptr) source->close();
            ofLogNotice("ofxFlex::Video") << "deleting";
//            if (source != nullptr) delete source;
        }
        void draw(float x, float y, float w, float h) {
            if (isLoaded() && bSafeToDraw) source->draw(x,y,w,h);
        }
        

        float getPositionAsSeconds() {

            #if defined(USING_OMX)
            source = new ofxOMXVideoPlayer();
            #elif defined(USING_HAP)
            return source->getPosition() * source->getDuration();
            #else
            return source->getPosition();
            #endif
        }
        
        int getCurrentFrame() {
            return source->getCurrentFrame();
        }
        int getTotalNumFrames() {
            return source->getTotalNumFrames();
        }
        
        void setPositionAsFrame(int frame_) {
            if (source == nullptr) return;
            playhead =  getDuration()/(float)getTotalNumFrames()*(float)frame_ ;
            source->setFrame( frame_ );
        }
        void setPositionAsPercent(float position_) {
            if (source == nullptr) return;
            playhead = position_ * getDuration();
//            source->setPosition(position_);
        }
        float getDuration() {
            if (source == nullptr) return;
            return source->getDuration();
        }
        void update() {
            
            
            if (bIsScraping && isLoaded() ) {
                
                if (!timelineFbo.isAllocated()) timelineFbo.allocate(timelineWidth, timelineHeight, GL_RGBA);
                bool newFrame = isFrameNew();
                bool timeout = (ofGetElapsedTimef() - timelineScrapeStamp) > 2;
                
                if ( newFrame  || timeout ) {
                    timelineIdx += 1;
                    timelineScrapeStamp = ofGetElapsedTimef();
                    float iter = source->getDuration()/(float)timelineWidth;
                    float position =  iter * timelineIdx ;
                    setPositionAsSeconds( position  );
                    
                    timelineFbo.begin();
                    ofSetColor(255);
//                    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
                    source->draw( timelineIdx, 0, 1, timelineHeight );
//                    source->draw( 0, timelineIdx, timelineWidth, 1 );
                    timelineFbo.end();
                }
                
                source->update();
                
                if (timelineIdx >= timelineWidth ) {
                    bIsScraping = false;
                }
                
            } else if ( isLoaded() ) {

                /*-- set play and pause --*/
                
                bool isPlaying = !source->isPaused();
                if (playing != isPlaying && playing ) {
                    ofLogNotice("ofxFlex::Video") << "set play";
                    source->setPaused(false);
                }
                if (playing != isPlaying && !playing ) {
                    ofLogNotice("ofxFlex::Video") << "set pause";
                    source->setPaused(true);
                }
                
                
                /*-- set speed --*/
                
                
                #if defined(USING_HAP)
                if (speed != source->getSpeed() && !source->isPaused()) {
                    ofLogNotice("ofxFlex::Video") << "set speed" << speed;
                    source->setSpeed(speed);
                }
                #endif


                /*-- set playhead --*/
                
                bool newPlayhead = playhead != previousPlayhead;
                bool newFrame = isFrameNew();
                
                if (newPlayhead) {
                    ofLogNotice("ofxFlex::Video") << "skipping playhead to..." << (float)playhead << getPositionAsSeconds();
                    setPositionAsSeconds( playhead );
                } else if (newPlayhead) {
//                    if (newPlayheadCount > maxNewPlayheads) {
//                        ofLogNotice("ofxFlex::Video") << "retriggering timeout playhead..." << playhead << getPositionAsSeconds();
//                        setPositionAsSeconds( playhead );
//                        newPlayheadCount = 0;
//                    }
//                    newPlayheadCount += 1;
                }
                
                source->update();


                if (newFrame) {
//                    ofLogNotice("ofxFlex::Video") << "received new frame..." << (float)playhead << getPositionAsSeconds();
                    playhead = getPositionAsSeconds();
                    previousPlayhead = playhead;
                    newPlayheadCount = 0;
                }
                bSafeToDraw = true;
            }
        }
        
        float getSpeed() {
            return source->getSpeed();
        }
        void setSpeed(float speed_) {
            #if defined(USING_HAP)
            speed = speed_;
            #endif
        }
        
        ofTexture & getTexture() {

            #if defined(USING_OMX)
            return source->getTextureReference();
            #elif defined(USING_HAP)
            return source->getTexture();
            #else
            return source->getTexture();
            #endif
        }
        ofPixels & getPixels() {
            return source->getPixels();
        }
        float getWidth() {
            return source->getWidth();
        }
        float getHeight() {
            return source->getHeight();
        }
        bool isFrameNew() {
            return source->isFrameNew();
        }
        void setUsePixels(bool bUsePixels_) {
            #if defined(USING_HAP)
            source->setUsePixels(bUsePixels_);
            #endif
        }
        
    };


};
