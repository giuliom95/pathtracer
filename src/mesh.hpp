#ifndef MESH_HPP
#define MESH_HPP

class Mesh {
public:
	const int t0;
	const int ntris;

	Mesh(int t0, int ntris) : t0(t0), ntris(ntris) {}
};

#endif
