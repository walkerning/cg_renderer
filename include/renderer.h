// Different renderers
#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "renderer.h"
#include "env.h"
#include <cstdio> // for dbg

struct Renderer;

typedef bool (*TraceCallback)(Renderer* render, Ray& ray_in, Ray& ray_out, Path& path, Object* obj, Vec3 intersection, BRDF* brdf, int depth);
typedef std::unordered_map<std::string, std::string> RendererConf;

std::string find_with_default(const RendererConf& conf, std::string key, std::string def);

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
    im_width = std::stoi(find_with_default(conf, "im_width", "512"));
    im_height = std::stoi(find_with_default(conf, "im_height", "384"));
    im_dist = std::stoi(find_with_default(conf, "im_dist", "2"));
    // the image width
    im_size_ratio = std::stod(find_with_default(conf, "im_size_ratio", "0.015625"));
    rr_depth = std::stoi(find_with_default(conf, "rr_depth", "5"));
    max_depth = std::stoi(find_with_default(conf, "max_depth", "20"));
    im = new Vec3[im_height * im_width];
  }

  inline void set_env(Environment* env_) {
    env = env_;
  }

  inline static Renderer* get_renderer(const std::string conf_fname) {
    return get_renderer(read_conf(conf_fname));
  }
  
  inline static bool write_conf(RendererConf conf, const std::string conf_name) {
    std::ofstream fs(conf_name);
    for (auto& c : conf) {
      fs << c.first << " = " << c.second << std::endl;
    }
    fs.close();
    if (!fs) {
      std::cerr << "ERROR: [write conf] write configuration to `" << conf_name << "` failed." << std::endl;
      return false;
    }
    return true;
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
      iss >> eq >> value >> std::ws;
      if (eq.compare(std::string("=")) != 0 || iss.get() != EOF) {
        std::cerr << "Wrong config line: " << str << std::endl;
        continue;
      }
      conf[name] = value;
    }
    // print configuration
    std::cerr << "Renderer configuration:" << std::endl;
    for (auto it = conf.begin(); it != conf.end(); ++it )
      std::cerr << it->first << ": " << it->second << std::endl;
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

struct _Register {
  _Register(const std::string& type,
            Renderer* (*creator)(const RendererConf& conf));
};

#define REGISTER_RENDERER(struct_name, type)		\
  Renderer* _creator_##type(const RendererConf& conf) { \
    return new struct_name(conf);                       \
  }                                                     \
  _Register _register_##type(#type, _creator_##type)
