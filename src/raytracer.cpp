#include "scene.hpp"
#include "io.hpp"

#include <queue>
#include <cmath>

#include <thread>

#define MAX_BOUNCES 4


const Vec3f sample_hemisphere(Vec3f n, const RndGen& rg) {
	const auto r0 = rg.next_float();
	const auto r1 = rg.next_float();
	const auto z = std::sqrt(r0);
    const auto r = std::sqrt(1 - z * z);
    const auto phi = 2 * PI * r1;

	const auto mat = refFromVec(n);
	return transformVector(mat, {z, r * std::cos(phi), r * std::sin(phi)});
}


Vec3f estimate_li(const Ray ray, const Scene& scene, int bounces, const RndGen& rg) {

	int tid;	// Triangle index relative to scene.ntris
	Vec3f tuv;	// Ray param and uv coords of triangle

	const Mesh* mesh = scene.intersect(ray, tid, tuv);
	if(mesh != nullptr) {
		auto mat = scene.mats[mesh->mat_idx];
		if (bounces == 0)
			return mat.ke;

		const auto ntri = scene.ntris[tid];
		const auto n0 = scene.norms[ntri[0]];
		const auto n1 = scene.norms[ntri[1]];
		const auto n2 = scene.norms[ntri[2]];	
		const auto n = (1-tuv[1]-tuv[2])*n0 + tuv[1]*n1 + tuv[2]*n2;
		const auto p = ray.o + tuv[0]*ray.d;

		const auto d = sample_hemisphere(n, rg);
		const auto li = estimate_li({p+0.0001*n, d}, scene, bounces-1, rg);
		const auto lr = li*mat.kd;

		return mat.ke + lr;
	} else {
		return {};
	}
}


void raytrace_mt(const Scene& scn, int w, unsigned h, int samples, std::vector<Vec4h>& img) {

	RndGen rg{};

	const auto nthreads = std::thread::hardware_concurrency();
	auto threads = std::vector<std::thread>();
	for (unsigned tid = 0; tid < nthreads; tid++) {
		threads.push_back(std::thread([=, &scn, &img]() {
			for (unsigned j = tid; j < h; j += nthreads) {
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


void debug_rt(const Scene& scn, int w, unsigned h, int samples, std::vector<Vec4h>& img) {
	RndGen rg{};
	for (unsigned j = 0; j < h; ++j) {
		for (auto i = 0; i < w; ++i) {
			const auto buf_idx = i+j*w;
			img[buf_idx] = {0, 0, 0, 1};
			const auto uv = scn.cam.sample_camera(i, j, h, rg);
			const auto r = scn.cam.generateRay(uv);

			int tid;	// Triangle index relative to scene.ntris
			Vec3f tuv;	// Ray param, uv coords of triangle

			scn.intersect(r, tid, tuv);
			if(tid >= 0) {
				img[buf_idx][0] += tid + 1;
				img[buf_idx][1] += tid + 1;
				img[buf_idx][2] += tid + 1;
			}
		}
	}
}

int main(int argc, char** argv) {

	int w, h, s;
	std::string in, out;
	if(!io::parseArgs(argc, argv, w, h, s, in, out)) return 1;

	auto scene = io::loadOBJ(in, w, h);

	std::vector<Vec4h> img{(size_t)w*h, {0, 0, 0, 0}};

	raytrace_mt(scene, w, h, s, img);
	//debug_rt(scene, w, h, s, img);

	io::saveEXR(out, w, h, img);
}

