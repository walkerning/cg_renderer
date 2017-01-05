#include <cstdio>
#include <cmath>
#include <cassert>
#include "bvh.h"
#include "object.h"
#include "brdf.h"

double eps = 1e-3;
int main() {
  vector<Object*> objs;
  Object* obj = new Sphere(Vec3(0, 10, 0), 5, new Diffuse(Vec3(0.5, 0.5, 0.5)));
  objs.push_back(obj);
  objs.push_back(new Triangle(Vec3(5, 0, 0), Vec3(5, 0, 5), Vec3(5, 4, 5), new Diffuse(Vec3(0.5, 0.5, 0.5))));

  BVH bvh(objs);
  bvh.build();
  printf("bvh root split_dim: %d, split_coord: %lf\n", bvh.root->split_dim,
         bvh.root->split_coord);
  assert(bvh.root->split_dim == 1 && bvh.root->split_coord == 7.5);
  assert(bvh.root->left->offset == 0 && bvh.build_objs[bvh.root->left->offset] == objs[1]);
  assert(bvh.root->right->offset == 1 && bvh.build_objs[bvh.root->right->offset] == objs[0]);

  Ray ray(Vec3(), Vec3(0, 1, 0));
  Object* obj_intersected;
  double t;
  assert(!bvh.root->left->bbox.intersect(ray, t));
  BBox bbox = bvh.root->right->bbox;
  bbox.print();
  // printf("(%lf, %lf, %lf); (%lf, %lf, %lf)\n", bbox.min[0], bbox.min[1], bbox.min[2],
  //        bbox.max[0], bbox.max[1], bbox.max[2]);
  assert(bbox.intersect(ray, t));
  bool intersected = bvh.intersect(ray, obj_intersected, t);
  assert (intersected && t == 5 && obj_intersected == objs[0]);

  ray = Ray(Vec3(), Vec3(1, 1, 1).normalize());
  intersected = bvh.intersect(ray, obj_intersected, t);
  assert (!intersected);

  ray = Ray(Vec3(), Vec3(5, 1, 2).normalize());
  intersected = bvh.intersect(ray, obj_intersected, t);
  assert (intersected && obj_intersected == objs[1] && std::abs(t - 5.47722) < eps);

  ray = Ray(Vec3(5, 5, 5), Vec3(-1, 1, -1).normalize());
  intersected = bvh.intersect(ray, obj_intersected, t);
  assert (intersected);
  assert (obj_intersected == objs[0]);
  assert (std::abs(t - 3.66025) < eps);
  return 0;
}
