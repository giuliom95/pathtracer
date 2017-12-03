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
	void enlarge(const BBox& box);

	bool intersect(const Ray& r) {
		auto t0 = (pMin - r.o) * r.invd;
		auto t1 = (pMax - r.o) * r.invd;
		if (r.invd[0] < 0) std::swap(t0[0], t1[0]);
		if (r.invd[1] < 0) std::swap(t0[1], t1[1]);
		if (r.invd[2] < 0) std::swap(t0[2], t1[2]);
		auto tmin = max(t0[2], max(t0[1], max(t0[0], r.tmin)));
		auto tmax = min(t1[2], min(t1[1], min(t1[0], r.tmax)));
		tmax *= 1.00000024f; // numerical precision fix
		return tmin <= tmax;
	}
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
