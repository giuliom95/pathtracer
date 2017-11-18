
#include "scene.hpp"
#include "io.hpp"

int main(int argc, char** argv) {

	int w, h, s;
	std::string in, out;
	if(!io::parseArgs(argc, argv, w, h, s, in, out)) return 1;


	auto scene = io::loadOBJ(in);

	printf("Vtx: %i, Tris: %i\n", scene.vtxs.size(), scene.tris.size());
	for(auto m : scene.meshes)
		printf("Mesh -> start: %i, ntris: %i\n", m.vtx0, m.ntris);

	std::vector<Vec4h> image{(size_t)w*h, {1, .5, .5, 1}};

	io::saveEXR(out, w, h, image);

	return 0;
}