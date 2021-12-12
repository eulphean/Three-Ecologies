#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  // File
  ofBuffer buffer = ofBufferFromFile("ecologies.txt");
  auto fileText = buffer.getText();
  words = ofSplitString(fileText, " ");
  
  // Video
  video.load("backyard.mov");
  video.play();
  
  // Setup Gui
  settings.setName("Terrain Backyard");
  settings.add(opacityWaitTime.set("Opacity Time", 300, 0, 5000));
  opacityWaitTime.addListener(this, &ofApp::opacityTimeChanged);
  
  videoParams.setName("Video Params");
  videoParams.add(blurAmt.set("Blur Amount", 4, 0, 20));
  
  topStringParams.setName("Top Strings");
  topStringParams.add(topFontSize.set("Top Font Size", 45, 10, 80));
  
  // Top font.
  top.load("cinecav.ttf", topFontSize);
  // Calculate x positions for top strings.
  auto writX = getTextXPos(top, writer);
  auto titX = getTextXPos(top, title);
  auto yrX = getTextXPos(top, year);
  
  topStringParams.add(writerX.set("Writer X Pos", writX, 0, ofGetWidth()));
  topStringParams.add(writerY.set("Writer Y Pos", 300, 0, ofGetHeight()));
  topStringParams.add(titleX.set("Title X Pos", titX, 0, ofGetWidth()));
  topStringParams.add(titleY.set("Title Y Pos", 350, 0, ofGetHeight()));
  topStringParams.add(yearX.set("Year X Pos", yrX, 0, ofGetWidth()));
  topStringParams.add(yearY.set("Year Y Pos", 400, 0, ofGetHeight()));
  topFontSize.addListener(this, &ofApp::loadFonts);
  
  middleStringParams.setName("Middle Strings");
  middleStringParams.add(middleFontSize.set("Middle Font Size", 35, 10, 80));
  
  // Middle font.
  middle.load("cinecav.ttf", middleFontSize);
  // Calculate x positions for middle strings.
  auto quotX = getTextXPos(middle, quote);
  auto quoteWritX = getTextXPos(middle, quoteWriter);
  
  middleStringParams.add(quoteX.set("Quote X Pos", quotX, 0, ofGetWidth()));
  middleStringParams.add(quoteY.set("Quote Y Pos", 450, 0, ofGetHeight()));
  middleStringParams.add(quoteWriterX.set("Quote Writer X Pos", quoteWritX, 0, ofGetWidth()));
  middleStringParams.add(quoteWriterY.set("Quote Writer Y Pos", 500, 0, ofGetHeight()));
  middleFontSize.addListener(this, &ofApp::loadFonts);
  
  bottomStringParams.setName("Bottom Strings");
  bottomStringParams.add(bottomFontSize.set("Bottom Font Size", 30, 10, 80));
  
  // Bottom font.
  bottom.load("cinecav.ttf", bottomFontSize);
  
  bottomStringParams.add(subtitleY.set("Subtitle Y Pos", 700, 0, ofGetHeight()));
  bottomStringParams.add(waitTime.set("Wait Time", 200, 0, 900));
  bottomStringParams.add(maxStringsToShow.set("Max Strings", 6, 1, 20));
  waitTime.addListener(this, &ofApp::waitTimeChanged);
  bottomFontSize.addListener(this, &ofApp::loadFonts);
  
  settings.add(videoParams);
  settings.add(topStringParams);
  settings.add(middleStringParams);
  settings.add(bottomStringParams);
  gui.setup(settings);
  gui.loadFromFile("TerrainBackyard.xml");
  
  // Prepare for Blurring. 
  if(ofIsGLProgrammableRenderer()){
    shaderBlurX.load("shadersGL3/shaderBlurX");
    shaderBlurY.load("shadersGL3/shaderBlurY");
  }else{
    shaderBlurX.load("shadersGL2/shaderBlurX");
    shaderBlurY.load("shadersGL2/shaderBlurY");
  }
  
  fboBlurOnePass.allocate(ofGetWidth(), ofGetHeight());
  fboBlurTwoPass.allocate(ofGetWidth(), ofGetHeight());
  
  length = maxStringsToShow;
}

//--------------------------------------------------------------
void ofApp::update(){
  video.update();
  
  // After a certain time create a new substring
  if (timer == waitTime) {
    auto maxWords = words.size();
    // Empty subtitle
    subtitle = "";
    
    // Create a substring that I want to print.
    for (int i = start; i < start + length; i++) {
      subtitle += words[i] + " ";
    }
    
    // Trim the space from the end.
    subtitle = ofTrimBack(subtitle);
  
    // Logic to set the next start counter.
    auto remainingWords = maxWords - (start + length);
    if (remainingWords == 0) {
      start = 0;
      length = maxStringsToShow;
    } else if (remainingWords > length) {
      start += length;
      length = maxStringsToShow;
    } else {
      start += length;
      length = remainingWords;
    }
    
    // Reset timer.
    timer = 0;
  } else {
    timer++;
  }
  
  prepareBlurTexture();
}

//--------------------------------------------------------------
void ofApp::draw(){
  //----------------------------------------------------------
  ofSetColor(ofColor::white);
  fboBlurTwoPass.draw(0, 0);
  
  // Opacity logic
  opacity = ofMap(opacityTimer, 0, opacityWaitTime, 0, 255, true);
  if (isOn) {
    opacityTimer += 1;
    if (opacityTimer == opacityWaitTime) {
      isOn = false;
    }
  } else {
    opacityTimer -= 1;
    if (opacityTimer == 0) {
      isOn = true;
    }
  }
  
  // Center Logic
  ofPushStyle();
    ofSetColor(ofColor::black, opacity);
  
    // Draw top strings.
    top.drawString(writer, writerX, writerY);
    top.drawString(title, titleX, titleY);
    top.drawString(year, yearX, yearY);
  
    // Draw middle strings.
    middle.drawString(quote, quoteX, quoteY);
    middle.drawString(quoteWriter, quoteWriterX, quoteWriterY);
  
  ofPopStyle();
  
  // Subtitle.
  drawSubtitle();
  
  if (showGui) {
    gui.draw();
    ofShowCursor();
  } else {
    ofHideCursor(); 
  }
}

void ofApp::loadFonts(float & param) {
  top.load("cinecav.ttf", topFontSize);
  middle.load("cinecav.ttf", middleFontSize);
  bottom.load("cinecav.ttf", bottomFontSize);
}

void ofApp::prepareBlurTexture() {
  // Pass 1 begin
  fboBlurOnePass.begin();
    shaderBlurX.begin();
    shaderBlurX.setUniform1f("blurAmnt", blurAmt);
    video.draw(0, 0, ofGetWidth(), ofGetHeight());
    shaderBlurX.end();
  fboBlurOnePass.end();
  
  // Pass 2 begin
  fboBlurTwoPass.begin();
    shaderBlurY.begin();
    shaderBlurY.setUniform1f("blurAmnt", blurAmt);
    fboBlurOnePass.draw(0, 0);
    shaderBlurY.end();
  fboBlurTwoPass.end();
}

// Draw the Subtitles Bar
void ofApp::drawSubtitle() {
  if (subtitle.size() > 0) {
    // Calculate x position for bottom string
    auto subtitleX = getTextXPos(bottom, subtitle);
    
    // A rectangle and inside that draw the subtitles
    auto rect = bottom.getStringBoundingBox(subtitle, subtitleX, subtitleY);
    ofPushStyle();
      ofSetColor(ofColor::black, 200);
      ofDrawRectangle(rect.x - 10, rect.y - 10, rect.width + 20, rect.height + 20);
    ofPopStyle();
    
    ofPushStyle();
      ofSetColor(ofColor::white);
      bottom.drawString(subtitle, subtitleX, subtitleY);
    ofPopStyle();
  }
}

void ofApp::waitTimeChanged(int & newWaitTime) {
  // Reset timer to start from 0 again.
  timer = 0;
}

void ofApp::exit() {
   gui.saveToFile("TerrainBackyard.xml");
}

float ofApp::getTextXPos(ofTrueTypeFont font, string s) {
  auto sw = font.stringWidth(s);
  return ofGetWidth()/2 - sw/2;
}

void ofApp::opacityTimeChanged(int & newTime) {
  // Reset current timer to start from the beginning
  opacityTimer = 0;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (key == 'h') {
    showGui = !showGui;
  }
}

// Title (Fade In)/(Fade Out)

// Send it to Jaclyn (press h to hide GUI)

// Post processing of the title text (not really required)
