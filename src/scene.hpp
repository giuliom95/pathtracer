#ifndef SCENE_H
#define SCENE_H

#include "math.hpp"

#include <vector>
#include <fstream>
#include <iostream>

class Mesh {
public:
	const int vtx0;
	const int ntris;

	Mesh(int vtx0, int ntris) : vtx0(vtx0), ntris(ntris) {}
};

class Camera {
public:
	Mat4 c2w;
	const float yfov, aspect, focus;

	Camera(	const Vec3f& pos, const Vec3f& look, const Vec3f& up, 
			const float yfov = 2,
			const float aspect = 16.0f / 9.0f,
			const float focus = 1);
};

class Scene {
public:
	const std::vector<Vec3f> vtxs;
	const std::vector<Vec3i> tris;

	const std::vector<Mesh> meshes;
	const Camera cam;

	Scene(	const std::vector<Vec3f>& vtxs,
			const std::vector<Vec3i>& tris,
			const std::vector<Mesh>& meshes,
			const Camera& cam)
			: vtxs(vtxs), tris(tris), meshes(meshes), cam(cam) {}

	static Scene loadOBJ(std::istream&);
};

#endif