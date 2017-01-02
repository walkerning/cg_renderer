#pragma once
#include <random>
#include <tr1/memory>

class RNG {
  static std::shared_ptr<RNG> instance_p;
  std::shared_ptr<std::mt19937> mt_p
;
private:
  RNG() {
    mt_p.reset(new std::mt19937(std::random_device{}()));
  }

  std::uniform_real_distribution<> uniform_dist(0, 1);
  std::normal_distribution<> normal_dist(0, 1);

public:
  static RNG& get() {
    if (!instance_p.get()) {
      instance_p.reset(new RNG());
    }
    return *(instance_p.get());
  }

  static void set_random_seed(const unsigned int seed) {
    // set RNG seed. for debug use
    get().mt_p.reset(new std::mt19937(seed));
  }

  // random sample methods
  double uniform_rand() {
    return uniform_dist(*(mt_p.get()));
  }

  double normal_rand() {
    return normal_dist(*(mt_p.get()));
  }
};

double uniform_rand() {
  return RNG::get().uniform_rand();
}

double normal_rand() {
  return RNG::get().normal_rand();
}

