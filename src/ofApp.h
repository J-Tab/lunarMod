#pragma once

#include "ofMain.h"
#include  "ofxAssimpModelLoader.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "ofxGui.h"
#include "box.h"
#include "ray.h"
#include "KdTree.h"




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
		void drawAxis(ofVec3f);
		void drawAxisLander();
		void drawAxisHeader();
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		Box meshBounds(const ofMesh &);
		KdTree treeAl;

		//variable for the rotation of the lander around the y-axis
		float yRotationAngle = 0;
		float headingAcceleration = 3.0;

		//heading vector for lander
		ofVec3f heading = ofVec3f(0,0,0);
		ofVec3f headingRot = heading;
		int timer = 0;
		
		ofEasyCam cam;
		ofxAssimpModelLoader lander, terrain;
		Particle landerParticle = Particle();
		Box boundingBox;
		Box landerBounds;


		ofLight light;
		ofImage backgroundImage;
		ofCamera *theCam = NULL;
		ofCamera topCam;

		//buttons hits
		bool upPressed = false;
		bool downPressed = false;
		bool leftPressed = false;
		bool rightPressed = false;
		bool wKeyPressed = false;
		bool sKeyPressed = false;


		//emitters attached to the lander
		ParticleEmitter yThruster = ParticleEmitter();
		ParticleEmitter fwdThruster = ParticleEmitter();
		ParticleEmitter bckThruster = ParticleEmitter();
		ParticleEmitter leftThruster = ParticleEmitter();
		ParticleEmitter rightThruster = ParticleEmitter();
		
		//rotation variables
		bool clockwiseRot = false;
		bool counterClockwiseRot = false;
		float rotationAccel = 0;
		float rotationVel = 0;

		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
	
		bool bBackgroundLoaded = false;
		bool bLanderLoaded = false;

		ofxPanel gui;
		ofxToggle axisButton;
		bool debugEnable = false;
};
