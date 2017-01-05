#include <iostream>
#include <string>
#include <cstdlib>
#include "vec3.h"
#include "renderer.h"
#include "env.h"

using namespace std;

// TODO: scene config, is there any widely-used scene config file format?
// or we can make a simple one

void print_help() {
  cerr << "usage: ./main <render_config> [, <scene config>]" << endl;
}

int main(int argc, const char** argv) {
  if (argc < 2) {
    print_help();
    exit(1);
  }
  Environment env;
  if (argc == 3) {
    env.init_env_from_file(string(argv[2]));
  } else {
    env.init_test_env();
  }
  env.build_bvh();

  Renderer* renderer = Renderer::get_renderer(string(argv[1]));
  renderer->set_env(&env);
  renderer->render();
  return 0;
}
