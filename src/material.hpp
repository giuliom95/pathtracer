#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "math.hpp"

class Material {
public:
	const Vec3f kd;
    const Vec3f ke;

    Material(const Vec3f& kd, const Vec3f& ke)
        : kd(kd), ke(ke) {}

    Material() : kd(), ke() {}
};

#endif
