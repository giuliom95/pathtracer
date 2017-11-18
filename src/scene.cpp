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
	const auto vx = cross(vy, vz);

	c2w = {vx, vy, vz, pos};
}