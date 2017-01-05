#pragma once
#include <cmath>

#define MIN(x, y) (x) >= (y)? (y) : (x)
#define MAX(x, y) (x) >= (y)? (x) : (y)

struct Vec3 {
  double x;
  double y;
  double z;

  Vec3(double x_=0, double y_=0, double z_=0):x(x_), y(y_), z(z_) {}

  Vec3 operator+(const Vec3 &other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
  Vec3 operator-(const Vec3 &other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
  Vec3 operator*(const Vec3 &other) const { return Vec3(x * other.x, y * other.y, z * other.z); }
  Vec3 operator%(const Vec3 &other) const { return Vec3(y * other.z - z * other.y,
                                                        z * other.x - x * other.z,
                                                        x * other.y - y * other.x); } // cross product

  double dot(const Vec3 &other) const { return x * other.x + y * other.y + z * other.z; }

  // broardcast scalar to 3 channel
  Vec3 operator+(double scalar) const { return Vec3(x + scalar, y + scalar, z + scalar); }
  Vec3 operator-(double scalar) const { return Vec3(x - scalar, y - scalar, z - scalar); }
  Vec3 operator*(double scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
  Vec3 operator/(double scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }

  double max() const { return MAX(MAX(x, y), z); }
  double norm() const { return sqrt(x * x + y * y + z * z); }
  Vec3& normalize() { return *this = *this * (1 / norm()); }

  double operator[](int index) const { return *(&x + index); }
};

inline Vec3 min_(Vec3 a, Vec3 b) {
  return Vec3(MIN(a.x, b.x), MIN(a.y, b.y), MIN(a.z, b.z));
}

inline Vec3 max_(Vec3 a, Vec3 b) {
  return Vec3(MAX(a.x, b.x), MAX(a.y, b.y), MAX(a.z, b.z));
}

struct Ray {
  Vec3 ori;
  Vec3 dir;
  Vec3 flux; // Unnormalized flux
  Ray(Vec3 ori_, Vec3 dir_, Vec3 flux_=Vec3(1, 1, 1)): ori(ori_), dir(dir_), flux(flux_) {}
};

typedef Ray Camera;

inline double cathetus(double r, double x) {
  if (r <= x) {
    return 0;
  }
  return sqrt(r * r - x * x);
}

