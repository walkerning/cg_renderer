#pragma once
#include "object.h"

struct BVH {
  const vector<Object*>& objects;

  BVH(const vector<Object*>& objects_): objects(objects_) {}

  // build the BVH
  void build();

  // Return intersect or not; the intersection object is in `obj`,
  // travel distance of ray is in `t`
  bool intersect(const Ray& ray, Object* obj, double& t) const;
};
