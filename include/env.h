#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
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
  Environment(): light(NULL), camera(NULL), bvh(NULL) {};

  // handle rectangles specially
  void add_wall(Vec3 a, Vec3 b, Vec3 c, Vec3 d, BRDF* brdf) {
    add_object(new Triangle(a, b, c, brdf));
    add_object(new Triangle(a, c, d, brdf));
  }

  void add_object(Object* obj) {
    // need a virtual object_type? for BVH adding?
    if (strcmp(obj->type(), "Mesh") == 0) {
      for (auto& tri : obj->triangles) {
        add_object(&tri);
      }
    } else {
      objects.push_back(obj);
    }
  }

  void build_bvh() {
    if (bvh) {
      delete bvh;
    }
    bvh = new BVH(objects);
    bvh.build();
  }

  // Construct a typical test environment.
  void init_test_env();

  // Construct environment from a scene config file.
  void init_env_from_file(std::string);

  // Renderer can use these functions.
  inline bool intersect(const Ray& ray, Object* &obj, double& t) const {
    if (!bvh) {
      // std::cerr << "Error: in `Environment::intersect`: BVH not build yet!" << std::endl;
      std::cerr << "Info: <Environment::intersect>: building BVH ..." << std::endl;
      build_bvh();
      // return false;
    }
    return bvh->intersect(ray, obj, t);
  }

  ~Environment() {
    // TODO: delete all malloced memory
    for (auto op : objects) {
      delete op;
    }
  }
};
