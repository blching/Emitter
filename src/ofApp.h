#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Emitter.h"
#include "Shape.h"
#include "Sprite.h"



class Agent : public Sprite {
public:
	Agent() {
//		Sprite::Sprite();
//		cout << "in Agent Constuctor" << endl;
	}
};

class AgentEmitter : public Emitter {
public:
	void spawnSprite();
	void moveSprite(Sprite*);
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
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        enum class GameState {
            START, PLAY, END
        };
    
		Emitter  *emitter = NULL;
		Sprite* target = NULL;

		ofImage defaultImage;
		ofVec3f mouse_last;
		bool imageLoaded;
		bool bDrag = false;
		glm::vec3 lastMousePos;
        ofImage background;
    
		// Some basic UI
		//
		bool bHide;
		ofxFloatSlider rate;
		ofxFloatSlider life;
		ofxVec3Slider velocity;
		ofxLabel screenSize;
		ofxFloatSlider scale;
		ofxFloatSlider rotationSpeed;
        ofxFloatSlider turnSpeed;
        ofxFloatSlider playerSpeed;
        ofxFloatSlider playerRot;
        ofxFloatSlider playerScale;
        ofxIntSlider nAgents;
        ofxIntSlider nEnergy;
        ofxIntSlider level;
        ofxToggle agentSprite;
        ofxPanel gui;
    
        map<int, bool> keyMap;
    
        int playerEnergy;
        int totalPlayerEnergy;
        float elapsedTime;
        float startTime;
        int currentLevel;
        float totalSurvivedTime;
    
        GameState state = GameState::START;
        ofTrueTypeFont *font;
        ofTrueTypeFont *info;
};
