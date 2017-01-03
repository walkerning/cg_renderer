#pragma once
#include "vec3.h"

// AABB: axis-aligned bounding box
struct BBox {
  Vec3 min, max;
  bool initialized;

  BBox(): initialized(false) {}

  BBox(Vec3 initial_p): min(initial_p), max(initial_p), initialized(true) {}

  bool intersect(const Ray& ray, double& t) {
    if (initialized) {
      return false;
    }
    for (int i = 0; i < 3; i++){ //we test slabs in every direction
      if (ray.dir[i] == 0) { // ray parallel to planes in this direction
        if ((ray.ori[i] < min[i]) || (ray.ori[i] > max[i])) {
          return false; // parallel AND outside box : no intersection possible
        }
      }
    }
    Vec3 dirfrac = Vec3(1.0/ray.dir.x, 1.0/ray.dir.y, 1.0/ray.dir.z);

    // `min` is the corner of AABB with minimal coordinates - left bottom, `max` is maximal corner
    Vec3 t1 = (min - ray.ori) * dirfrac;
    Vec3 t2 = (max - ray.ori) * dirfrac;

    float tmin = MAX(MAX(MIN(t1.x, t2.x), MIN(t1.y, t2.y)), MIN(t1.z, t2.z));
    float tmax = MIN(MIN(MAX(t1.x, t2.x), MAX(t1.y, t2.y)), MAX(t1.z, t2.z));

    // if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
    if (tmax < 0) {
      t = tmax;
      return false;
    }
    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax) {
      t = tmax;
      return false;
    }
    t = tmin;
    return true;
  }

  void fit(Vec3 point) {
    if (!initialized) {
      min = max = point;
      initialized = true;
      return;
    }
    min = min_(min, point);
    max = max_(max, point);
  }

  void merge(const BBox& bbox) {
    if (!initialized) {
      min = bbox.min;
      max = bbox.max;
      initialized = bbox.initialized;
      return;
    }
    min = min_(min, bbox.min);
    max = max_(max, bbox.max);
  }

  int max_dim() {
    Vec3 extent = max - min;
    if (extent[1] > extent[0] && extent[1] > extent[2]) {
      return 1;
    }
    if (extent[2] > extent[0] && extent[2] > extent[1]) {
      return 2;
    }
    return 0;
  }
};
