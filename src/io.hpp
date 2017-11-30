#ifndef IO_HPP
#define IO_HPP

#include "math.hpp"
#include "scene.hpp"

#include <string>
#include <map>
#include <vector>
#include <ImfRgbaFile.h>
#include <ImfRgba.h>

namespace io {

	static const int defaultScreenWidth = 1920;
	static const int defaultScreenHeight = 1080;
	static const int defaultPixelSamples = 1;

	Scene loadOBJ(std::string path, int w, int h);

	void saveEXR(   std::string path, 
					const int w, const int h, 
					const std::vector<Vec4h>& image);

	bool parseArgs(	int argc, char** argv, 
					int& w, int& h, int& s, 
					std::string& in, std::string& out);
}
#endif
