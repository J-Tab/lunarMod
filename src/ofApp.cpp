
//--------------------------------------------------------------
//
//  10.15.19
//
//  CS 134 Midterm - Fall 2019 - Startup Files
// 
//   author:  Kevin M. Smith   

#include "ofApp.h"
#include "Util.h"
#include <glm/gtx/intersect.hpp>

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup() {

	//debug GUI
	gui.setup();
	gui.add(axisButton.setup("Axis Toggle", false));

	ofSetFrameRate(60);
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;

	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	cam.disableMouseInput();

	topCam.setNearClip(.1);
	topCam.setFov(65.5);
	topCam.setPosition(0, 15, 0);
	topCam.lookAt(glm::vec3(0, 0, 0));

	// set current camera;
	//
	theCam = &cam;

	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofEnableDepthTest();

	// load BG image
	//
	bBackgroundLoaded = backgroundImage.load("images/starfield-plain.jpg");


	// setup rudimentary lighting 
	//
	initLightingAndMaterials();

	//setup terrain
	terrain.loadModel("geo/moon-houdini.obj");
	terrain.setRotation(0, -180, 1, 0, 0);
	terrain.setScaleNormalization(false);

	//boundingBox = meshBounds(terrain.getMesh(0));



	// load lander model
	//
	if (lander.loadModel("geo/lander.obj")) {
		lander.setScaleNormalization(false);
		lander.setScale(.5, .5, .5);
		lander.setRotation(0, -180, 1, 0, 0);
		lander.setPosition(0, 0, 0);
		bLanderLoaded = true;
	}
	else {
		cout << "Error: Can't load model" << "geo/lander.obj" << endl;
		ofExit(0);
	}

	// setup particle system
	// attach thrusters to lander
	yThruster.setPosition(landerParticle.position);
	yThruster.setRate(1000);


	//Tree creation
	treeAl.create(terrain.getMesh(0), 6);
	
}





void ofApp::update() {
	timer++;
	//Heading depending on arrow keys
	ofVec3f tempVec = ofVec3f(0, 0, 0);
	ofVec3f turbVec = ofVec3f(0, 0, 0);
	//W and S headings
	if ((wKeyPressed && sKeyPressed) || (!wKeyPressed && !sKeyPressed)) {
		tempVec.y = -.5;
	}
	else if (wKeyPressed) {
		tempVec.y = 5.0;
		yThruster.start();
		yThruster.oneShot = true;
	}
	else if (sKeyPressed) {
		tempVec.y = -5;
		yThruster.start();
		yThruster.oneShot = true;
	}

	//Up and Down Heading
	
	if (upPressed && downPressed) {
		
	}
	else if (upPressed) {
		tempVec.z = -headingAcceleration;
		yThruster.start();
		yThruster.oneShot = true;
	}
	else if (downPressed) {
		tempVec.z = headingAcceleration;
		yThruster.start();
		yThruster.oneShot = true;
	}
	//Left and Right Heading
	if (leftPressed && rightPressed) {
	}
	else if (leftPressed) {
		tempVec.x = -headingAcceleration;
		yThruster.start();
		yThruster.oneShot = true;
	}
	else if (rightPressed) {
		tempVec.x = headingAcceleration;
		yThruster.start();
		yThruster.oneShot = true;
	}

	
	
	
	
	//Rotate lander using 'a' and 'd' keys
	if (counterClockwiseRot) {
		rotationAccel = 1;
	}
	else if (clockwiseRot) {
		rotationAccel = -1;
	}
	else {
		rotationAccel = 0;
	}

	//Deceleration when nothing hit
	if (rotationVel > 0) {
		rotationVel -= .05;
	}
	else if (rotationVel < 0) {
		rotationVel += .05;
	}

	rotationVel = rotationVel + (rotationAccel / 5);
	yRotationAngle = yRotationAngle + rotationVel*.2;
	

	
	//Turbulance effect when not on the floor; adds slight randomness to the heading vector
	if (landerParticle.position.y > 0) {
		float v = float(rand()) / (float(RAND_MAX) + 2.0);
		int rAngle = rand() % 360;
		turbVec.x += v;
		turbVec = turbVec.rotate(rAngle, ofVec3f(0, 1, 0));
		heading = tempVec.rotate(yRotationAngle, ofVec3f(0, 1, 0)) + turbVec;
	}
	else {
		heading = tempVec.rotate(yRotationAngle, ofVec3f(0, 1, 0));
	}
		
	
	
	lander.setRotation(1, yRotationAngle, 0, 1, 0);
	landerParticle.acceleration = heading;

	//Update physics movement for the lander
	landerParticle.integrate();
	if (landerParticle.position.y < 0){
		landerParticle.position.y = 0;
	}
	lander.setPosition(landerParticle.position.x,landerParticle.position.y,landerParticle.position.z);

	//Update thruster exhaust

	yThruster.setPosition(landerParticle.position+ofVec3f(0,1,0));


	//LEM update
	yThruster.update();



}

//--------------------------------------------------------------
void ofApp::draw() {


	// draw background image
	//
	if (bBackgroundLoaded) {
		ofPushMatrix();
		ofDisableDepthTest();
		ofSetColor(50, 50, 50);
		ofScale(2, 2);
		backgroundImage.draw(-200, -100);
		ofEnableDepthTest();
		ofPopMatrix();
	}

	theCam->begin();

	ofPushMatrix();

		// draw a reference grid
		//
		ofPushMatrix();
		ofRotate(90, 0, 0, 1);
		ofSetLineWidth(1);
		ofSetColor(ofColor::dimGray);
		ofDisableLighting();
		ofDrawGridPlane();
		ofPopMatrix();

	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		terrain.drawWireframe();
		ofSetColor(ofColor::slateGray);
		if (bLanderLoaded) {

			//  Note this is how to rotate LEM model about the "Y" or "UP" axis
			//
//			lander.setRotation(0, angle, 0, 1, 0);    
			lander.drawWireframe();
		}
	}
	else {
		ofEnableLighting();              // shaded mode
		terrain.drawFaces();
		if (bLanderLoaded) {
//			lander.setRotation(0, angle, 0, 1, 0);
			lander.drawFaces();

		}
	}


	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
	}

	
	ofPopMatrix();

	//draw axis
	if (debugEnable) {
		drawAxis(ofVec3f(0, 0, 0));
		drawAxisLander();
		drawAxisHeader();
	}
	

	//draw exhaust

	yThruster.draw();
	fwdThruster.draw();
	bckThruster.draw();
	leftThruster.draw();
	rightThruster.draw();


	theCam->end();

	// draw screen data
	//
	string str;
	str += "Frame Rate: " + std::to_string(ofGetFrameRate());
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str, ofGetWindowWidth() - 170, 15);

	string str2;
	str2 += "Altitide (AGL): " + std::to_string(lander.getPosition().y);
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str2, 5, 15);

	
}


// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);
	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}

void ofApp::drawAxisLander() {
	ofPushMatrix();
	ofTranslate(landerParticle.position);
	ofRotate(yRotationAngle, 0, 1, 0);
	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));


	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, -1));

	ofPopMatrix();
}

void ofApp::drawAxisHeader() {
	ofPushMatrix();
	ofTranslate(landerParticle.position+heading);
	ofRotate(yRotationAngle, 0, 1, 0);
	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));


	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, -1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case '`':
		debugEnable = !debugEnable;
		break;
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'P':
	case 'p':
		break;
	case 'r':
		cam.reset();
		break;
	case 'g':
		savePicture();
		break;
	case 't':
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'm':
		toggleWireframeMode();
		break;
	case 'd':     // rotate spacecraft clockwise (about Y (UP) axis)
		clockwiseRot = true;
		break;
	case 'a':     // rotate spacecraft counter-clockwise (about Y (UP) axis)
		counterClockwiseRot = true;
		break;
	case 'w':     // spacecraft thrust UP
		wKeyPressed = true;
		break;
	case 's':     // spacefraft thrust DOWN
		sKeyPressed = true;
		break;
	case OF_KEY_F1:
		theCam = &cam;
		break;
	case OF_KEY_F3:
		theCam = &topCam;
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_UP:    // move forward
		upPressed = true;
		//landerParticle.acceleration.set(landerParticle.acceleration.x, landerParticle.acceleration.y, -5);
		break;
	case OF_KEY_DOWN:   // move backward
		downPressed = true;
		//landerParticle.acceleration.set(landerParticle.acceleration.x, landerParticle.acceleration.y, 5);
		break;
	case OF_KEY_LEFT:   // move left
		leftPressed = true;
		//landerParticle.acceleration.set(-5, landerParticle.acceleration.y, landerParticle.acceleration.z);
		break;
	case OF_KEY_RIGHT:   // move right
		rightPressed = true;
		
		//landerParticle.acceleration.set(5, landerParticle.acceleration.y, landerParticle.acceleration.z);
		break;
	default:
		break;
	}
}


void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}


void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	case OF_KEY_UP:
		upPressed = false;
		heading.z = 0;
		break;
	case OF_KEY_DOWN:
		downPressed = false;
		heading.z = 0;
		break;
	case OF_KEY_LEFT:
		leftPressed = false;
		heading.x = 0;
		break;
	case OF_KEY_RIGHT:
		rightPressed = false;
		heading.x = 0;
		break;
	case 'd':
		clockwiseRot = false;
		break;
	case 'a':
		counterClockwiseRot = false;
		break;
	case 'w':
		wKeyPressed = false;
		break;
	case 's':
		sKeyPressed = false;
		break;
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	default:
		break;

	}

}


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {


}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {


}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}





//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ .7f, .7f, .7f, 1.0f };

	static float position[] =
	{20.0, 20.0, 20.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
//	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
//	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
