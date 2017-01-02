#pragma once
#include <vector>
#include "vec3.h"
#include "rand.h"

using std::vector;

struct Path {
  int next_index;
  vector<double> rand_values;

  Path(): next_index(0) {}

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
  double mutate(double mutation_size) {

  }
};
