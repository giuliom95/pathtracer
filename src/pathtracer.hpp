#ifndef PATHTRACER_HPP
#define PATHTRACER_HPP

#include "scene.hpp"
#include "io.hpp"

#include <queue>
#include <cmath>

#include <thread>
#include <algorithm>

#define MAX_BOUNCES 4

void pathtrace(const Scene& scn, int w, unsigned h, int samples, std::vector<Vec4h>& img);

#endif