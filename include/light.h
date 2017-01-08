#pragma once
#include "vec3.h"
#include "path.h"

struct Light {
  virtual Ray sample_ray(Path& path) = 0;
};

struct RadPointLight: Light {
  Vec3 position;
  double radius;

  RadPointLight(Vec3 pos, double radius_=0): position(pos), radius(radius_) {}

  virtual Ray sample_ray(Path& path) {
    Vec3 dir = path_to_dir(path);
    // if (dir.dot()
    return Ray(position + dir * radius, dir, Vec3(1000, 3800, 2000));
  }
};
