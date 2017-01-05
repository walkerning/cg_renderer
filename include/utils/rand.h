#pragma once
#include <random>

class RNG {
  static RNG* instance_p;
  std::mt19937* mt_p;

private:
  RNG() {
    mt_p = new std::mt19937(std::random_device()());
    uniform_dist = new std::uniform_real_distribution<>(0, 1);
    normal_dist = new std::normal_distribution<>(0, 1);
  }

  std::uniform_real_distribution<>* uniform_dist;
  std::normal_distribution<>* normal_dist;

public:
  static RNG& get() {
    if (!instance_p) {
      instance_p = new RNG();
    }
    return *(instance_p);
  }

  inline void reset_random_seed(const unsigned int seed) {
    delete mt_p;
    mt_p = new std::mt19937(seed);
  }

  inline static void set_random_seed(const unsigned int seed) {
    // set RNG seed. for debug use
    get().reset_random_seed(seed);
  }

  // random sample methods
  inline double uniform_rand() {
    return (*uniform_dist)(*mt_p);
  }

  inline double normal_rand() {
    return (*normal_dist)(*mt_p);
  }
};

inline double uniform_rand() {
  return RNG::get().uniform_rand();
}

inline double normal_rand() {
  return RNG::get().normal_rand();
}


