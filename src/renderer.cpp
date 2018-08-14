#include "scene.hpp"
#include "io.hpp"

#include "pathtracer.hpp"


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
			const auto p = r.o + tuv[0]*r.d;
			if(tid >= 0) {
				/*img[buf_idx][0] += tid + 1;
				img[buf_idx][1] += tid + 1;
				img[buf_idx][2] += tid + 1;*/
				img[buf_idx][0] += p[0];
				img[buf_idx][1] += p[1];
				img[buf_idx][2] += p[2];
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

	pathtrace(scene, w, h, s, img);
	//debug_rt(scene, w, h, s, img);

	io::saveEXR(out, w, h, img);
}

