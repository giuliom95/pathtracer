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
			--f[0]; --f[1]; --f[2];
			tris.push_back(f);
		}
	}

	std::vector<Mesh> meshes;
	Mesh m{0, (int)tris.size()};
	meshes.push_back(m);

	Camera cam{{0, 0, 3}, {0, 0, -1}, {0,1,0}};

	return {vtxs, tris, meshes, cam};
}


void io::saveEXR(	std::string path, 
					const int w, const int h, 
					const std::vector<Vec4h>& image) {

	Imf::RgbaOutputFile file(path.c_str(), w, h, Imf::WRITE_RGBA);
	file.setFrameBuffer((Imf::Rgba*)image.data(), 1, w);
	file.writePixels(h);
}

bool io::parseArgs(	int argc, char** argv, 
					int& w, int& h, int& s, 
					std::string& in, std::string& out) {

	w = defaultScreenWidth;
	h = defaultScreenHeight;
	s = defaultPixelSamples;

	auto optCount = 0;
	for(auto i = 1; i < argc; ++i) {
		if(argv[i][0] == '-') {
			if(argv[i][1] == 'w')
				w = std::stoi(argv[i+1]);
			if(argv[i][1] == 'h')
				h = std::stoi(argv[i+1]);
			if(argv[i][1] == 's')
				s = std::stoi(argv[i+1]);
			++i;
		} else {
			if(optCount == 0)
				in = std::string(argv[i]);
			else
				out = std::string(argv[i]);
			++optCount;
		}
	}

	if(in.size() == 0) {
		std::cerr << "No input specified" << std::endl;
		return false;
	}

	if(out.size() == 0) {
		std::cerr << "No output specified" << std::endl;
		return false;
	}

	return true;
}
