#pragma once
#include <vector>
#include "vec3.h"
#include "rand.h"

using std::vector;

struct Path {
  int next_index;
  vector<double> rand_values;

  Path(): next_index(0) {}

  Path(const Path& other): rand_values(other.rand_values) {}

  // lazy path evaluation
  double next_value() {
    int sz = static_cast<int>(rand_values.size());
    if (sz <= next_index) {
      for (int i = 0; i < next_index - sz; i++) {
        rand_values.push_back(uniform_rand());
      }
    }
    return rand_values[next_index++];
  }

  // apply mutation
  Path mutate(double mutation_size) {
    Path new_path = path;
    for (auto &value : new_path.values) {
      double e1 = uniform_rand();
      double e2 = uniform_rand();
      value += pow(e2, 1. / mutation_size + 1) * ((e1 > 0.5)? 1: -1);
      // wrapping around
      if (value > 1) {
        value -= 1;
      } else if (value < 0) {
        value += 1;
      }
    }
    return
  }
};

// from path, generate uniform distributed sphere direction
Vec3 path_to_dir(Path& path) {
  double z = path.next() * 2 - 1;
  double r = cathetus(1, z);
  double theta = path.next() * M_PI * 2;
  return Vec3(r * sin(theta), r * cos(theta), z);
}
