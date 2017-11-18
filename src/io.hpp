#ifndef IO_HPP
#define IO_HPP

#include "math.hpp"
#include "scene.hpp"

#include <string>
#include <vector>
#include <ImfRgbaFile.h>
#include <ImfRgba.h>

namespace io {

    Scene loadOBJ(std::string path);

    void saveEXR(   std::string path, 
                    const int w, const int h, 
                    const std::vector<Vec4h>& image);
}
#endif