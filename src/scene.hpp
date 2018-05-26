#ifndef SCENE_HPP
#define SCENE_HPP

#include "math.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "bvh.hpp"

#include <vector>

class Camera {
public:
	Mat4 c2w;
	const float yfov, aspect, focus;

	Camera(	const Vec3f& pos, const Vec3f& look, const Vec3f& up,
			const float yfov = 2,
			const float aspect = 16.0f / 9.0f,
			const float focus = 1);

	const Ray generateRay(const Vec2f&) const;
	const Vec2f sample_camera(const int i, const int j, const int res, const RndGen&) const;

private:
	const float h, w;
};

class Scene {
public:
	const std::vector<Vec3f> vtxs;		// Vertices list
	const std::vector<Vec3f> norms;		// Normals list

	const std::vector<Vec3i> vtris;		// Triangle->"vertex indexes" mapping
	const std::vector<Vec3i> ntris;		// Triangle->"normal indexes" mapping

	const std::vector<Mesh> mshs;
	const std::vector<Material> mats;

	const Camera cam;

	const BVHTree bvh;

	const Mesh* intersect(const Ray&, int&, Vec3f&) const;

	Scene(	const std::vector<Vec3f>& vtxs,
			const std::vector<Vec3f>& norms, 
			const std::vector<Vec3i>& vtris, 
			const std::vector<Vec3i>& ntris,
			const std::vector<Mesh>& meshes,
			const std::vector<Material>& materials,
			const Camera& camera) :
			vtxs(vtxs),
			norms(norms),
			vtris(vtris),
			ntris(ntris),
			mshs(meshes),
			mats(materials),
			cam(camera),
			bvh{vtxs, vtris} {}

};

#endif
