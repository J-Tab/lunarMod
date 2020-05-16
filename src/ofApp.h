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
		Box meshBounds(const ofMesh &);
		//Box multiMeshBounds(ofxAssimpModelLoader model);
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
		void drawBox(const Box &box);
		Vector3 closestPt(Vector3 pt,TreeNode source);
		Vector3 closestPt(Vector3 pt, vector<int> source);
		bool ptCollide(TreeNode &surfaceNode, Box &landerBX, Vector3 &ptRtn);
		void randomLandingZone();
		void randomLandingZoneFind(const TreeNode & node, TreeNode & nodeRtn);
		Vector3 closePt;
		KdTree treeAl;
		float AGL;

		string lastScore;
		

		//variable for the rotation of the lander around the y-axis
		float yRotationAngle = 0;
		float headingAcceleration = 3.0;

		//heading vector for lander
		ofVec3f heading = ofVec3f(0,0,0);
		ofVec3f headingRot = heading;
		int timer = 0;
		
		ofEasyCam cam;
		ofxAssimpModelLoader lander, terrain;
		
		//physics
		Particle landerParticle = Particle();
		Box boundingBox;
		Box landerBounds;
		Vector3 landedPoint;
		bool lunarLanded;
		TreeNode rayBox;
		TreeNode bottomKdBox1;
		TreeNode bottomKdBox2;


		ofLight light;
		ofImage backgroundImage;
		ofCamera *theCam = NULL;
		ofCamera topCam;
		ofCamera trackingCam;
		ofEasyCam onboardCam;
		ofEasyCam easyCam;
		Particle easyCamParticle = Particle();

		//buttons hits
		bool upPressed = false;
		bool downPressed = false;
		bool leftPressed = false;
		bool rightPressed = false;
		bool wKeyPressed = false;
		bool sKeyPressed = false;

		bool numLeft = false;
		bool numRight = false;
		bool numUp = false;
		bool numDown = false;
		bool numPGup = false;
		bool numPGdown = false;
		bool easyFunc = false;

		bool easyTarget = false;

		Box LandingBox;
		Vector3 landingPoint;


		glm::vec3 currentMouse;
		glm::vec3 lastMouse;
		bool bDrag = false;

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
