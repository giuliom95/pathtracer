#ifndef MESH_HPP
#define MESH_HPP

#include "material.hpp"

class Mesh {
public:
	const int t0;		// Index of the first triangle 
	const int ntris;	// Number of tris
	const int mat_idx;	// Material index relative to the material vector of the scene

	Mesh(int t0, int ntris, const int mat_idx)
		: t0(t0), ntris(ntris), mat_idx(mat_idx) {}
};

#endif
