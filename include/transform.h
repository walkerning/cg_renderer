// Basic image transform
#pragma once
#include "vec3.h"

void image_scale(Vec3* im, int height, int width, Vec3 scale) {
  int len = height * width;
  for (int i = 0; i < len; i++) {
    im[i] = im[i] * scale;
  }
}

double _gamma(double in) {
  return pow(MAX(MIN(in, 1.), 0.), 1./2.2);
}

void image_gamma(Vec3* im, int height, int width) {
  int len = height * width;
  for (int i = 0; i < len; i++) {
    im[i].x = _gamma(im[i].x);
    im[i].y = _gamma(im[i].y);
    im[i].z = _gamma(im[i].z);
  }
}
