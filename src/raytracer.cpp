#include "scene.hpp"
#include "io.hpp"

#include <queue>
#include <cmath>

#include <thread>

Vec4h eval_ray(const Ray ray, const Scene& scene) {

	Mesh* mesh;
	int triangle;
	Vec3f tuv;

	if(scene.intersect(ray, mesh, triangle, tuv)) {
		printf("INT\n");
		const auto ntri = scene.ntris[mesh->t0 + triangle];
		const auto n0 = scene.norms[ntri[0]];
		const auto n1 = scene.norms[ntri[1]];
		const auto n2 = scene.norms[ntri[2]];

		auto normal = (1-tuv[1]-tuv[2])*n0 + tuv[1]*n1 + tuv[2]*n2;
		return {normal[0],normal[1],normal[2],1};
	} else {
		return {0,0,0,0};
	}
}

void raytrace(const Scene& scn, int w, int h, int s, std::vector<Vec4h>& img) {

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
							auto r = scn.cam.generateRay({u,v});
							img[i+j*w] += eval_ray(r, scn);
						}
					}
				}
			}
		}));
	}
	for (auto& thread : threads) thread.join();
}

int main(int argc, char** argv) {

	int w, h, s;
	std::string in, out;
	if(!io::parseArgs(argc, argv, w, h, s, in, out)) return 1;

	auto scene = io::loadOBJ(in, w, h);

	std::vector<Vec4h> img{(size_t)w*h, {0, 0, 0, 0}};

	raytrace(scene, w, h, s, img);

	//auto r = scene.cam.generateRay({.5, .5});
	//auto p = eval_ray(r, scene);
	//printf("INTERSECTION\n");

	io::saveEXR(out, w, h, img);
}

