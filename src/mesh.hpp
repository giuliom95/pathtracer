#ifndef MESH_HPP
#define MESH_HPP

#include "material.hpp"

class Mesh {
public:
	const int t0;			// Index of the first triangle 
	const int ntris;		// Number of tris
	const Material& mat;

	Mesh(int t0, int ntris, const Material& mat)
		: t0(t0), ntris(ntris), mat(mat) {}
};

#endif
