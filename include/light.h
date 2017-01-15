#pragma once
#include "vec3.h"
#include "path.h"

struct Light {
  virtual Ray sample_ray(Path& path) = 0;
};

struct RadPointLight: Light {
  Vec3 position;
  double radius;
  Vec3 flux;

  RadPointLight(Vec3 pos, Vec3 flux_, double radius_=0): position(pos), flux(flux_), radius(radius_) {}

  virtual Ray sample_ray(Path& path) {
    Vec3 dir = path_to_dir(path);
    // if (dir.dot()
    return Ray(position + dir * radius, dir, flux);
  }
};
