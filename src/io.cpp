#include "io.hpp"

#include <fstream>
#include <string>
#include <map>
#include <iostream>

Scene io::loadOBJ(std::string path, int w, int h) {

	std::ifstream input{path};
	
	if(!input) {
		std::cerr << "ERROR: Impossible to open \"" << path << "\"!\n";
		throw;
	}

	Vec3f cam_eye, cam_view, cam_up;

	std::vector<Vec3f> vtxs;
	std::vector<Vec3f> norms;

	std::vector<Vec3i> vtris;
	std::vector<Vec3i> ntris;

	std::vector<Mesh> meshes;
	std::vector<Material> mats;
	std::map<std::string, int> mats_map;

	std::vector<int> lights;

	mats.push_back({});

	auto lastt0 = 0;
	auto cur_mat_idx = 0;
	while(!input.eof()) {
		std::string head;
		input >> head;
		if(head == "mtllib") {
			std::string mtl_file;
			input >> mtl_file;
			// Build the path to the mtl file using the obj path
			const auto pos = path.find_last_of('/');
    		const auto mtl_path = path.substr(0, pos+1);
			std::ifstream mtl_input{mtl_path + mtl_file};
			if(!mtl_input) {
				std::cerr << "ERROR: Impossible to open the MTL file \"" << mtl_file << "\" from \"" << path << "\"!\n";
				throw;
			}

			std::string mat_name{""};
			Vec3f kd, ke;
			while(!mtl_input.eof()) {
				std::string mtl_head;
				mtl_input >> mtl_head;
				if(mtl_head == "newmtl") {
					if(mat_name != "") {
						mats.push_back({kd, ke});
						mats_map.insert(std::make_pair(mat_name, mats.size()-1));
					}
					mtl_input >> mat_name;
					kd = {};
					ke = {};
				} else if(mtl_head == "Kd") {
					mtl_input >> kd[0] >> kd[1] >> kd[2];
				} else if(mtl_head == "Ke") {
					mtl_input >> ke[0] >> ke[1] >> ke[2];
				}
			}

			mats.push_back({kd, ke});
			mats_map.insert(std::make_pair(mat_name, mats.size()-1));

		} else if(head == "v") {
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
				const auto sep1 = elem.find('/');
				vtri[i] = std::stoi(elem.substr(0, sep1)) - 1;

				elem = elem.substr(sep1+1, elem.length()-(sep1+1));
				const auto sep2 = elem.find('/');
				ntri[i] = std::stoi(elem.substr(sep2+1, elem.length()-(sep2+1))) - 1;
			}
			vtris.push_back(vtri);
			ntris.push_back(ntri);
		} else if(head == "o") {
			// Save mesh data
			if(!vtris.empty()) {
				meshes.push_back({lastt0, (int)vtris.size()-lastt0, cur_mat_idx});
				lastt0 = (int)vtris.size();

				const auto ke = mats[cur_mat_idx].ke;
				if(dot(ke, ke) > 0)
					lights.push_back(meshes.size() - 1);
			}
		} else if(head == "usemtl") {
			std::string mat_name;
			input >> mat_name;
			cur_mat_idx = mats_map.at(mat_name);
		} else if(head == "c") {
			input >> cam_eye[0]		>> cam_eye[1]	>> cam_eye[2];
			input >> cam_view[0] 	>> cam_view[1]	>> cam_view[2];
			input >> cam_up[0]		>> cam_up[1]	>> cam_up[2];
		}
	}

	meshes.push_back({lastt0, (int)vtris.size()-lastt0, cur_mat_idx});
	const auto ke = mats[cur_mat_idx].ke;
	if(dot(ke, ke) > 0)
		lights.push_back(meshes.size() - 1);

	Camera cam{cam_eye, cam_view, cam_up, 1, (float)(w)/h};

	return {vtxs, norms, vtris, ntris, meshes, mats, lights, cam};
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
					std::string& in,
					std::string& out) {

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
		std::cerr << "No obj specified" << std::endl;
		return false;
	}

	if(out.size() == 0) {
		std::cerr << "No output specified" << std::endl;
		return false;
	}

	return true;
}
