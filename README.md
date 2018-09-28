# Path tracer

A path tracer written in modern C++.  
### Details:
* Multithread.
* Uses a BVH to accelerate the ray-scene intersection.
* The only dependency is OpenEXR.
* The input can be a triangulated OBJ + MTL scene.
* Exr enviroment maps.

### Weak points:
* Texture maps are not supported.
* Phong BRDF only.
* The project uses `std::mt19937` as random number generator.
* No Russian Roulette.