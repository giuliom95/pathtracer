#include "pathtracer.hpp"

const Vec3f sample_bxdf(const Material& mat, const Vec3f n, const Vec3f v, const RndGen& rg) {
	const auto r0 = rg.next_float();
	const auto r1 = rg.next_float();
	const auto z = std::sqrt(r0);
    const auto r = std::sqrt(1 - z * z);
    const auto phi = 2 * PI * r1;

	const auto m = refFromVec(n);
	return transformVector(m, {z, r * std::cos(phi), r * std::sin(phi)});
}


const void sample_lights(const Scene& scn, const RndGen& rg, Vec3f& p, Vec3f& n) {
	const auto& cdf = scn.lgt_tris_areas;
	const auto r0 = rg.next_float() * cdf.back();
	const auto lgt_tri_idx = (int)(std::upper_bound(cdf.begin(), cdf.end(), r0) - cdf.begin());
	
	const auto tri_idx = scn.lgt_tris[lgt_tri_idx];

	const auto vtri = scn.vtris[tri_idx];
	const auto v0 = scn.vtxs[vtri[0]];
	const auto v1 = scn.vtxs[vtri[1]];
	const auto v2 = scn.vtxs[vtri[2]];

	const auto ntri = scn.ntris[tri_idx];
	const auto n0 = scn.norms[ntri[0]];
	const auto n1 = scn.norms[ntri[1]];
	const auto n2 = scn.norms[ntri[2]];

	const auto r1_sqrt = std::sqrt(rg.next_float());
	const auto r2 = rg.next_float();
	const auto a = r1_sqrt*(1 - r2);
	const auto b = (1 - r1_sqrt);
	const auto c = r1_sqrt*r2;

	p = a*v0 + b*v1 + c*v2;
	n = a*n0 + b*n1 + c*n2;
}

const Vec3f phong_brdf_cos(const Material& mat, const Vec3f& i, const Vec3f& o, const Vec3f& n) {
	const auto m = transpose(refFromVec(n));
	const auto loc_i = transformVector(m, i);
	const auto loc_o = transformVector(m, o);
	const Vec3f loc_neg_o{-loc_o[0], loc_o[1], loc_o[2]};
	const auto dot_res = dot(loc_i, loc_neg_o);
	return INV_PI * dot(n, i) * (std::pow(std::max(0.0f, dot_res), mat.exp)*mat.ks + mat.kd);
}

Vec3f estimate_li(const Ray ray, const Scene& scene, int bounces, const RndGen& rg) {
	if(bounces == 0) return {0,0,0};

	int tid;	// Triangle index relative to scene.ntris
	Vec3f tuv;	// Ray param and uv coords of triangle

	const auto mesh = scene.intersect(ray, tid, tuv);
	if(mesh == nullptr) {
		if(bounces < MAX_BOUNCES)
			return scene.sample_envmap(ray.d);
		else
			return {};
	}
	const auto ntri = scene.ntris[tid];
	const auto n0 = scene.norms[ntri[0]];
	const auto n1 = scene.norms[ntri[1]];
	const auto n2 = scene.norms[ntri[2]];	
	const auto n = normalize((1-tuv[1]-tuv[2])*n0 + tuv[1]*n1 + tuv[2]*n2);
	const auto p = ray.o + tuv[0]*ray.d;
	const auto mat = scene.mats[mesh->mat_idx];

	Vec3f lgt_p, lgt_n;
	sample_lights(scene, rg, lgt_p, lgt_n);
	const auto vec_dir = lgt_p - p;
	const auto i_dir = normalize(vec_dir);
	const Mesh* lgt_mesh = scene.intersect({p+0.0001*n, i_dir}, tid, tuv);
	const Vec3f li_dir = lgt_mesh != nullptr ? scene.mats[lgt_mesh->mat_idx].ke : Vec3f();
	const auto inv_dir_pdf = scene.light_pdf_area_coeff * std::abs(dot(lgt_n, n)) / dot(vec_dir, vec_dir);
	const auto lr_dir = inv_dir_pdf*li_dir*phong_brdf_cos(mat, i_dir, ray.d, n);

	const auto i_ind = sample_bxdf(mat, n, ray.d, rg);
	const Ray new_ray{p+0.0001*n, i_ind};
	const auto inv_ind_pdf = PI / dot(n, i_ind);
	return mat.ke + lr_dir + inv_ind_pdf * estimate_li(new_ray, scene, bounces-1, rg) * phong_brdf_cos(mat, i_ind, ray.d, n);
}

void pathtrace(const Scene& scn, int w, unsigned h, int samples, std::vector<Vec4h>& img) {

	const auto nthreads = std::thread::hardware_concurrency();
	auto threads = std::vector<std::thread>();
	for (unsigned tid = 0; tid < nthreads; tid++) {
		threads.push_back(std::thread([=, &scn, &img]() {
			RndGen rg{};
			for (unsigned j = tid; j < h; j += nthreads) {
				for (auto i = 0; i < w; i++) {
					const auto buf_idx = i+j*w;
					img[buf_idx] = {0, 0, 0, 1};
					for (auto s = 0; s < samples; ++s) {
						const auto uv = scn.cam.sample_camera(i, j, h, rg);
						const auto r = scn.cam.generateRay(uv);
						// const auto pix = estimate_li_prod(r, scn, MAX_BOUNCES, rg); 
						const auto pix = estimate_li(r, scn, MAX_BOUNCES, rg); 
						//const auto pix = estimate_li_prod_only_indirect(r, scn, MAX_BOUNCES, rg); 
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
