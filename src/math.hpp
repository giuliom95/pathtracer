#ifndef MATH_HPP
#define MATH_HPP

#include <array>
#include <cmath>
#include <limits>

#include <half.h>

#include <random>
#include <functional>

#define PI 3.141592654f
#define INV_PI 0.318309886f

inline const float max(const float a, const float b) {return a > b ? a : b;}
inline const float min(const float a, const float b) {return a < b ? a : b;}


//////// VECTOR ////////

using Vec2f = std::array<float, 2>;
using Vec3i = std::array<int, 3>;
using Vec3f = std::array<float, 3>; 
using Vec4h = std::array<half, 4>;

inline const float dot			(const Vec3f& a, const Vec3f& b) { return a[0]*b[0] + a[1]*b[1] + a[2]*b[2]; }
inline const Vec3f cross		(const Vec3f& a, const Vec3f& b) { return {a[1]*b[2] - a[2]*b[1], a[2]*b[0] - a[0]*b[2], a[0]*b[1] - a[1]*b[0]}; }
inline const Vec3f operator-	(const Vec3f& a, const Vec3f& b) { return {a[0]-b[0], a[1]-b[1], a[2]-b[2]}; }
inline const Vec3f operator+	(const Vec3f& a, const Vec3f& b) { return {a[0]+b[0], a[1]+b[1], a[2]+b[2]}; }
inline const Vec3f operator*	(const Vec3f& a, const Vec3f& b) { return {a[0]*b[0], a[1]*b[1], a[2]*b[2]}; }
inline const Vec3f operator*	(const float f,  const Vec3f& v) { return {f*v[0], f*v[1], f*v[2]}; }
inline const Vec3f operator/	(const Vec3f& v, const float f) { return {v[0]/f, v[1]/f, v[2]/f}; }
inline const float length		(const Vec3f& v) { return std::sqrt(dot(v, v)); }
inline const Vec3f normalize	(const Vec3f& v) { return (1 / length(v))*v; }

inline const void operator+=	(Vec4h& a, const Vec4h& b) { a[0]+=b[0]; a[1]+=b[1]; a[2]+=b[2]; a[3]+=b[3]; }

inline std::ostream& operator<<(std::ostream& os, const Vec3f& v) {
	return os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
}

inline std::ostream& operator<<(std::ostream& os, Vec3f& v) {
	return os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
}

//////// MATRIX ////////

class Mat4 {
	std::array<float, 16> data;
public:
	Mat4() : data {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1} {}

	Mat4(	float a00, float a01, float a02, float a03,
			float a10, float a11, float a12, float a13,
			float a20, float a21, float a22, float a23,
			float a30, float a31, float a32, float a33)
			: data{	a00, a01, a02, a03, a10, a11, a12, a13, 
					a20, a21, a22, a23, a30, a31, a32, a33} {}

	Mat4(	const Vec3f& x, const Vec3f& y, const Vec3f& z, const Vec3f& w)
			: data{	x[0], y[0], z[0], w[0],
					x[1], y[1], z[1], w[1],
					x[2], y[2], z[2], w[2],
						0,    0,    0,    1} {}

	const 	float& operator[](int idx) 		const	{ return data[idx]; }
			float& operator()(int i, int j)			{ return data[i*4+j]; }
	const 	float& operator()(int i, int j) const	{ return data[i*4+j]; }
};

inline const Vec3f transformPoint	(const Mat4& m, const Vec3f& p) { return {m(0,0)*p[0] + m(0,1)*p[1] + m(0,2)*p[2] + m(0,3), m(1,0)*p[0] + m(1,1)*p[1] + m(1,2)*p[2] + m(1,3), m(2,0)*p[0] + m(2,1)*p[1] + m(2,2)*p[2] + m(2,3)}; }
inline const Vec3f transformVector	(const Mat4& m, const Vec3f& v) { return {m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2], m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2], m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2]}; }

inline const Mat4 refFromVec (const Vec3f& v) {
	Vec3f v2{};
	if (std::abs(v[0]) > std::abs(v[1]))
		v2 = Vec3f{-v[2], 0, v[0]} / std::sqrt(v[0] * v[0] + v[2] * v[2]);
	else
		v2 = Vec3f{0, v[2], -v[1]} / std::sqrt(v[1] * v[1] + v[2] * v[2]);
	
	const auto v3 = cross(v, v2);
	return {v, v2, v3, {}};
}

inline const Mat4 transpose (const Mat4& m) {
	return {	m[0], m[4],  m[8], m[12],
				m[1], m[5],  m[9], m[13],
				m[2], m[6], m[10], m[14],
				m[3], m[7], m[11], m[15]};
}

//////// RAY ////////

class Ray {
public:
	Vec3f o, d, invd;
	float tmin, tmax;

	Ray(const Vec3f& o, const Vec3f& d, float tmin=0.0, float tmax=std::numeric_limits<float>::max())
		: o(o), d(d), invd{1/d[0], 1/d[1], 1/d[2]}, tmin(tmin), tmax(tmax) {}
};



//////// INTERSECTIONS ////////

// Möller-Trumbore ray-triangle intersection
// Returns t, u and v as a Vec3f
// TODO: Maybe aligning in memory the argument will lead to speed-ups. Gotta check this out.
inline Vec3f intersectTriangle(const Ray& r, Vec3f v0, Vec3f v1, Vec3f v2) {
	const auto v01 = v1 - v0;
	const auto v02 = v2 - v0;
	const auto h = cross(r.d, v02);
	const auto f = 1/dot(v01, h);
	const auto s = r.o - v0;
	const auto u = f * dot(s, h);
	if (u < 0 || u > 1)
		return {-1, 0, 0};
	const auto q = cross(s, v01);
	const auto v = f * dot(r.d, q);
	if (v < 0 || u + v > 1)
		return {-1, 0, 0};
	const auto t = f * dot(v02, q);
	return {t, u, v};
}



//////// RANDOM NUMBERS ////////

class RndGen {
    std::mt19937* gen;
    std::uniform_real_distribution<float>* distribution;
public:
    RndGen() {
		gen = new std::mt19937(42);
		distribution = new std::uniform_real_distribution<float>(0.0, 1.0);
	} 
    float next_float() const {return (*distribution)(*gen);}
};


#endif