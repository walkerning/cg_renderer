#include "path.h"

Vec3 path_to_dir(Path& path) {
  double z = path.next_value() * 2 - 1;
  double r = cathetus(1, z);
  double theta = path.next_value() * M_PI * 2;
  return Vec3(r * sin(theta), r * cos(theta), z);
}

double Path::next_value() {
  int sz = static_cast<int>(rand_values.size());
  if (sz <= next_index) {
    for (int i = 0; i <= next_index - sz; i++) {
      rand_values.push_back(uniform_rand());
    }
  }
  return rand_values[next_index++];
}

Path Path::mutate(double mutation_size) {
  Path new_path(*this);
  for (auto &value : new_path.rand_values) {
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
  return new_path;
}
