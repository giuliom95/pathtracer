# Path tracer

![Sample image](https://giuliom95.github.io/projects/pt.jpg)

A recursive naive path tracer written in modern C++.  
Details:
* Multithread.
* Uses a BVH to accelerate the ray-scene intersection.
* The only dependency is OpenEXR.
* The input can be a triangulated OBJ + MTL scene.

Weak points:
* Texture maps are not supported (so no envmaps either)
* Lambert-like BRDF only.
* The project uses `std::mt19937` as random number generator.
* No Russian Roulette.