
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

	trackingCam.setNearClip(.1);
	trackingCam.setFov(65.5);
	trackingCam.setPosition(2, 5, 2);
	trackingCam.lookAt(landerParticle.position);

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
	//terrain.loadModel("geo/mars-5k.obj");
	terrain.loadModel("geo/moon-houdini.obj");
	terrain.setScaleNormalization(false);
	terrain.setRotation(0, -180, 1, 0, 0);
	terrain.update();
	cout << "terrain meshes: " << terrain.getNumMeshes() << endl;
	boundingBox = meshBounds(terrain.getMesh(0));



	// load lander model
	//
	if (lander.loadModel("geo/lander.obj")) {
		
		lander.setScaleNormalization(false);
		lander.setScale(.5, .5, .5);
		lander.setRotation(0, -180, 1, 0, 0);
		lander.setPosition(0, 0, 0);
		lander.update();
		//landerBounds = Box(Vector3(lander.getSceneMin().x, lander.getSceneMin().y, lander.getSceneMin().z), Vector3(lander.getSceneMin().x+5, lander.getSceneMin().y+5, lander.getSceneMin().z+5));
		cout << "Lander Max:  " << lander.getSceneMax() << endl;
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
	//treeAl.create(terrain.getMesh(0), 10);

	//set to 17 for default
	treeAl.create(terrain.getMesh(0), 12);
}


int LANDER_SIZE = 1.9;


void ofApp::update() {
	timer++;

	//Camera
	trackingCam.lookAt(landerParticle.position);
	//Heading depending on arrow keys
	ofVec3f tempVec = ofVec3f(0, 0, 0);
	ofVec3f turbVec = ofVec3f(0, 0, 0);
	//W and S headings
	if ((wKeyPressed && sKeyPressed) || (!wKeyPressed && !sKeyPressed)) {
		tempVec.y = -.1;
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
	if (landerParticle.position.y > landedPoint.y() && !lunarLanded) {
		float v = float(rand()) / (float(RAND_MAX) + 2.0);
		int rAngle = rand() % 360;
		turbVec.x += v;
		turbVec = turbVec.rotate(rAngle, ofVec3f(0, 1, 0));
		heading = tempVec.rotate(yRotationAngle, ofVec3f(0, 1, 0)) + turbVec;
	}
	else {
		heading = tempVec.rotate(yRotationAngle, ofVec3f(0, 1, 0));
	}
		
	//Ray collision for altitude

	rayBox = treeAl.findRayNode(Ray(Vector3(landerParticle.position.x, landerParticle.position.y, landerParticle.position.z), Vector3(landerParticle.position.x, landerParticle.position.y-LANDER_SIZE*100, landerParticle.position.z)));
	bottomKdBox1 = treeAl.findRayNode(Ray(Vector3(landerParticle.position.x - LANDER_SIZE, landerParticle.position.y, landerParticle.position.z - LANDER_SIZE), Vector3(landerParticle.position.x - LANDER_SIZE, landerParticle.position.y-LANDER_SIZE, landerParticle.position.z - LANDER_SIZE)));
	bottomKdBox2 = treeAl.findRayNode(Ray(Vector3(landerParticle.position.x + LANDER_SIZE, landerParticle.position.y, landerParticle.position.z + LANDER_SIZE), Vector3(landerParticle.position.x + LANDER_SIZE, landerParticle.position.y-LANDER_SIZE, landerParticle.position.z + LANDER_SIZE)));
	closePt = closestPt(Vector3(landerParticle.position.x, landerParticle.position.y, landerParticle.position.z), rayBox);
	

	
	lander.setRotation(1, yRotationAngle, 0, 1, 0);
	landerParticle.acceleration = heading;



	landerBounds = Box(Vector3(landerParticle.position.x - LANDER_SIZE, landerParticle.position.y, landerParticle.position.z - LANDER_SIZE), Vector3(landerParticle.position.x + LANDER_SIZE, landerParticle.position.y + LANDER_SIZE, landerParticle.position.z + LANDER_SIZE));
	//Box collision 
	if (ptCollide(rayBox, landerBounds, landedPoint) || ptCollide(bottomKdBox1, landerBounds, landedPoint) || ptCollide(bottomKdBox2, landerBounds, landedPoint)) {
		lunarLanded = true;
		cout << "PT COLLIDE" << endl;
		AGL = 0;
	}
	else {
		lunarLanded = false;
		AGL = Vector3(landerParticle.position.x, landerParticle.position.y, landerParticle.position.z).y() - closePt.y();
	}

	if (lunarLanded && !wKeyPressed) {
		landerParticle.acceleration = ofVec3f(landerParticle.acceleration.x, -landerParticle.acceleration.y, landerParticle.acceleration.z) / 2;
		landerParticle.velocity = ofVec3f(landerParticle.velocity.x, -landerParticle.velocity.y, landerParticle.velocity.z) / 2;



		/*landerParticle.acceleration = ofVec3f(0, 0, 0);
		landerParticle.velocity = ofVec3f(0, 0, 0);*/
	}
	lander.setPosition(landerParticle.position.x, landerParticle.position.y, landerParticle.position.z);

	//Update physics movement for the lander
	//Also collision detection
	landerParticle.integrate();
	

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

	ofSetColor(ofColor::blue);
	ofFill();
	ofDrawSphere(ofVec3f(landerParticle.position.x, landerParticle.position.y - LANDER_SIZE, landerParticle.position.z),1);

	ofNoFill();
	ofSetColor(ofColor::white);
	drawBox(landerBounds);
	drawBox(rayBox.box);
	ofSetColor(ofColor::lightGray);
	//treeAl.drawLeafNodes(treeAl.root);

	//draw exhaust

	ofDisableLighting();
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
	str2 += "Altitide (AGL): " + std::to_string(AGL);
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
	case OF_KEY_F2:
		theCam = &trackingCam;
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
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
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


Box ofApp::meshBounds(const ofMesh & mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

//Box ofApp::multiMeshBounds(ofxAssimpModelLoader model) {
//	int n = model.getMesh(0).getNumVertices();
//	ofVec3f v = model.getMesh(0).getVertex(0);
//	ofVec3f max = v;
//	ofVec3f min = v;
//	for (int i = 1; i < n; i++) {
//		ofVec3f v = model.getMesh(0).getVertex(i);
//
//		if (v.x > max.x) max.x = v.x;
//		else if (v.x < min.x) min.x = v.x;
//
//		if (v.y > max.y) max.y = v.y;
//		else if (v.y < min.y) min.y = v.y;
//
//		if (v.z > max.z) max.z = v.z;
//		else if (v.z < min.z) min.z = v.z;
//	}
//	
//	for (int z = 1; z < model.getNumMeshes(); z++) {
//		ofMesh currentMesh = model.getMesh(z);
//		for (int i = 0; i < currentMesh.getNumVertices(); i++) {
//			ofVec3f v = currentMesh.getVertex(i);
//
//			if (v.x > max.x) max.x = v.x;
//			else if (v.x < min.x) min.x = v.x;
//
//			if (v.y > max.y) max.y = v.y;
//			else if (v.y < min.y) min.y = v.y;
//
//			if (v.z > max.z) max.z = v.z;
//			else if (v.z < min.z) min.z = v.z;
//		}
//	}
//
//	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
//
//}




void ofApp::drawBox(const Box &box) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();
	ofDrawBox(p, w, h, d);
}


//Check from the treeNode pts
Vector3 ofApp::closestPt(Vector3 pt,TreeNode source)
{
	int closeID = source.points[0];
	Vector3 rVec = Vector3(treeAl.mesh.getVertex(closeID).x, treeAl.mesh.getVertex(closeID).y, treeAl.mesh.getVertex(closeID).z);
	float closestDist = pt.distance(rVec);
	for (int i = 1; i < source.points.size(); i++) {
		int check = source.points[i];
		Vector3 checkVec = Vector3(treeAl.mesh.getVertex(check).x, treeAl.mesh.getVertex(check).y, treeAl.mesh.getVertex(check).z);
		float checkDist = pt.distance(checkVec);
		if (closestDist > checkDist) {
			closeID = i;
			rVec = checkVec;
			closestDist = checkDist;
		}
	}
	return rVec;
}

//Check closest points from an already provided source
Vector3 ofApp::closestPt(Vector3 pt, vector<int> source)
{
	int closeID = source[0];
	Vector3 rVec = Vector3(treeAl.mesh.getVertex(closeID).x, treeAl.mesh.getVertex(closeID).y, treeAl.mesh.getVertex(closeID).z);
	float closestDist = pt.distance(rVec);
	for (int i = 1; i < source.size(); i++) {
		int check = source[i];
		Vector3 checkVec = Vector3(treeAl.mesh.getVertex(check).x, treeAl.mesh.getVertex(check).y, treeAl.mesh.getVertex(check).z);
		float checkDist = pt.distance(checkVec);
		if (closestDist > checkDist) {
			closeID = i;
			rVec = checkVec;
			closestDist = checkDist;
		}
	}
	return rVec;
}


//Check for points of collission on the surface
bool ofApp::ptCollide(TreeNode &surfaceNode, Box &landerBX, Vector3 & ptRtn) {
	vector<int> insidePts;
	for (int i = 0; i < surfaceNode.points.size(); i++) {
		Vector3 curPt = Vector3(-treeAl.mesh.getVertex(surfaceNode.points[i]).x, treeAl.mesh.getVertex(surfaceNode.points[i]).y, -treeAl.mesh.getVertex(surfaceNode.points[i]).z);
		if (landerBX.inside(curPt)) {
			insidePts.push_back(i);
		}
	}
	//Check if there any pts that collided
	if (insidePts.size() > 0) {
		ptRtn = closestPt(landerBX.center(), insidePts);
		return true;
	}
	else {
		return false;
	}

}