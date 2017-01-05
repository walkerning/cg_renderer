#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include "bvh.h"
#include "vec3.h"
#include "light.h"
#include "object.h"

struct Environment {
  // objects in the environment: objects, light, camera
  std::vector<Object*> objects;
  Light* light;
  Camera* camera;

  // BVH for faster intersection.
  BVH* bvh;

  // *****************
  // **** Methods ****
  // *****************
  Environment();

  // handle rectangles specially
  void add_wall(Vec3 a, Vec3 b, Vec3 c, Vec3 d, BRDF* brdf);

  void add_object(Object* obj);

  void build_bvh();

  // Construct a typical test environment.
  void init_test_env();

  // Construct environment from a scene config file.
  void init_env_from_file(std::string);

  // Renderer can use these functions.
  inline bool intersect(const Ray& ray, Object* &obj, double& t) {
    if (!bvh) {
      // std::cerr << "Error: in `Environment::intersect`: BVH not build yet!" << std::endl;
      build_bvh();
      // return false;
    }
    return bvh->intersect(ray, obj, t);
  }

  ~Environment();
};
