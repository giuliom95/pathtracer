#include "io.hpp"

#include <fstream>
#include <string>
#include <iostream>

Scene io::loadOBJ(std::string path) {
	std::ifstream input{path};

	std::vector<Vec3f> vtxs;
	std::vector<Vec3f> norms;

	std::vector<Vec3i> vtris;
	std::vector<Vec3i> ntris;

	while(!input.eof()) {
		std::string head;
		input >> head;
		if(head == "v") {
			Vec3f v;
			input >> v[0] >> v[1] >> v[2];
			vtxs.push_back(v);
		} else if(head == "vn") {
			Vec3f n;
			input >> n[0] >> n[1] >> n[2];
			n = normalize(n);
			norms.push_back(n);
		} else if(head == "f") {
			// Remember that OBJ start counting from one
			Vec3i vtri;
			Vec3i ntri;
			for(auto i = 0; i < 3; ++i) {
				std::string elem;
				input >> elem;
				const auto sep = elem.find('/');
				vtri[i] = std::stoi(elem.substr(0, sep)) - 1;
				ntri[i] = std::stoi(elem.substr(sep+2, elem.length()-(sep+2))) - 1;
			}
			vtris.push_back(vtri);
			ntris.push_back(ntri);
		}
	}

	// Let's assume that OBJ contains only one mesh
	std::vector<Mesh> meshes{1, {0, 0, (int)vtris.size()}};
	
	// Dummy fixed camera
	Camera cam{{0, 2, 4}, {0, 0, -1}, {0,1,0}};

	return {vtxs, norms, vtris, ntris, meshes, cam};
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
