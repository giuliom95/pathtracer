#include "scene.hpp"
#include "io.hpp"

#include <queue>
#include <cmath>

#include <thread>

#define MAX_BOUNCES 4


const Vec3f sample_hemisphere(Vec3f p, Vec3f n, const RndGen& rg) {
	const auto r2 = rg.next_float();
	const auto a = 2*PI*rg.next_float();
	const auto b = std::sqrt(1-r2);

	const Vec3f o{b*std::cos(a), b*std::sin(a), r2};

	const auto mat = refFromVec(n);
	return transformVector(mat, {r2, b*std::sin(a), b*std::cos(a)});
}


Vec3f estimate_li(const Ray ray, const Scene& scene, int bounces, const RndGen& rg) {

	int triangle;
	Vec3f tuv;

	const Mesh* mesh = scene.intersect(ray, triangle, tuv);
	if(mesh != nullptr) {
		if (bounces == 0)
			return mesh->mat.ke;

		const auto ntri = scene.ntris[mesh->t0 + triangle];
		const auto n0 = scene.norms[ntri[0]];
		const auto n1 = scene.norms[ntri[1]];
		const auto n2 = scene.norms[ntri[2]];	
		const auto n = (1-tuv[1]-tuv[2])*n0 + tuv[1]*n1 + tuv[2]*n2;
		const auto p = ray.o + tuv[0]*ray.d;

		const auto kd = mesh->mat.kd;

		const auto d = sample_hemisphere(p, n, rg);
		const auto li = estimate_li({p+0.0001*n, d}, scene, bounces-1, rg);
		const auto lr = li*kd;

		return mesh->mat.ke + lr;
	} else {
		return {};
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
					img[buf_idx] = {0, 0, 0, 1};
					for (auto s = 0; s < samples; ++s) {
						const auto uv = scn.cam.sample_camera(i, j, h, rg);
						const auto r = scn.cam.generateRay(uv);
						const auto pix = estimate_li(r, scn, MAX_BOUNCES, rg); 
						img[buf_idx][0] += pix[0];
						img[buf_idx][1] += pix[1];
						img[buf_idx][2] += pix[2];
					}
					img[buf_idx][0] /= samples; 
					img[buf_idx][1] /= samples;
					img[buf_idx][2] /= samples;
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

