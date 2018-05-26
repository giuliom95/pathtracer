#include "bvh.hpp"

#include <algorithm>

BBox::BBox(const Vec3f& p0, const Vec3f& p1) : pMin(p0), pMax(p0) {
	enlarge(p1);
}

BBox::BBox(	const Mesh& msh,
			const std::vector<Vec3f>& vtxs,
			const std::vector<Vec3i>& vtris)
			: pMin(), pMax() {

	const auto tri = vtris[msh.t0];
	pMin = pMax = vtxs[tri[0]];
	for(auto t = msh.t0; t < msh.t0 + msh.ntris; ++t) {
		const auto tri = vtris[t];
		enlarge(vtxs[tri[0]]);
		enlarge(vtxs[tri[1]]);
		enlarge(vtxs[tri[2]]);
	}
};

const void BBox::enlarge(const Vec3f& p) {
	pMin[0] = min(pMin[0], p[0]);
	pMin[1] = min(pMin[1], p[1]);
	pMin[2] = min(pMin[2], p[2]);

	pMax[0] = max(pMax[0], p[0]);
	pMax[1] = max(pMax[1], p[1]);
	pMax[2] = max(pMax[2], p[2]);
}


const void BBox::enlarge(const BBox& box) {
	enlarge(box.pMin);
	enlarge(box.pMax);
}


BVHNode* build_tree(std::vector<BVHNode>& nodes,
					std::vector<int>& elems,
					const std::vector<BBox>& boxes,
					const std::vector<Vec3f>& centroids,
					const unsigned mintris, const unsigned maxrec) {

	if(elems.size() <= mintris || maxrec == 0) {
		BBox b = boxes[elems[0]];
		for(unsigned i = 1; i < elems.size(); ++i) {
			b.enlarge(boxes[elems[i]]);
		}
		BVHNode node{nullptr, nullptr, b, elems};
		nodes.push_back(node);
		return &nodes[nodes.size()-1];
	} else {
		// Compute the span of the centroids for each dimension
		auto minCentroid = centroids[elems[0]];
		auto maxCentroid = centroids[elems[0]];
		for(unsigned e = 1; e < elems.size(); ++e) {
			auto c = centroids[e];
			minCentroid[0] = min(minCentroid[0], c[0]);
			minCentroid[1] = min(minCentroid[1], c[1]);
			minCentroid[2] = min(minCentroid[2], c[2]);

			maxCentroid[0] = max(maxCentroid[0], c[0]);
			maxCentroid[1] = max(maxCentroid[1], c[1]);
			maxCentroid[2] = max(maxCentroid[2], c[2]);
		}

		// Get longer span
		auto axis = 0;
		const auto spans = maxCentroid - minCentroid;
		if(spans[2] > spans[1] && spans[1] > spans[0]) axis = 2;
		if(spans[1] > spans[2] && spans[2] > spans[0]) axis = 1;


		const auto size = elems.size();
		const auto mid = size/2;

		// Perform a pseudo-sorting the centroids
		std::nth_element(
			&elems[0], &elems[mid], &elems[size],
			[axis, &centroids](const int a, const int b){
				return centroids[a][axis] < centroids[b][axis];
			});

		// Build the two groups
		std::vector<int> group_l(mid);
		std::vector<int> group_r(size - mid);
		for(unsigned i = 0; i < mid; ++i)
			group_l[i] = elems[i];
		for(unsigned i = 0; i < (size - mid); ++i)
			group_r[i] = elems[i + mid];

		// Recursion
		auto ptr_l = build_tree(nodes, group_l, boxes, centroids, mintris, maxrec - 1);
		auto ptr_r = build_tree(nodes, group_r, boxes, centroids, mintris, maxrec - 1);

		// Build bounding box
		
		BBox b = boxes[elems[0]];
		for(unsigned i = 1; i < elems.size(); ++i) {
			b.enlarge(boxes[elems[i]]);
		}

		// Create node and return
		BVHNode node{ptr_l, ptr_r, b, {}};
		nodes.push_back(node);
		return &nodes[nodes.size()-1];
	}
}


BVHTree::BVHTree(	const std::vector<Vec3f>& vtxs,
					const std::vector<Vec3i>& vtris) :
					nodes(), root(nullptr) {

	const unsigned ntris{vtris.size()};

	// Enlarge the nodes vector to fit all the nodes
	nodes.reserve(2*(ntris/4) - 1);

	std::vector<BBox> boxes{};
	std::vector<Vec3f> centroids{};

	// Compute bboxes and centroids
	for(const auto& t : vtris) {
		BBox b{vtxs[t[0]], vtxs[t[1]]};
		b.enlarge(vtxs[t[2]]);
		boxes.push_back(b);

		const auto c = 0.5 * (b.pMin + b.pMax);
		centroids.push_back(c);
	}

	std::vector<int> elems(ntris);
	for(unsigned i = 0; i < ntris; ++i) elems[i] = i;

	const auto max_recursion_steps = 10;
	const auto min_tris_per_leaf = 10;
	root = build_tree(	nodes, elems, boxes, centroids, 
						min_tris_per_leaf, max_recursion_steps);
}