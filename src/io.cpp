#include "io.hpp"

#include <fstream>
#include <iostream>

Scene io::loadOBJ(std::string path) {
	std::ifstream input{path};

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


void io::saveEXR(	std::string path, 
					const int w, const int h, 
					const std::vector<Vec4h>& image) {

	Imf::RgbaOutputFile file(path.c_str(), w, h, Imf::WRITE_RGBA);
	file.setFrameBuffer((Imf::Rgba*)image.data(), 1, w);
	file.writePixels(h);
}
