//  KdTree Template - Simple KdTree class
//
//  SJSU  - CS134 Game Development
//
//  Kevin M. Smith   04/19/20

//  **Important:  Vertices (x, y, z) in the mesh are stored in the Tree node as an integer index.
//  to read the 3D vertex point from the mesh given index i,  use the function ofMesh::getVertex(i);  See
//  KdTree::meshBounds() for an example of usage;
//
//

#include "KdTree.h"
 
// draw KdTree (recursively)
//
void KdTree::draw(TreeNode & node, int numLevels, int level) {
	if (level >= numLevels) return;
	drawBox(node.box);
	level++;
	for (int i = 0; i < node.children.size(); i++) {
		draw(node.children[i], numLevels, level);
	}
}

// draw only leaf Nodes
//
void KdTree::drawLeafNodes(TreeNode & node) {

	if (node.children.size() == 0) {
		drawBox(node.box);
	}
	else {
		for (int i = 0; i < node.children.size(); i++) {
			drawLeafNodes(node.children[i]);
		}
	}
}


//draw a box from a "Box" class  
//
void KdTree::drawBox(const Box &box) {
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

// return a Mesh Bounding Box for the entire Mesh
//
Box KdTree::meshBounds(const ofMesh & mesh) {
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
	//cout << "vertices: " << n << endl;
//	cout << "min: " << min << "max: " << max << endl;
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

// getMeshPointsInBox:  return an array of indices to points in mesh that are contained 
//                      inside the Box.  Return count of points found;
//
int KdTree::getMeshPointsInBox(const ofMesh & mesh, const vector<int>& points,
	Box & box, vector<int> & pointsRtn)
{
	int counter = 0;
	for (int i = 0; i < points.size(); i++) {
		int currentID = points[i];
		//Negative becuase it doesn't work otherwise, not sure
		Vector3 current = Vector3(-mesh.getVertex(currentID).x, mesh.getVertex(currentID).y, -mesh.getVertex(currentID).z);

		if (box.inside(current)) {
			//cout << "Vertex in: " << current.x() << ":"<< current.y() << ":"<< current.z() << ":" << endl;
			//cout << "Box: " << box.min().x() <<","<< box.min().y()<<","<< box.min().z() << ":" << box.max().x()<<","<< box.max().y()<<","<< box.max().z() << endl<<endl;
			counter++;
			
			pointsRtn.push_back(points.at(i));
		}
	}
	//cout << "vertices: " << counter << endl;
	return counter;
}



//  Subdivide a Box; return children in  boxList
//
void KdTree::subDivideBox(const Box &box, vector<Box> & boxList) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;

	//Divide X-Way
	if (size.x() >= size.y() && size.x() >= size.z()) {
		Vector3 max1 = Vector3(max.x() - size.x() / 2, max.y(), max.z());

		Box firstBox = Box(min, max1);

		Vector3 min2 = Vector3(max.x() - size.x() / 2, min.y(), min.z());
		Box secondBox = Box(min2, max);

		boxList.push_back(firstBox);
		boxList.push_back(secondBox);
	}
	//Divide Y-Way
	else if (size.y() >= size.x() && size.y() >= size.z()) {
		Vector3 max1 = Vector3(max.x(), max.y() - size.y() / 2, max.z());
		Box firstBox = Box(min, max1);

		Vector3 min2 = Vector3(min.x(), max.y() - size.y() / 2, min.z());
		Box secondBox = Box(min2, max);

		boxList.push_back(firstBox);
		boxList.push_back(secondBox);
	}
	//Divide Z-Way
	else {

		Vector3 max1 = Vector3(max.x(), max.y(), max.z() - size.z() / 2);
		Box firstBox = Box(min, max1);

		Vector3 min2 = Vector3(min.x(), min.y(), max.z() - size.z() / 2);
		Box secondBox = Box(min2, max);

		boxList.push_back(firstBox);
		boxList.push_back(secondBox);
	}


}

void KdTree::create(const ofMesh & geo, int numLevels) {
	// initialize KdTree structure
	//
	mesh = geo;
	int level = 0;

	root.box = meshBounds(mesh);
	for (int i = 0; i < mesh.getNumVertices(); i++) {
		root.points.push_back(i);
	}
	level++;

	subdivide(mesh, root, numLevels, level);


}

void KdTree::subdivide(const ofMesh & mesh, TreeNode & node, int numLevels, int level) {

	if (numLevels < level ) {
		return;
	}
	else {
		int newLevel = level+1;
		vector<Box> x;
		subDivideBox(node.box, x);
		int meshPts = 0;
		
		//1st child
		TreeNode first;
		first.box = x.at(0);
		meshPts = getMeshPointsInBox(mesh, node.points, first.box, first.points);
		if (meshPts > 0) {
			
			subdivide(mesh, first, numLevels, newLevel);
			node.children.push_back(first);
		}
		
		



		//2nd Child
		TreeNode second;
		second.box = x.at(1);
		meshPts = getMeshPointsInBox(mesh, node.points, second.box, second.points);
		if (meshPts > 0) {
			subdivide(mesh, second, numLevels, newLevel);
			node.children.push_back(second);
		}
		
		
	}

}

TreeNode KdTree::findRayNode(const Ray &ray) {
	TreeNode finalLeaf;
	if (root.children[0].box.intersect(ray, 0, 1000)) {
		intersect(ray, root.children[0], finalLeaf);
	}
	else {
		intersect(ray, root.children[1], finalLeaf);
	}
	return finalLeaf;
}

void KdTree::intersect(const Ray &ray, const TreeNode & node, TreeNode & nodeRtn) {
	//or statement due to possibility of only having 1 child
	if (node.children[0].box.intersect(ray, 0, 1000) ||node.children.size() == 1) {
		if (node.children[0].children.size() != 0) {
			intersect(ray, node.children[0], nodeRtn);
		}
		else {
			nodeRtn = node.children[0];
		}
	}
	else {
		if (node.children[1].children.size() != 0) {
			intersect(ray, node.children[1], nodeRtn);
		}
		else {
			nodeRtn = node.children[1];
		}
	}
}



