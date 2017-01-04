#include "renderer.h"

Renderer* Renderer::get_renderer(const RendererConf& conf) {
  std::string type;
  auto got = conf.find("type");
  if (got == conf.end()) {
    type = "adaptive_photon_mapper"; // default type
  } else {
    type = got->second;
  }
  return RenderRegistry[type](conf);
}

void Renderer::trace(Ray& ray, Path& path, TraceCallback cb, int max_depth) {
  int depth = 0;
  Object* obj;
  double t;
  for (; depth < max_depth; depth++) {
    bool intersected = env->intersect(ray, obj, t);
    if (!intersected) {
      break;
    }
    Vec3 intersection = ray.ori + ray.dir * t;
    Vec3 normal = obj->get_normal(intersection);

    BRDF* brdf = obj->brdf->get(path);
    brdf->apply_absorption(ray, ray.ori, intersection, normal);
    Ray ray_out = brdf->sample_ray(path, ray, normal, intersection);
    if (!cb(this, ray, ray_out, path, obj, intersection, brdf, depth)) {
      break;
    }
    ray = ray_out;
  }
}
