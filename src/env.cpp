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
    fprintf(stderr, "[load object] added mesh! triangles number: %d\n", dynamic_cast<MeshObject*>(obj)->triangles.size());
  } else {
    objects.push_back(obj);
  }
}

void Environment::build_bvh() {
  std::cerr << "INFO: <Environment> building BVH ... objects number: " << objects.size() << std::endl;
  if (bvh != NULL) {
    delete bvh;
    bvh = NULL;
  }
  bvh = new BVH(objects);
  bvh->build();
  std::cerr <<"INFO: <Environment> building BVH finished ..." << std::endl;
}

void Environment::init_test_env() {
  //light = new RadPointLight(Vec3(20, 39, 20), Vec3(1000, 3800, 2000));
  light = new RadPointLight(Vec3(20, 1, 20), Vec3(2000, 2000, 2000));
  // camera = new Camera(Vec3(20, 20, 1), Vec3(0, 0, 1));
  camera = new Camera(Vec3(20, 20, -20), Vec3(0, 0, 1));
  // front
  //add_wall(Vec3(40, 40, 40), Vec3(40, 0, 40), Vec3(0, 0, 40), Vec3(0, 40, 40), new Diffuse(Vec3(0.5, 0.5, 1)));
  add_wall(Vec3(40, 40, 40), Vec3(40, 0, 40), Vec3(0, 0, 40), Vec3(0, 40, 40), new Diffuse(Vec3(0, 0, 1)));
  // ceil
  //add_wall(Vec3(40, 0, 40), Vec3(40, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 40), new Diffuse(Vec3(1, 0.2, 0.1)));
  add_wall(Vec3(40, 0, 40), Vec3(40, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 40), new Diffuse(Vec3(1, 0, 0.1)));
  // floor
  //add_wall(Vec3(40, 40, 0), Vec3(40, 40, 40), Vec3(0, 40, 40), Vec3(0, 40, 0), new Diffuse(Vec3(0.2, 0.1, 0.1)));
  add_wall(Vec3(40, 40, 0), Vec3(40, 40, 40), Vec3(0, 40, 40), Vec3(0, 40, 0), new Diffuse(Vec3(0.2, 0.1, 0.1)));
  // right
  add_wall(Vec3(40, 40, 0), Vec3(40, 0, 0), Vec3(40, 0, 40), Vec3(40, 40, 40), new Reflection(Vec3(0.7, 0.7, 0.7)));
  //add_wall(Vec3(40, 40, 0), Vec3(40, 0, 0), Vec3(40, 0, 40), Vec3(40, 40, 40), new Diffuse(Vec3(0.9, 0.9, 0.9)));
  // left
  //add_wall(Vec3(0, 40, 0), Vec3(0, 40, 40), Vec3(0, 0, 40), Vec3(0, 0, 0), new Diffuse(Vec3(0.5, 0.5, 0.5)));
  add_wall(Vec3(0, 40, 0), Vec3(0, 40, 40), Vec3(0, 0, 40), Vec3(0, 0, 0), new Diffuse(Vec3(0, 1, 0)));

  // objects
  // add_object(new Sphere(Vec3(20, 20, 30), 5,
  //                       new Diffuse(Vec3(0.3, 0.1, 0.9))));
  // add_object(new Sphere(Vec3(20, 20, 30), 5,
  // 			new Refraction(1.5,
  // 				       Vec3(0.8, 0.8, 0.8),
  // 				       Vec3(0.5, 0.5, 0.5))));
  // add_object(new Sphere(Vec3(30, 20, 30), 5,
  // 			new Refraction(1.5,
  // 				       Vec3(0.3, 0.3, 0.3),
  // 				       Vec3(0.6, 0.6, 0.6))));
    
  // add_object(new Sphere(Vec3(10, 30, 10), 8,
  //  			new Reflection(Vec3(0.95, 0.95, 0.95))));
  add_object(new Sphere(Vec3(12, 31, 30), 6,
			new Reflection(Vec3(0.95, 0.95, 0.95))));
  add_object(new Sphere(Vec3(28, 33, 20), 5,
			new Reflection(Vec3(0.98, 0.98, 0.98))));
  // add_object(new MeshObject(Vec3(20, 20, 25), "./models/bunny.obj", new Diffuse(Vec3(0.8, 0.8, 0.8)), 55, Vec3(1, -1, -1)));
  // add_object(new MeshObject(Vec3(20, 20, 25), "./models/cube.obj", new Diffuse(Vec3(0.8, 0.8, 0.8)), 5));
  // walls
  // // front
  // add_wall(Vec3(40, 40, 40), Vec3(40, 0, 40), Vec3(0, 0, 40), Vec3(0, 40, 40), new Diffuse(Vec3(0.5, 0.5, 1)));
  // // // back
  // // add_wall(Vec3(40, 0, 0), Vec3(40, 40, 0), Vec3(0, 40, 0), Vec3(0, 0, 0), new Diffuse(Vec3(0.5, 0.5, 0.5)));
  // // left
  // //add_wall(Vec3(40, 0, 40), Vec3(40, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 40), new Reflection(Vec3(0.95, 0.95, 0.95)));
  // //add_wall(Vec3(40, 0, 40), Vec3(40, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 40), new Reflection(Vec3(1, 1, 1)));
  // add_wall(Vec3(40, 0, 40), Vec3(40, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 40), new Diffuse(Vec3(0.8, 0.2, 0.1)));
  // // right
  // add_wall(Vec3(40, 40, 0), Vec3(40, 40, 40), Vec3(0, 40, 40), Vec3(0, 40, 0), new Diffuse(Vec3(0.2, 0.1, 0.1)));
  // // right
  // add_wall(Vec3(40, 40, 0), Vec3(40, 0, 0), Vec3(40, 0, 40), Vec3(40, 40, 40), new Diffuse(Vec3(0.5, 0.5, 0.5)));
  // // left
  // add_wall(Vec3(0, 40, 0), Vec3(0, 40, 40), Vec3(0, 0, 40), Vec3(0, 0, 0), new Diffuse(Vec3(0.5, 0.5, 0.5)));

  // // objects
  // add_object(new Sphere(Vec3(20, 20, 30), 5,
  //                       new Diffuse(Vec3(0.3, 0.1, 0.9))));
                        //new Refraction(1.5,
  //Vec3(0.8, 0.8, 0.8),
  //Vec3(0.5, 0.5, 0.5))));

  //add_wall(Vec3(2, 1, 1), Vec3(2, -1, 1), Vec3(0, -1, 1), Vec3(0, 1, 1), new Diffuse(Vec3(0.5, 0.5, 0.5))); // front
  // add_wall(Vec3(2, -1, -1), Vec3(2, 1, -1), Vec3(0, 1, -1), Vec3(0, -1, -1), new Diffuse(Vec3(0.725, 0.71, 0.68))); // back
  // add_wall(Vec3(2, -1, 1), Vec3(2, -1, -1), Vec3(0, -1, -1), Vec3(0, -1, 1), new Diffuse(Vec3(0.63, 0.065, 0.05))); // left
  // add_wall(Vec3(2, 1, -1), Vec3(2, 1, 1), Vec3(0, 1, 1), Vec3(0, 1, -1), new Diffuse(Vec3(0.14, 0.45, 0.091))); // right
  // add_wall(Vec3(2, 1, -1), Vec3(2, -1, -1), Vec3(2, -1, 1), Vec3(2, 1, 1), new Diffuse(Vec3(0.725, 0.71, 0.68))); // ceil
  // add_wall(Vec3(0, 1, -1), Vec3(0, -1, 1), Vec3(0, -1, -1), Vec3(0, 1, -1), new Diffuse(Vec3(0.725, 0.71, 0.68))); // floor
  // camera = new Camera(Vec3(1, 0, 6.838), Vec3(0, 0, -1));
  // light = new RadPointLight(Vec3(1, 0.998, 0));
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
