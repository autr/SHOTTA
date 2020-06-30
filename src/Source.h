#pragma once
#include "ofMain.h"

namespace ofxFlex {

	class Source {
    protected:
        ofTexture texture;
        ofPixels pixels;
        
	public:
        
        virtual void update() {};
        virtual ofTexture & getTexture() { return texture; };
        virtual ofPixels & getPixels() {  return pixels; };

	};



};

#include "Video.h"
