#ifndef BVH_H
#define BVH_H

#include <vector>

#include "math.hpp"
#include "scene.hpp"

class BBox {
public:
	Vec3f pMin;
	Vec3f pMax;

	BBox(const Vec3f&, const Vec3f&);
	BBox(const Mesh&, const Scene&);

	void enlarge(const Vec3f& p);
};

class BVHNode {
public:
	const BVHNode* left;
	const BVHNode* right;
	const BBox box;
	const Mesh* mesh;
};

class BVHTree {
	std::vector<BVHNode> nodes;
public:
	BVHNode* root;
	BVHTree(const Scene&);
};


#endif
