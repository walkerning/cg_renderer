#include <cstdio>
#include <cmath>
#include <cassert>
#include "object.h"
#include "brdf.h"

double eps = 1e-3;
int main() {
  Object* obj = new Sphere(Vec3(0, 10, 0), 5, new Diffuse(Vec3(0.5, 0.5, 0.5)));
  Ray ray(Vec3(), Vec3(0, 1, 0));
  double t;
  bool intersected = obj->intersect(ray, t);
  assert (intersected && t == 5);

  ray = Ray(Vec3(), Vec3(1, 1, 0).normalize());
  intersected = obj->intersect(ray, t);
  assert (!intersected);

  obj = new Triangle(Vec3(0, 5, 0), Vec3(5, 5, 0), Vec3(0, 5, 5), new Diffuse(Vec3(0.5, 0.5, 0.5)));
  ray = Ray(Vec3(), Vec3(2, 5, 1).normalize());
  intersected = obj->intersect(ray, t);
  assert (intersected && std::abs(t - 5.47722) < eps);
  BBox bbox = obj->get_bbox();
  assert(bbox.intersect(ray, t));
  //bbox.print();
  return 0;
}
