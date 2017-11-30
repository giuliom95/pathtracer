#ifndef SCENE_HPP
#define SCENE_HPP

#include "math.hpp"

#include <vector>

class Mesh {
public:
	const int p0;
	const int ntris;

	Mesh(int p0, int ntris) : p0(p0), ntris(ntris) {}
};

class Camera {
public:
	Mat4 c2w;
	const float yfov, aspect, focus;

	Camera(	const Vec3f& pos, const Vec3f& look, const Vec3f& up, 
			const float yfov = 2,
			const float aspect = 16.0f / 9.0f,
			const float focus = 1);

	const Ray generateRay(const Vec2f&) const;
};

class Scene {
public:
	const std::vector<Vec3f> vtxs;
	const std::vector<Vec3f> norms;

	const std::vector<Vec3i> vtris;
	const std::vector<Vec3i> ntris;

	const std::vector<Mesh> meshes;
	const Camera cam;

	Scene(	const std::vector<Vec3f>& vtxs,
			const std::vector<Vec3f>& norms,
			const std::vector<Vec3i>& vtris,
			const std::vector<Vec3i>& ntris,
			const std::vector<Mesh>& meshes,
			const Camera& cam)
			:	vtxs(vtxs), norms(norms),
				vtris(vtris), ntris(ntris),
				meshes(meshes), cam(cam) {}
};

#endif
