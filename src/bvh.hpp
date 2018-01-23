#ifndef BVH_HPP
#define BVH_HPP

#include <vector>

#include "math.hpp"
#include "mesh.hpp"

class BBox {
public:
	Vec3f pMin;
	Vec3f pMax;

	BBox(const Vec3f&, const Vec3f&);
	BBox(	const Mesh&,
			const std::vector<Vec3f>&,
			const std::vector<Vec3i>&);

	void enlarge(const Vec3f& p);
	void enlarge(const BBox& box);

	inline const bool intersect (const Ray& r) const {
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
	const int mesh;	//This contains the index of the mesh inside the mesh vector of the scene
};

class BVHTree {
	std::vector<BVHNode> nodes;
public:
	BVHNode* root;
	BVHTree(const std::vector<Mesh>& meshes,
			const std::vector<Vec3f>& vtxs,
			const std::vector<Vec3i>& vtris);
};


#endif
