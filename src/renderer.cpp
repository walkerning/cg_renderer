#include "renderer.h"
#include "cstdio"

std::unordered_map<std::string, RendererCreator> RenderRegistry;

Renderer* Renderer::get_renderer(const RendererConf& conf) {
  std::string type;
  auto got = conf.find("type");
  if (got == conf.end()) {
    type = "ada_photon_mapper"; // default type
  } else {
    type = got->second;
  }
  return RenderRegistry[type](conf);
}

void Renderer::trace(Ray& ray, Path& path, TraceCallback cb, int max_depth) {
  int depth = 0;
  Object* obj;
  double t;
  // Ray ray = ray_st;
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

_Register::_Register(const std::string& type,
                     Renderer* (*creator)(const RendererConf& conf)) {
  auto got = RenderRegistry.find(type);
  if (got != RenderRegistry.end()) {
    std::cerr << "Render type `" << type << "` is registered more than once! Ignore multiple registration." << std::endl;
    return;
  }
  std::cerr << "INFO: <RenderRegistry> register render type `" << type << "`\n";
  RenderRegistry[type] = creator;
}

std::string find_with_default(const RendererConf& conf, std::string key, std::string def) {
  auto got = conf.find(key);
  if (got == conf.end()) {
    return def;
  } else {
    return got->second;
  }
}
