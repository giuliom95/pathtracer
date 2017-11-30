#include "scene.hpp"

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
