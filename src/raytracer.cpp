
#include "scene.hpp"
#include "io.hpp"

#include <queue>
#include <cmath>

#include <thread>

Vec4h eval_ray(const Ray ray, const Scene& scene) {

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

	std::vector<Vec4h> img{(size_t)w*h, {0, 0, 0, 0}};

	auto nthreads = std::thread::hardware_concurrency();
	auto threads = std::vector<std::thread>();
	for (auto tid = 0; tid < nthreads; tid++) {
		threads.push_back(std::thread([=, &img]() {
			for (auto j = tid; j < h; j += nthreads) {
				for (auto i = 0; i < w; i++) {
					img[i+j*w] = {0, 0, 0, 0};
					for (auto sj = 0; sj < s; sj++) {
						for (auto si = 0; si < s; si++) {
							auto u = (i + (si + 0.5f) / s) / w;
							auto v = ((h - j) + (sj + 0.5f) / s) / h;
							auto r = scene.cam.generateRay({u,v});
							img[i+j*w] += eval_ray(r, scene);
						}
					}
				}
			}
		}));
	}
	for (auto& thread : threads) thread.join();

	io::saveEXR(out, w, h, img);
	return 0;
}