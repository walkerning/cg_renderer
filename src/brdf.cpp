#include "brdf.h"

BRDF* BRDF::get(Path& path) { return this; }
void BRDF:: apply_absorption(Ray& ray, Vec3 start, Vec3 end, Vec3 normal) {}

Reflection::Reflection(Vec3 decay_): decay(decay_) {}
bool Reflection::is_specular() { return true; }
Ray Reflection::sample_ray(Path& path, Ray ray_in, Vec3 normal, Vec3 intersection) {
  Vec3 dir_in = ray_in.dir;
  // reflect direction
  Vec3 dir_out = (dir_in - (2 * dir_in.dot(normal))).normalize();
  Ray ray_out(intersection, dir_out, ray_in.flux * decay);
  ray_out.flux = ray_in.flux * decay;
  return ray_out;
}

// Refraction
Refraction::Refraction(double refract_index_,
                       Vec3 reflect_decay_,
                       Vec3 refract_decay_,
                       double absorption_decay_): refract_index(refract_index_),
                                                    reflect_decay(reflect_decay_),
                                                    refract_decay(refract_decay_),
                                                    absorption_decay(absorption_decay_) {}
bool Refraction::is_specular() { return true; }
void Refraction:: apply_absorption(Ray& ray, Vec3 start, Vec3 end, Vec3 normal) {
  if (ray.dir.dot(normal) < 0) {
    return;
  }
  double dist = (end - start).norm();
  ray.flux.x *= exp(-dist * absorption_decay[0]);
  ray.flux.y *= exp(-dist * absorption_decay[1]);
  ray.flux.z *= exp(-dist * absorption_decay[2]);
}
Ray Refraction::sample_ray(Path& path, Ray ray_in, Vec3 normal, Vec3 intersection) {
  Vec3 dir_in = ray_in.dir; // .normalize()
  bool into = (cos < 0);
  double cos = dir_in.dot(normal);
  double sin_in = cathetus(1, cos);
  double index_ratio = into? refract_index : 1/refract_index;
  double sin_out = sin_in / index_ratio;
  Vec3 reflect_out = (dir_in - (2 * dir_in.dot(normal))).normalize();

  if (sin_out > 1) { // full reflection
    // FIXME: 从里向外全反射时, 应该有衰减还是没有?
    return Ray(intersection, reflect_out, ray_in.flux * Vec3(1, 1, 1));
  } else {
    // Decide refract or reflect.
    double select = path.next_value();
    // Calculate refraction probability of this incidence angley
    // https://en.wikipedia.org/wiki/Fresnel_equations
    double cos_out = cathetus(1, sin_out);
    double cos_in = -cos;
    // s-极化分量反射比
    double R_s = ((cos_in - index_ratio * cos_out)/(cos_in + index_ratio * cos_out));
    // p-极化分量反射比
    double R_p = ((cos_out - index_ratio * cos_in)/(cos_out + index_ratio * cos_in));
    // equal mix of s and p polarisations
    double prob_reflect = (R_s * R_s + R_p * R_p) / 2;
    if (select < prob_reflect) {
      return Ray(intersection, reflect_out, ray_in.flux * reflect_decay);
    }
    // Cal: normal direction: -N*cos_out; tangent direction: (dir_in + normal * cos_in) / index_ratio
    Vec3 refract_out = (dir_in / index_ratio + normal * (cos_in / index_ratio - cos_out)).normalize();
    return Ray(intersection, refract_out, ray_in.flux * refract_decay);
  }
}
// Diffuse
Diffuse::Diffuse(Vec3 decay_): decay(decay_) {}
bool Diffuse::is_specular() { return false; }
Ray Diffuse::sample_ray(Path& path, Ray ray_in, Vec3 normal, Vec3 intersection) {
  // If path is already sampled, follow the already-sampled path
  Vec3 dir_out = path_to_dir(path);
  if (dir_out.dot(normal) < 0) {
    dir_out = dir_out * (-1);
  }
  return Ray(intersection, dir_out, ray_in.flux * (decay / M_PI));
}

// Mixed
bool Mixed::is_specular() { return false; }
Mixed::Mixed(const vector<pair<BRDF*, double> >& brdfs_) {
  prob_sum = 0;
  for (const auto it : brdfs_) {
    prob_sum += it.second;
    brdfs.push_back(it);
  }
}

BRDF* Mixed::get(Path& path) {
  double select = path.next_value() * prob_sum;
  for (const auto& it : brdfs) {
    if (select < it.second) {
      return it.first->get(path); // for nested Mixed BRDF
    }
    select -= it.second;
  }
  // control should not reach here. numerical problem?
  return brdfs.back().first;
}

Ray Mixed::sample_ray(Path& path, Ray ray_in, Vec3 normal, Vec3 intersection) {
  return get(path)->sample_ray(path, ray_in, normal, intersection);
}
