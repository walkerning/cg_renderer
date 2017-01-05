#include "env.h"
#include <cstdio> // dbg

Environment::Environment(): light(NULL), camera(NULL), bvh(NULL) {}

void Environment::add_wall(Vec3 a, Vec3 b, Vec3 c, Vec3 d, BRDF* brdf) {
  add_object(new Triangle(a, b, c, brdf));
  add_object(new Triangle(a, c, d, brdf));
}

void Environment::add_object(Object* obj) {
  // need a virtual object_type? for BVH adding?
  if (strcmp(obj->type(), "Mesh") == 0) {
    for (auto tri : dynamic_cast<MeshObject*>(obj)->triangles) {
      add_object(tri);
    }
  } else {
    objects.push_back(obj);
  }
}

void Environment::build_bvh() {
  std::cerr << "INFO: <Environment> building BVH ..." << std::endl;
  if (bvh != NULL) {
    delete bvh;
    bvh = NULL;
  }
  bvh = new BVH(objects);
  bvh->build();
  std::cerr <<"INFO: <Environment> building BVH finished ..." << std::endl;
}

void Environment::init_test_env() {
  light = new RadPointLight(Vec3(39, 20, 20));
  camera = new Camera(Vec3(20, 20, 1), Vec3(0, 0, 1));

  // walls
  // front
  add_wall(Vec3(40, 40, 40), Vec3(40, 0, 40), Vec3(0, 0, 40), Vec3(0, 40, 40), new Diffuse(Vec3(0.5, 0.5, 0.5)));
  // back
  add_wall(Vec3(40, 0, 0), Vec3(40, 40, 0), Vec3(0, 40, 0), Vec3(0, 0, 0), new Diffuse(Vec3(0.5, 0.5, 0.5)));
  // left
  add_wall(Vec3(40, 0, 40), Vec3(40, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 40), new Reflection(Vec3(0.95, 0.95, 0.95)));
  // right
  add_wall(Vec3(40, 40, 0), Vec3(40, 40, 40), Vec3(0, 40, 40), Vec3(0, 40, 0), new Diffuse(Vec3(0.5, 0.5, 0.5)));
  // ceil
  add_wall(Vec3(40, 40, 0), Vec3(40, 0, 0), Vec3(40, 0, 40), Vec3(40, 40, 40), new Diffuse(Vec3(0.5, 0.5, 0.5)));
  // floor
  add_wall(Vec3(0, 40, 0), Vec3(0, 0, 40), Vec3(0, 0, 0), Vec3(0, 40, 0), new Diffuse(Vec3(0.5, 0.5, 0.5)));

  // objects
  add_object(new Sphere(Vec3(20, 20, 20), 10,
                        new Diffuse(Vec3(0.5, 0.5, 0.5))));
                        //new Refraction(1.5,
  //Vec3(0.8, 0.8, 0.8),
  //Vec3(0.5, 0.5, 0.5))));
}

void Environment::init_env_from_file(std::string) {

}

Environment::~Environment() {
  // TODO: delete all malloced memory
  for (auto op : objects) {
    delete op;
  }
  if (bvh != NULL) {
    delete bvh;
  }
}
