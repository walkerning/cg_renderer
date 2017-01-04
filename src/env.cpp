#include "env.h"

void Environment::init_test_env() {
  light = new RadPointLight(Vec3(40, 20, 20));
  camera = new Camera(Vec3(20, 20, 0), Vec3(0, 0, 1));

  // walls
  // front
  add_wall(Vec3(40, 40, 40), Vec3(40, 0, 40), Vec3(0, 0, 40), Vec3(0, 40, 40), new Diffuse(Vec3(0.5, 0.5, 0.5)));
  // left
  add_wall(Vec3(40, 0, 40), Vec3(40, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 40), new Reflection(0.95, 0.95, 0.95));
  // right
  add_wall(Vec3(40, 40, 0), Vec3(40, 40, 40), Vec3(0, 40, 40), Vec3(0, 40, 0), new Diffuse(Vec3(0.5, 0.5, 0.5)));
  // ceil
  add_wall(Vec3(40, 40, 0), Vec3(40, 0, 0), Vec3(40, 0, 40), Vec3(40, 40, 40), new Diffuse(Vec3(0.5, 0.5, 0.5)));
  // floor
  add_wall(Vec3(0, 40, 0), Vec3(0, 0, 40), Vec3(0, 0, 0), Vec3(0, 40, 0), new Diffuse(Vec3(0.5, 0.5, 0.5)));

  // objects
  add_object(new Sphere(Vec3(20, 20, 20), 5,
                        new Refraction(1.5,
                                       Vec3(0.8, 0.8, 0.8),
                                       Vec3(0.5, 0.5, 0.5))));
}

void Environment::init_env_from_file(std::string) {

}
