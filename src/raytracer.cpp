
#include "scene.hpp"


int main() {

    std::ifstream input {"./bunny.obj"};
    Scene scene = Scene::loadOBJ(input);

    printf("Vtx: %i, Tris: %i\n", scene.vtxs.size(), scene.tris.size());
    for(auto m : scene.meshes)
        printf("Mesh -> start: %i, ntris: %i\n", m.vtx0, m.ntris);
}