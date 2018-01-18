#ifndef IO_HPP
#define IO_HPP

#include "math.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "scene.hpp"

#include <string>
#include <vector>
#include <ImfRgbaFile.h>
#include <ImfRgba.h>

namespace io {

	static const int defaultScreenWidth = 960;
	static const int defaultScreenHeight = 540;
	static const int defaultPixelSamples = 64;

	Scene loadOBJ(std::string path, int w, int h);

	void saveEXR(   std::string path, 
					const int w, const int h, 
					const std::vector<Vec4h>& image);

	bool parseArgs(	int argc, char** argv, 
					int& w, int& h, int& s, 
					std::string& in,
					std::string& out);
}
#endif
