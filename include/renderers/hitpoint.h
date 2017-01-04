#pragma once
#include "vec3.h"

struct HitPoint {
  HitPoint(Vec3 position_, Vec3 normal_, Vec3 weight_, int pixel_x_, int pixel_y_): position(position_),
                                                                                    normal(normal_),
                                                                                    weight(weight_),
                                                                                    pixel_x(pixel_x_),
                                                                                    pixel_y(pixel_y_),
                                                                                    N(0), M(0), flux(Vec3(0, 0, 0)) {}

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
