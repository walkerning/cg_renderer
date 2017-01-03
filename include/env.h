#pragma once
#include <iostream>
#include <string>
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

  void add_object(Object* obj) {
    // need a virtual object_type? for BVH adding?
    switch (obj->type()) {
    case "Mesh": {
      for (auto& tri : obj->triangles) {
        add_object(&tri);
      }
      break;
    }
    default:
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
  inline bool intersect(const Ray& ray, Object* obj, double& t) const {
    if (!bvh) {
      std::cerr << "Error: in `Environment::intersect`: BVH not build yet!" << std::endl;
      return false;
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
