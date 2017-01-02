#pragma once
#include "vec3.h"

class Light {
  virtual Ray sample_ray() = 0;
};

class PointLight {
  virtual Ray sample_ray() {

  }
};
