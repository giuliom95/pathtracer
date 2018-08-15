#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "math.hpp"

class Material {
public:
	const Vec3f kd;
    const Vec3f ke;
    const Vec3f ks;
    const float exp;

    Material(const Vec3f& kd, const Vec3f& ke, const Vec3f& ks, const float exp)
        : kd(kd), ke(ke), ks(ks), exp(exp) {}

    Material() : kd(), ke(), ks(), exp(1.0f) {}
};

#endif
