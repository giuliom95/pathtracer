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
	const std::vector<Vec3f> 	vtxs;		// Vertices list
	const std::vector<Vec3f> 	norms;		// Normals list

	const std::vector<Vec3i> 	vtris;		// Triangle->"vertex indexes" mapping
	const std::vector<Vec3i> 	ntris;		// Triangle->"normal indexes" mapping

	const std::vector<Mesh>		mshs;
	const std::vector<Material>	mats;

	const std::vector<int>		lgt_tris;		// Indexes of the light emitting triangles
	const std::vector<float>	lgt_tris_areas;	// Areas of the emitting triangles

	const std::vector<Vec4h>	envmap;
	const unsigned				envmap_w;
	const unsigned				envmap_h;

	const Camera cam;

	const BVHTree bvh;

	const float light_pdf_area_coeff;

	const Mesh* intersect(const Ray&, int&, Vec3f&) const;

	const Vec3f sample_envmap(const Vec3f& v) const;

	Scene(	const std::vector<Vec3f>&		vtxs,
			const std::vector<Vec3f>&		norms, 
			const std::vector<Vec3i>&		vtris, 
			const std::vector<Vec3i>&		ntris,
			const std::vector<Mesh>&		meshes,
			const std::vector<Material>&	materials,
			const std::vector<int>& 		light_tris,
			const std::vector<float>& 		light_tris_areas,
			const std::vector<Vec4h>& 		envmap,
			const unsigned 					envmap_w,
			const unsigned 					envmap_h,
			const Camera& 					camera) :
			vtxs(vtxs),
			norms(norms),
			vtris(vtris),
			ntris(ntris),
			mshs(meshes),
			mats(materials),
			lgt_tris(light_tris),
			lgt_tris_areas(light_tris_areas),
			envmap{envmap},
			envmap_w{envmap_w},
			envmap_h{envmap_h},
			cam(camera),
			bvh{vtxs, vtris}, 
			light_pdf_area_coeff(light_tris_areas.size() * light_tris_areas.back()) {}

};

#endif
