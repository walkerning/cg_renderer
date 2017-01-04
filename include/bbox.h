#pragma once
#include "vec3.h"

// AABB: axis-aligned bounding box
struct BBox {
  Vec3 min, max;
  bool initialized;

  BBox();

  BBox(Vec3 initial_p);

  bool intersect(const Ray& ray, double& t);
  void fit(Vec3 point);
  void merge(const BBox& bbox);
  int max_dim();
};
