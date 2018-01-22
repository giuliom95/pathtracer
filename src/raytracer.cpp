#include "scene.hpp"
#include "io.hpp"

#include <queue>
#include <cmath>

#include <thread>




Vec4h eval_ray(const Ray ray, const Scene& scene) {

	int triangle;
	Vec3f tuv;

	const Mesh* mesh = scene.intersect(ray, triangle, tuv);
	if(mesh != nullptr) {
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


void raytrace_mt(const Scene& scn, int w, int h, int samples, std::vector<Vec4h>& img) {

	RndGen rg{};

	const auto nthreads = std::thread::hardware_concurrency();
	auto threads = std::vector<std::thread>();
	for (auto tid = 0; tid < nthreads; tid++) {
		threads.push_back(std::thread([=, &scn, &img]() {
			for (auto j = tid; j < h; j += nthreads) {
				for (auto i = 0; i < w; i++) {
					const auto buf_idx = i+j*w;
					img[buf_idx] = {0, 0, 0, 0};
					for (auto s = 0; s < samples; ++s) {
						const auto uv = scn.cam.sample_camera(i, j, h, rg);
						const auto r = scn.cam.generateRay(uv);
						img[buf_idx] += eval_ray(r, scn);
					}
					img[buf_idx][0] /= samples; 
					img[buf_idx][1] /= samples;
					img[buf_idx][2] /= samples;
					img[buf_idx][3] /= samples;
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

	raytrace_mt(scene, w, h, s, img);

	io::saveEXR(out, w, h, img);
}

