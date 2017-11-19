
#include "scene.hpp"
#include "io.hpp"

#include <queue>
#include <cmath>

Vec4h eval_sample(const Ray& ray, const Scene& scene) {

	for(auto m : scene.meshes) {
		for(auto t = 0; t < m.ntris; ++t) {
			auto tri = scene.tris[m.vtx0 + t];
			auto v0 = scene.vtxs[tri[0]];
			auto v1 = scene.vtxs[tri[1]];
			auto v2 = scene.vtxs[tri[2]];

			auto ints = intersectTriangle(ray, v0, v1, v2);
			if(ints[0] >= 0){
				return {1,0,0,1};
			} 
		}
	}
	return {0,0,0,0};
}


int main(int argc, char** argv) {

	int w, h, s;
	std::string in, out;
	if(!io::parseArgs(argc, argv, w, h, s, in, out)) return 1;

	auto scene = io::loadOBJ(in);
	printf("Vtx: %i, Tris: %i\n", scene.vtxs.size(), scene.tris.size());
	for(auto m : scene.meshes)
		printf("Mesh -> start: %i, ntris: %i\n", m.vtx0, m.ntris);

	std::vector<Vec4h> image{(size_t)w*h, {0, 0, 0, 0}};

	// Fills the queue with evenly distributed samples
	std::queue<Vec2f> samples;
	auto sample_step = 1.0f / (s+1);
	for(auto i = 0; i < w; ++i) {
		for(auto j = 0; j < h; ++j) {
			for(auto sx = 0; sx < s; ++sx) {
				for(auto sy = 0; sy < s; ++sy) {
					auto x = i + (sx+1)*sample_step;
					auto y = j + (sy+1)*sample_step;
					samples.push({x, y});
				}
			}
		}
	}

	while(!samples.empty()) {
		auto sample = samples.front();
		samples.pop();


		Vec2f uv{sample[0] / w, sample[1] / h};
		auto r = scene.cam.generateRay(uv);

		auto i = std::floor(sample[0]);
		auto j = std::floor(sample[1]);
		image[i + j*w] = image[i + j*w] + eval_sample(r, scene);
	}
	io::saveEXR(out, w, h, image);
	return 0;
}