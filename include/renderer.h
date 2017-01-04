// Different renderers
#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "renderer.h"
#include "env.h"

struct Renderer;

typedef bool (*TraceCallback)(Renderer* render, Ray& ray_in, Ray& ray_out, Path& path, Object* obj, Vec3 intersection, BRDF* brdf, int depth);
typedef std::unordered_map<std::string, std::string> RendererConf;

std::string find_with_default(const RendererConf& conf, std::string key, std::string def) {
  auto got = conf.find(key);
  if (got == conf.end()) {
    return def;
  } else {
    return got->second;
  }
}

struct Renderer {
  Vec3* im;
  Environment* env;
  int im_height, im_width;
  double im_dist;
  double im_size_ratio;

  // Tracing configuration
  int rr_depth; // after `rr_depth` tracing, apply unbiased Russian Roulette to prune tracing.
  int max_depth; // max tracing depth

  Renderer(const RendererConf& conf) {
    im_height = std::stoi(find_with_default(conf, "im_height", "512"));
    im_width = std::stoi(find_with_default(conf, "im_width", "384"));
    im_dist = std::stoi(find_with_default(conf, "im_dist", "100"));
    im_size_ratio = std::stoi(find_with_default(conf, "im_size_ratio", "0.5"));
    rr_depth = std::stoi(find_with_default(conf, "rr_depth", "10"));
    max_depth = std::stoi(find_with_default(conf, "max_depth", "100"));
    im = new Vec3[im_height * im_width];
  }

  inline void set_env(Environment* env_) {
    env = env_;
  }

  inline static Renderer* get_renderer(const std::string conf_fname) {
    return get_renderer(read_conf(conf_fname));
  }

  inline static RendererConf read_conf(const std::string conf_fname) {
    std::ifstream fs(conf_fname);
    std::string str;
    RendererConf conf;
    while (std::getline(fs, str)) {
      std::istringstream iss(str);
      std::string name, eq, value;
      if (!(iss >> name)) { // blank line
        continue;
      }
      if (name[0] == '#') { // comment
        continue;
      }
      if (!(iss >> eq >> value >> std::ws) || eq.compare(std::string("=")) != 0 || iss.get() != EOF) {
        std::cerr << "Wrong config line: " << str << std::endl;
        continue;
      }
      conf[name] = value;
    }
    return conf;
  }

  static Renderer* get_renderer(const RendererConf& conf);

  void render() {
    if (!env) {
      std::cerr << "Environment is not set for this render. Cannot render." << std::endl;
      return;
    }
    do_render();
  }

  virtual void do_render() = 0;

  void trace(Ray& ray, Path& path, TraceCallback cb, int max_depth=100);

  virtual ~Renderer() {
    delete[] im;
  }
};

// registry for renderers
typedef Renderer* (*RendererCreator)(const RendererConf&);
std::unordered_map<std::string, RendererCreator> RenderRegistry;

struct _Register {
  _Register(const std::string& type,
            Renderer* (*creator)(const RendererConf& conf)) {
    auto got = RenderRegistry.find(type);
    if (got != RenderRegistry.end()) {
      std::cerr << "Render type `" << type << "` is registered more than once! Ignore multiple registration." << std::endl;
      return;
    }
    RenderRegistry[type] = creator;
  }
};

#define REGISTER_RENDERER(struct_name, type) \
  Renderer* _creator_##type(const RendererConf& conf) { \
    return new struct_name(conf);                       \
  }                                                     \
  _Register _register_##type(#type, _creator_##type)
