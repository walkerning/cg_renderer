#pragma once
#include <utility>
#include "vec3.h"
#include "path.h"

using std::pair;

class BRDF {
  virtual bool is_specular() { return false; } // stub for mixed BRDF

  // A indirect get layer for mixed BRDF. Random select a BRDF that is working.
  virtual BRDF* get(Path& path) { return this; }

  // FIXME: in fact intersection is unrelated to BRDF calculation... should it be moved out?
  virtual Ray sample_ray(Path& path, Ray ray_in, Vec3 normal, Vec3 intersection) = 0;
};

class Reflection: BRDF {
  Vec3 decay;

  Reflection(Vec3 decay_): decay(decay_) {}

  virtual bool is_specular() { return true; }

  virtual Ray sample_ray(Path& path, Ray ray_in, Vec3 normal, Vec3 intersection) {
    Vec3 dir_in = ray_in.dir;
    // reflect direction
    Vec3 dir_out = (dir_in - (2 * dir_in.dot(normal))).normalize();
    Ray ray_out(intersection, dir_out);
    ray_out.flux = ray_in.flux * decay;
    return ray_out;
  }
};

class Refraction: BRDF {
  double refract_index; // 折射率
  Vec3 reflect_decay; // decay factor for different color
  Vec3 refract_decay;

  Refraction(double refract_index_,
             Vec3 reflect_decay_=Vec3(1, 1, 1),
             Vec3 refract_decay_=Vec3(1, 1, 1)): refract_index(refract_index_),
                                                 reflect_decay(reflect_decay_),
                                                 refract_decay_(refract_decay_) {}

  virtual bool is_specular() { return true; }

  virtual Ray sample_ray(Path& path, Ray ray_in, Vec3 normal, Vec3 intersection) {
    Vec3 dir_in = ray_in.dir; // .normalize()
    bool into = (cos < 0);
    double cos = dir_in.dot(normal);
    double sin_in = cathetus(1, cos);
    double index_ratio = into? refract_index : 1/refract_index;
    double sin_out = sin_in / index_ratio;
    Vec3 reflect_out = (dir_in - (2 * dir_in.dot(normal))).normalize();

    if (sin_out > 1) { // full reflection
      // FIXME: 从里向外全反射时, 应该有衰减还是没有?
      return Ray(intersection, reflect_out, Vec3(1, 1, 1));
    } else {
      // Decide refract or reflect.
      double select = path.next();
      // Calculate refraction probability of this incidence angley
      // https://en.wikipedia.org/wiki/Fresnel_equations
      double cos_out = cathetus(1, sin_out);
      double cos_in = -cos;
      // s-极化分量反射比
      double R_s = ((cos_in - index_ratio * cos_out)/(cos_in + index_ratio * cos_out))^2;
      // p-极化分量反射比
      double R_p = ((cos_out - index_ratio * cos_in)/(cos_out + index_ratio * cos_in))^2;
      // equal mix of s and p polarisations
      double prob_reflect = (R_s + R_p) / 2;
      if (select < prob_reflect) {
        return Ray(intersection, reflect_out, reflect_decay);
      }
      // Cal: normal direction: -N*cos_out; tangent direction: (dir_in + normal * cos_in) / index_ratio
      Vec3 refract_out = (dir_in / index_ratio + normal * (cos_in / index_ratio - cos_out)).normalize();
      return Ray(intersection, refract_out, refract_decay);
    }
  }
};

class Diffuse: BRDF {
  Vec3 decay;

  virtual bool is_specular() { return false; }

  virtual Ray sample_ray(Path& path, Ray ray_in, Vec3 normal, Vec3 intersection) {
    // If path is already sampled, follow the already-sampled path
    Vec3 dir_out = path_to_dir(path);
    if (dir_out.dot(normal) < 0) {
      // 这个处理全部都对吗?
      dir_out = dir_out * (-1);
    }
    return Ray(intersection, dir_out, decay / pi);
  }
};

class Mixed: BRDF {
  vector<pair<BRDF*, double> > brdfs;
  double prob_sum;

  Mixed(const vector<pair<BRDF*, double> >& brdfs_) {
    prob_sum = 0;
    for (const auto it : brdfs_) {
      prob_sum += it.second;
      brdfs.push_back(it);
    }
  }

  virtual BRDF* get(Path& path) {
    double select = path.next_value() * prob_sum;
    for (const auto& it : brdfs) {
      if (select < it.second) {
        return it.first->get(path); // for nested Mixed BRDF
      }
      select -= it.second;
    }
    // control should not reach here. numerical problem?
    return brdfs_.back()
  }
  virtual Ray sample_ray(Path& path, Ray ray_in, Vec3 normal) {
    return get(path)->sample_ray(path, ray_in, normal);
  }
};
