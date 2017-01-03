#pragma once
#include "vec3.h"
#include "path.h"

class Light {
  virtual Ray sample_ray() = 0;
};

class RadPointLight {
  Vec3 position;
  double radius;

  virtual Ray sample_ray(Path& path) {
    Vec3 dir = path_to_dir(path);
    return Ray(position + dir * radius, dir);
  }
};
