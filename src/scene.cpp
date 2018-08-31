#include "scene.hpp"

#include <vector>
#include "bvh.hpp"

Camera::Camera(	const Vec3f& pos,
				const Vec3f& look, 
				const Vec3f& up, 
				const float yfov,
				const float aspect,
				const float focus)
				: yfov(yfov), aspect(aspect), focus(focus),
				  h{2 * std::tan(yfov / 2)}, w{h * aspect} {

	const auto vz = normalize(look);
	const auto vy = normalize(up);
	const auto vx = cross(vz, vy);

	c2w = {vx, vy, -1*vz, pos};
}


const Vec2f Camera::sample_camera(const int i, const int j, const int res, const RndGen& rg) const {
	const auto sh = res;
	const auto sw = res * aspect;
	const auto u = (i + rg.next_float()) / sw;
	const auto v = (j + rg.next_float()) / sh;
	return {u, v};
}

const Ray Camera::generateRay(const Vec2f& uv) const {
	const Vec3f q{w * focus * (uv[0] - 0.5f), -h * focus * (uv[1] - 0.5f), -focus};
	return {transformPoint(c2w, {0,0,0}), normalize(transformVector(c2w, q))};
}


const Mesh* Scene::intersect(const Ray& r, int& triangle, Vec3f& tuv) const {

	triangle = -1;	// It's only for precaution

	auto intersection = false;

	std::vector<const BVHNode*> stack;
	stack.reserve(2*(vtris.size()/4)-1);
	stack.push_back(bvh.root);

	auto t = r.tmax;

	while (!stack.empty()) {

		auto node = stack.back();
		stack.pop_back();
		if (!node->box.intersect(r)) continue;

		const auto tris = node->tris;
		const auto numtris = tris.size();
		if (numtris > 0) {
			for(const auto ti : tris) {
				const auto vtri = vtris[ti];
				const auto v0 = vtxs[vtri[0]];
				const auto v1 = vtxs[vtri[1]];
				const auto v2 = vtxs[vtri[2]];

				const auto ints = intersectTriangle(r, v0, v1, v2);

				if(	ints[0] >= r.tmin &&
					ints[0] <= r.tmax &&
					ints[0] < t) {

					triangle = ti;
					t = ints[0];
					tuv[0] = ints[0];
					tuv[1] = ints[1];
					tuv[2] = ints[2];
					intersection = true;
				}
			}

		} else {
			stack.push_back(node->left);
			stack.push_back(node->right);
		}
	}

	if(intersection) {
		for(auto& m : mshs) {
			const auto delta = triangle - m.t0;
			if(0 <= delta && delta < m.ntris)
				return &m;
		}
	}
	return nullptr;
}


const Vec3f Scene::sample_envmap(const Vec3f& vec) const {
	if(envmap_w == 0) return {};
	const auto p = cart2polar(vec);
	const auto u = std::abs(p[0] * INV_PI * 0.5);
	const auto v = std::abs(((-p[1] * INV_PI) + 0.5));
	const unsigned i = envmap_w*u;
	const unsigned j = envmap_h*v;
	const auto c = envmap[i+j*envmap_w];
	return {c[0], c[1], c[2]};
}