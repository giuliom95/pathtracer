#include "bvh.hpp"

BBox::BBox(const Vec3f& p0, const Vec3f& p1) : pMin(p0), pMax(p0) {
	enlarge(p1);
}

BBox::BBox(const Mesh& msh, const Scene& scn) : pMin(), pMax() {
	const auto tri = scn.vtris[msh.t0];
	pMin = pMax = scn.vtxs[tri[0]];
	for(auto t = msh.t0; t < msh.t0 + msh.ntris; ++t) {
		const auto tri = scn.vtris[t];
		enlarge(scn.vtxs[tri[0]]);
		enlarge(scn.vtxs[tri[1]]);
		enlarge(scn.vtxs[tri[2]]);
	}
};


void BBox::enlarge(const Vec3f& p) {
	pMin[0] = pMin[0] < p[0] ? pMin[0] : p[0];
	pMin[1] = pMin[1] < p[1] ? pMin[1] : p[1];
	pMin[2] = pMin[2] < p[2] ? pMin[2] : p[2];

	pMax[0] = pMax[0] > p[0] ? pMax[0] : p[0];
	pMax[1] = pMax[1] > p[1] ? pMax[1] : p[1];
	pMax[2] = pMax[2] > p[2] ? pMax[2] : p[2];
}



BVHTree::BVHTree(const Scene& scn) : nodes() {
	// Build bounding boxes


}
