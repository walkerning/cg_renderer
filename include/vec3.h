#pragma once
#include <cmath>

struct Vec3 {
  double x, y, z;
  Vec3(double x_=0, double y_=0, double z_=0):x(x_), y(y_), z(z_) {}
  Vec3 operator+(const Vec3 &other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
  Vec3 operator-(const Vec3 &other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
  Vec3 operator*(const Vec3 &other) const { return Vec3(x * other.x, y * other.y, z * other.z); }

  double dot(const Vec3 &other) const { return x * other.x + y * other.y + z * other.z; }

  Vec3 operator*(double scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
  Vec3 operator/(double scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }
  Vec3& norm(){ return *this = *this * (1 / sqrt(x * x + y * y + z * z)); }
};

struct Ray {
  Vec3 ori;
  Vec3 dir;
  Vec3 flux; // Unnormalized flux
  Ray(Vec3 ori_, Vec3 dir_, Vec3 flux_=Vec3(1, 1, 1)): ori(ori_), dir(dir_), flux(flux_) {}
};

double cathetus(double r, double x) {
  if (r <= x) {
    return 0;
  }
  return sqrt(r * r - x * x);
}

// from path, generate uniform distributed sphere direction
Vec3 path_to_dir(Path& path) {
  double z = path.next() * 2 - 1;
  double r = cathetus(1, z);
  double theta = path.next() * M_PI * 2;
  return Vec3(r * sin(theta), r * cos(theta), z);
}
