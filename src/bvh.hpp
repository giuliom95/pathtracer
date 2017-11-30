#ifndef BVH_H
#define BVH_H

#include <vector>

#include "math.hpp"

class BVHTree {
	std::vector<BVHNode> nodes;
public:
	BVHTree(const Scene&);
};

class BVHNode {
public:
	BVHNode* right;
	BVHNode* left;
	BBox box;
	Mesh* mesh;
};

#endif
