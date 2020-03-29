#pragma once

#include "ofMain.h"
#include  "ofxAssimpModelLoader.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"




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
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();

		//variable for the rotation of the lander around the y-axis
		float yRotationAngle = 0;
		float headingAcceleration = 3.0;

		//heading vector for lander
		ofVec3f heading = ofVec3f(0,0,0);
		ofVec3f headingRot = heading;
		
		ofEasyCam cam;
		ofxAssimpModelLoader lander;
		Particle landerParticle = Particle();

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

		
		
		//rotation variables
		bool clockwiseRot = false;
		bool counterClockwiseRot = false;


		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
	
		bool bBackgroundLoaded = false;
		bool bLanderLoaded = false;
};
