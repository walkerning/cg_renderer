#pragma once
#include <utility>
#include <cmath>
#include "vec3.h"
#include "path.h"

using std::pair;

struct BRDF {
  virtual bool is_specular() = 0;

  // A indirect get layer for mixed BRDF. Random select a BRDF that is working.
  virtual BRDF* get(Path& path);

  // For refraction materials
  virtual void apply_absorption(Ray& ray, Vec3 start, Vec3 end, Vec3 normal);

  // FIXME: in fact intersection is unrelated to BRDF calculation... should it be moved out?
  virtual Ray sample_ray(Path& path, Ray ray_in, Vec3 normal, Vec3 intersection) = 0;
};

struct Reflection: BRDF {
  Vec3 decay;

  Reflection(Vec3 decay_);

  virtual bool is_specular();

  virtual Ray sample_ray(Path& path, Ray ray_in, Vec3 normal, Vec3 intersection);
};

struct Refraction: BRDF {
  double refract_index; // 折射率
  Vec3 reflect_decay; // decay factor for different color
  Vec3 refract_decay;
  Vec3 absorption_decay; // exponential absorption

  Refraction(double refract_index_,
             Vec3 reflect_decay_=Vec3(1, 1, 1),
             Vec3 refract_decay_=Vec3(1, 1, 1),
             double absorption_decay_=1);

  virtual bool is_specular();

  virtual void apply_absorption(Ray& ray, Vec3 start, Vec3 end, Vec3 normal);

  virtual Ray sample_ray(Path& path, Ray ray_in, Vec3 normal, Vec3 intersection);
};

struct Diffuse: BRDF {
  Vec3 decay;

  Diffuse(Vec3 decay_);

  virtual bool is_specular();

  virtual Ray sample_ray(Path& path, Ray ray_in, Vec3 normal, Vec3 intersection);
};

struct Mixed: BRDF {
  vector<pair<BRDF*, double> > brdfs;
  double prob_sum;

  virtual bool is_specular();

  Mixed(const vector<pair<BRDF*, double> >& brdfs_);
  virtual BRDF* get(Path& path);
  virtual Ray sample_ray(Path& path, Ray ray_in, Vec3 normal, Vec3 intersection);
};
