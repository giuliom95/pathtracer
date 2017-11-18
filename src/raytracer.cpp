
#include "scene.hpp"
#include "io.hpp"

int main() {

	Scene scene = io::loadOBJ("./bunny.obj");

	printf("Vtx: %i, Tris: %i\n", scene.vtxs.size(), scene.tris.size());
	for(auto m : scene.meshes)
		printf("Mesh -> start: %i, ntris: %i\n", m.vtx0, m.ntris);

	std::vector<Vec4h> image{1920*1080, {1, .5, .5, 1}};

	io::saveEXR("out/test2.exr", 300, 300, image);
}