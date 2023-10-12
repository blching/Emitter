#include "ofApp.h"

// spawnSprite - we override this function in the Emitter class to spawn our 
// "custom" Agent sprite.
//
void AgentEmitter::spawnSprite() {
    for (int i = 0; i < agents; i++) {
        float randX = rand() % ofGetWindowWidth();
        float randY = rand() % ofGetWindowHeight();
        
        Agent sprite;
        if (haveChildImage) sprite.setImage(childImage);
        sprite.velocity = velocity;
        sprite.lifespan = lifespan;
        sprite.pos = glm::vec3(randX, randY, 0);
        sprite.birthtime = ofGetElapsedTimeMillis();
        sprite.rot = rand() % 360;
        sprite.bShowImage = false;
        
        sys->add(sprite);
    }
}

//  moveSprite - we override this function in the Emitter class to implment
//  "following" motion towards the target
//
void AgentEmitter::moveSprite(Sprite* sprite) {
	
	// Get a pointer to the app so we can access application
	// data such as the target sprite.
	//
	ofApp* theApp = (ofApp*)ofGetAppPtr();

	// rotate sprite to point towards target
	//  - find vector "v" from sprite to target
	//  - set rotation of sprite to align with v
    
	glm::vec3 v = glm::normalize(theApp->target->pos - sprite->pos);
	glm::vec3 u = glm::vec3(0, -1, 0); //Nutural Heading Vector
    glm::vec3 h = glm::vec3(sin(glm::radians(sprite->rot)), -cos(glm::radians(sprite->rot)), 0); //Heading of Sprite vector

	float a = glm::orientedAngle(u, v, glm::vec3(0, 0, 1)); //Angle between vertical and target
    float a2 = glm::orientedAngle(h, v, glm::vec3(0,0,1)); //Angle between sprite and target
    
	//sprite->rot = glm::degrees(a);
    
    if (sprite->rot != glm::degrees(a)) {
        if (turnSpeed > abs(glm::degrees(a2))) { //Checks if turnSpeed will overshoot the angle, set angle directly
            sprite-> rot = glm::degrees(a);
        } else if (a2 > 0) {
            sprite->rot += turnSpeed;
        } else {
            sprite->rot -= turnSpeed;
        }
    }
    
    //sprite->velocity = glm::length(sprite->velocity) * v;
        
    //Moves sprite in direction of its heading vector, using its velocity
    sprite->velocity = glm::length(sprite->velocity) * h;
    
	Emitter::moveSprite(sprite);

}


//--------------------------------------------------------------
void ofApp::setup(){
    
    state = GameState::START;
	ofSetVerticalSync(true);
    
    font = new ofTrueTypeFont();
    info = new ofTrueTypeFont();
    font->load("Raleway-Medium.ttf", 32);
    info->load("Raleway-Medium.ttf", 18);
    
	// create an image for sprites being spawned by emitter
	//
	if (defaultImage.load("images/octoSprite.png")) {
		imageLoaded = true;
	}
	else {
		cout << "Can't open image file" << endl;
	}
    
    background.load("images/backGroundsmall.png");
    
	ofSetBackgroundColor(ofColor::black);

	

	// create an emitter
	//

	emitter = new AgentEmitter();  // C++ polymorphism

	emitter->pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
	emitter->drawable = false; //Hides emmitter
	if (imageLoaded) emitter->setChildImage(defaultImage);
	emitter->start();

	// create a target sprite to chase
	//
	target = new Sprite();
	target->pos = glm::vec3(400, 100, 0);


	gui.setup();
	gui.add(rate.setup("rate", 1, .5, 10));
	gui.add(life.setup("life", 5, .1, 15));
	gui.add(velocity.setup("Agent Velocity", ofVec3f(0, -200, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	gui.add(scale.setup("Agent Scale", .5, .1, 1.0));
	//gui.add(rotationSpeed.setup("Rotation Speed (deg/Frame)", 0, 0, 10));
    gui.add(turnSpeed.setup("Agent Turn Speed", 1, 0.5, 7.5));
    
    gui.add(playerSpeed.setup("Player Speed", 5, 3, 10));
    gui.add(playerRot.setup("Player rotation speed (deg)", 2, .5, 5));
    gui.add(playerScale.setup("Player Scale", 1, 0.1, 3));
    
    gui.add(nAgents.setup("Number of Agents", 1, 1, 10));
    gui.add(nEnergy.setup("Player Energy", 10, 1, 25));
    
    gui.add(agentSprite.setup("Toggle Agent Sprite", true));
    gui.add(level.setup("Level of Difficulty", 1,1,3));
    
    currentLevel == 1;
	bHide = false;

}

//--------------------------------------------------------------
void ofApp::update() {
    if (keyMap[' ']) {
        if (state == GameState::START || state == GameState::END) {
            //Resets the game
            state = GameState::PLAY;
            playerEnergy = totalPlayerEnergy;
            emitter->sys->sprites.clear();
            emitter->start();
            startTime = ofGetElapsedTimeMillis();
        }
    }
    
    if (level == 1 && currentLevel != 1) {
        rate = 1;
        life = 5;
        nAgents = 1;
        nEnergy = 15;
        currentLevel = 1;
    }
    
    if (level == 2 && currentLevel != 2) {
        rate = 1.5;
        life = 7.5;
        nAgents = 2;
        nEnergy = 10;
        currentLevel = 2;
    }
    
    if (level == 3 && currentLevel != 3) {
        rate = 3;
        life = 10;
        nAgents = 5;
        nEnergy = 7;
        currentLevel = 3;
    }
    emitter->setRate(rate);
    emitter->setLifespan(life * 1000);    // convert to milliseconds
    emitter->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
    emitter->setTurnSpeed(turnSpeed);
    emitter->setnAgents(nAgents);
    emitter->update();
    
    if (totalPlayerEnergy != nEnergy) playerEnergy = nEnergy;
    
    totalPlayerEnergy = nEnergy;
    
    if (agentSprite) {
        for (int i = 0; i < emitter->sys->sprites.size(); i++) {
            emitter->sys->sprites[i].bShowImage = false;
        }
    } else {
        for (int i = 0; i < emitter->sys->sprites.size(); i++) {
            emitter->sys->sprites[i].bShowImage = true;
        }
    }
    
    //Allow Scalability
    float s = playerScale;
    target->scale = glm::vec3(s, s, 1);
    
    
    target->updateHitBox();
    
    for (int i = 0; i < emitter->sys->sprites.size(); i++) {
        
        // Get values from sliders and update sprites dynamically, and update hitBox
        //
        float sc = scale;
        float rs = rotationSpeed;
        emitter->sys->sprites[i].scale = glm::vec3(sc, sc, sc);
        emitter->sys->sprites[i].rotationSpeed = rs;
        emitter->sys->sprites[i].updateHitBox();
    }
    
    //Checks if target and each sprite is in each others' hitbox
    for (int i = 0; i < emitter->sys->sprites.size(); i++) {
        float distance = glm::length(emitter->sys->sprites[i].pos - target->pos);
        float combinedHitBox = emitter->sys->sprites[i].hitBox + target->hitBox;
        
        glm::vec3 inBetween = (target->pos + emitter->sys->sprites[i].pos) / 2;
        glm::vec3 headerPoint = (emitter->sys->sprites[i].pos + emitter->sys->sprites[i].rot * emitter->sys->sprites[i].height/2);
        
        if (distance < combinedHitBox) { //Check both headerpoint and inbetween point of player and sprites
            if (target->inside(inBetween) && emitter->sys->sprites[i].inside(inBetween)) {
                emitter->sys->sprites.erase(emitter->sys->sprites.begin() + i);
                playerEnergy--;
                //cout << playerEnergy << endl;
                
                if (playerEnergy == 0) {
                    emitter->stop();
                    state = GameState::END; //End game if energy is equal to 0
                    totalSurvivedTime = elapsedTime;
                }
            if (target->inside(headerPoint) && emitter->sys->sprites[i].inside(headerPoint)) {
                emitter->sys->sprites.erase(emitter->sys->sprites.begin() + i);
                playerEnergy--;
                //cout << playerEnergy << endl;
                
                if (playerEnergy == 0) {
                    emitter->stop();
                    state = GameState::END; //End game if energy is equal to 0
                }
                
            }
                
            }
        }
    }
    
    //Movement for player
    float speed = playerSpeed;
    float rot = playerRot;
    
    glm::vec3 h = glm::vec3(sin(glm::radians(target->rot)), -cos(glm::radians(target->rot)), 0);
    
    float hitBoxRadius = target->hitBox/2;
    glm::vec3 expectedForPos = target->pos + (h*speed);
    glm::vec3 expectedBackPos = target->pos - (h*speed);
    
    //Detects if the estimated future location of the player will be out of bounds
    //Uses offset provied by hitbox to prevent half of player from exiting frame
    //If passes, moves along the heading vector h
    
    if (expectedForPos.x+hitBoxRadius < ofGetWindowWidth() && expectedForPos.y+hitBoxRadius < ofGetWindowHeight()) {
        if (expectedForPos.x-hitBoxRadius > 0 && expectedForPos.y-hitBoxRadius > 0) {
            if (keyMap[OF_KEY_UP]) {
                target->pos += h * speed;
            }
        }
    }
    
    
    
    if (expectedBackPos.x+hitBoxRadius < ofGetWindowWidth() && expectedBackPos.y+hitBoxRadius < ofGetWindowHeight()) {
        if (expectedBackPos.x-hitBoxRadius > 0 && expectedBackPos.y-hitBoxRadius > 0) {
            if (keyMap[OF_KEY_DOWN]) {
                target->pos -= h * speed ;
            }
        }
    }
    
    
    if (keyMap[OF_KEY_RIGHT]) {
        target->rot += rot;
    }
    
    if (keyMap[OF_KEY_LEFT]) {
        target->rot -= rot;
    }
    
}


//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(ofColor::white);
    glEnable(GL_DEPTH);
    background.draw(0,0,1);
    ofSetColor(ofColor::yellow);

    switch (state) {
    
        case GameState::START: {
            ofSetColor(ofColor::white);
            string begin = "Press Spacebar to Begin!";
            font->drawString(begin, ofGetWindowWidth()/2-font->stringWidth(begin)/2, ofGetWindowHeight()/2);
            ofSetColor(ofColor::yellow);
            break;
        }
        case GameState::PLAY: {
                        
            emitter->draw();
            target->draw();
            
            if (!bHide) {
                gui.draw();
            }
            string pEnergy = to_string(playerEnergy);
            string tEnergy = to_string(totalPlayerEnergy);
            int fps = ofGetFrameRate();
            string fpsString = to_string(fps);
            
            elapsedTime = ofGetElapsedTimeMillis() - startTime;
            elapsedTime = elapsedTime / 1000; //Convert to seconds
            
            string timeString = to_string(elapsedTime);
            timeString.substr(0, timeString.find("."));

            string eng = pEnergy + "/" + tEnergy;
            info->drawString(eng, ofGetWindowWidth()-info->stringWidth(eng), info->stringHeight(tEnergy));
            info->drawString(fpsString, ofGetWindowWidth()-info->stringWidth(fpsString), info->stringHeight(fpsString)+info->stringHeight(tEnergy));
            info->drawString(timeString, 0, ofGetWindowHeight());
            
            
            break;
        }
        case GameState::END: {
            ofSetColor(ofColor::white);
            //Centers both lines of game over
            string over1 = "GAME OVER";
            string over2 = "Press Spacebar to Restart";
            string time = "You survived: " + to_string(totalSurvivedTime) + " seconds!";
            font->drawString(over1, ofGetWindowWidth()/2-font->stringWidth(over1)/2, ofGetWindowHeight()/2);
            font->drawString(over2, ofGetWindowWidth()/2-font->stringWidth(over2)/2, ofGetWindowHeight()/2+font->stringHeight(over2));
            font->drawString(time, ofGetWindowWidth()/2-font->stringWidth(time)/2, ofGetWindowHeight()/2+font->stringHeight(time)+font->stringHeight(over2));
            break;
        }
    }
}


//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
//	cout << "mouse( " << x << "," << y << ")" << endl;
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (bDrag) {
		glm::vec3 p = glm::vec3(x, y, 0);
		glm::vec3 delta = p - lastMousePos;
		target->pos += delta;
		lastMousePos = p;
	}

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	glm::vec3 pos = glm::vec3(x, y, 0);
	if (target->inside(pos)) {
		bDrag = true;
		lastMousePos = pos;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bDrag = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

void ofApp::keyPressed(int key) {
	switch (key) {
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bHide = !bHide;
		break;
            
    /* case ' ':
        if (state == GameState::START || state == GameState::END) {
            //Resets the game
            state = GameState::PLAY;
            playerEnergy = totalPlayerEnergy;
            emitter->sys->sprites.clear();
        } */
	}
    
    keyMap[key] = true;

}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case OF_KEY_LEFT:
	case OF_KEY_RIGHT:
	case OF_KEY_UP:
	case OF_KEY_DOWN:
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	}
    
    keyMap[key] = false;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

