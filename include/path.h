#pragma once
#include <vector>
#include "vec3.h"
#include "utils/rand.h"

using std::vector;

struct Path {
  int next_index;
  vector<double> rand_values;

  Path(): next_index(0) {}

  Path(const Path& other): rand_values(other.rand_values) {}

  inline void reset() {
    next_index = 0;
  }

  // lazy path evaluation
  double next_value();

  // apply mutation
  Path mutate(double mutation_size);
};

// from path, generate uniform distributed sphere direction
Vec3 path_to_dir(Path& path);
