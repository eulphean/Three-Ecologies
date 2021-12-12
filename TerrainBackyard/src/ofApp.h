#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    void exit();
		void keyPressed(int key);
  
    void drawSubtitle();
    float getTextXPos(ofTrueTypeFont font, string s);
    void waitTimeChanged(int & newWaitTime);
    void prepareBlurTexture();
    void loadFonts(float & newParam);
    void opacityTimeChanged(int & newOpacityTime);
  
    // File reading
    ofFile ecologyFile;
    std::vector<string> words;
    string subtitle; 

    // Video and fonts
    ofVideoPlayer video;
    ofTrueTypeFont top;
    ofTrueTypeFont middle;
    ofTrueTypeFont bottom;
  
    // Flags
    bool showGui = true; 
  
    // GUI
    ofxPanel gui;
    ofParameterGroup settings;
    ofParameter<int> opacityWaitTime; 
  
    // Video params
    ofParameterGroup videoParams;
    ofParameter<float> blurAmt; 

    // General settings
    ofParameterGroup topStringParams;
    ofParameter<float> topFontSize;
    ofParameter<float> writerX;
    ofParameter<float> writerY;
    ofParameter<float> titleX;
    ofParameter<float> titleY;
    ofParameter<float> yearX;
    ofParameter<float> yearY;
  
    ofParameterGroup middleStringParams;
    ofParameter<float> middleFontSize;
    ofParameter<float> quoteX;
    ofParameter<float> quoteY;
    ofParameter<float> quoteWriterX;
    ofParameter<float> quoteWriterY;
  
    ofParameterGroup bottomStringParams;
    ofParameter<float> bottomFontSize; 
    ofParameter<float> subtitleY;
    ofParameter<int> waitTime; // Add a callback to reset the current time to 0
    ofParameter<int> maxStringsToShow; 
  
    // Top Strings
    string writer = "Felix Guttari";
    string title = "The Three Ecologies";
    string year = "1989";
  
    // Middle Strings
    string quote = "There is an ecology of bad ideas, just as there is an ecology of weeds.";
    string quoteWriter = "Gregory Bateson";
  
    int timer = 0;
    int start = 0;
    int length;
  
    // Blur shader
    ofShader shaderBlurX;
    ofShader shaderBlurY;

    // Blur fbos. 
    ofFbo fboBlurOnePass;
    ofFbo fboBlurTwoPass;
  
    int opacityTimer = 0;
    bool isOn = true;
  
    int opacity = 0; 
};
