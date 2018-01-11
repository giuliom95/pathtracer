#include "scene.hpp"

#include <vector>
#include "bvh.hpp"

Camera::Camera(	const Vec3f& pos,
				const Vec3f& look, 
				const Vec3f& up, 
				const float yfov,
				const float aspect,
				const float focus)
				: yfov(yfov), aspect(aspect), focus(focus) {

	const auto vz = normalize(look);
	const auto vy = normalize(up);
	const auto vx = cross(vz, vy);

	c2w = {vx, vy, -1*vz, pos};
}

const Ray Camera::generateRay(const Vec2f& uv) const {

	auto h = 2 * std::tan(yfov / 2);
	auto w = h * aspect;
	Vec3f q{w * focus * (uv[0] - 0.5f), h * focus * (uv[1] - 0.5f), -focus};

	return {transformPoint(c2w, {0,0,0}), transformVector(c2w, normalize(q))};
}


const bool Scene::intersect(const Ray& r, const Mesh* mesh, int& triangle, Vec3f& tuv) const {

	std::vector<const BVHNode*> stack;
	stack.reserve(meshes.size()*2-1);
	stack.push_back(bvh.root);

	auto t = r.tmax;

	//printf("RAY\n");
	while (!stack.empty()) {
		
		//for(auto& f : stack) {
		//	printf("{%f, %f, %f}{%f, %f, %f} ", f->box.pMin[0], f->box.pMin[1], f->box.pMin[2], f->box.pMax[0], f->box.pMax[1], f->box.pMax[2]);
		//}
		//printf("\n");

		auto node = stack.back();
		stack.pop_back();
		if (!node->box.intersect(r)) continue;

		if (node->mesh != nullptr) {
			auto* m = node->mesh;
			for(auto ti = 0; ti < m->ntris; ++ti) {
				const auto vtri = vtris[m->t0 + ti];
				const auto v0 = vtxs[vtri[0]];
				const auto v1 = vtxs[vtri[1]];
				const auto v2 = vtxs[vtri[2]];

				const auto tuv = intersectTriangle(r, v0, v1, v2);

				if(	tuv[0] >= r.tmin &&
					tuv[0] <= r.tmax &&
					tuv[0] < t) {

					triangle = ti;
					t = tuv[0];
					mesh = m;
				}
				// TODO: MAY NOT HIT MESH
			}

			if (t < r.tmax)
				return true;

		} else {
			stack.push_back(node->left);
			stack.push_back(node->right);
		}
	}

	return false;
}
