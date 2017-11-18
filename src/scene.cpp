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

Scene Scene::loadOBJ(std::istream& input) {

	std::vector<Vec3f> vtxs;
	std::vector<Vec3i> tris;

	while(!input.eof()) {
		std::string head;
		input >> head;
		if(head == "v") {
			Vec3f v;
			input >> v[0] >> v[1] >> v[2];
			vtxs.push_back(v);
		} else if(head == "f") {
			Vec3i f;
			input >> f[0] >> f[1] >> f[2];
			tris.push_back(f);
		}
	}

	std::vector<Mesh> meshes;
	Mesh m{0, (int)tris.size()};
	meshes.push_back(m);

	Camera cam{{0, -0.2, 7.5}, {0, 0, -1}, {0,1,0}};

	return {vtxs, tris, meshes, cam};
}