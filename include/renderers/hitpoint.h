#pragma once
#include "vec3.h"

struct HitPoint {
  Vec3 position;
  Vec3 normal;
  Vec3 weight;
  int pixel_x;
  int pixel_y;
  double radius_sqr; // square of radius
  int N; // accumulated photon count
  int M; // accumulated photon count in current photon tracing pass
  Vec3 flux; // accumulated flux
};
