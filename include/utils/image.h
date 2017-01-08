#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <unordered_map>
#include "utils/transform.h"

enum ImageType {
  BMP = 1
};

std::unordered_map<std::string, ImageType> image_type_map = {{"bmp", BMP}};

void write_bmp(Vec3* im, int height, int width, std::string fname);

void write_image(Vec3* im, int height, int width, double scale, std::string fname, ImageType type) {
  // copy to im_write
  int len = height * width;
  Vec3* im_write = new Vec3[len];
  double sum = 0;
  for (int i = 0; i < len; i++) {
    im_write[i] = im[i];
    sum += im_write[i].x + im_write[i].y + im_write[i].z;
  }

  // try auto exposure
  scale = 3 * 0.18 * len / sum;
  // get normalized radiance
  image_scale(im_write, height, width, Vec3(scale, scale, scale));
  // gamma correction
  // image_gamma(im_write, height, width);

  switch (type) {
  case BMP:
    write_bmp(im_write, height, width, fname);
    break;
  }

  delete[] im_write;
}

void write_image(Vec3* im, int height, int width, double scale, std::string fname, std::string type) {
  auto got = image_type_map.find(type);
  if (got == image_type_map.end()) {
    std::cerr << "Error: <image> Unknown image type " << type << std::endl;
    return;
  }
  write_image(im, height, width, scale, fname, got->second);
}

// code copied from cobra.cpp
void write_bmp(Vec3* im, int height, int width, std::string fname) {
#define INT2CHAR_BIT(num, bit) (unsigned char)(((num) >> (bit)) & 0xff)
#define INT2CHAR(num) INT2CHAR_BIT((num),0), INT2CHAR_BIT((num),8), INT2CHAR_BIT((num),16), INT2CHAR_BIT((num),24)
  unsigned char buf[54] = { 'B', 'M', INT2CHAR (54 + width*height * 32), INT2CHAR (0), INT2CHAR (54), INT2CHAR (40), INT2CHAR (width), INT2CHAR (height), 1, 0, 32, 0 };
  std::ofstream ofs (fname, std::ios_base::out | std::ios_base::binary);
  ofs.write ((char *)buf, sizeof (buf));
  int len = height * width;
  for (int i = 0; i < len; i++) {
    Vec3& color = im[i];
    // printf("(%lf, %lf, %lf), ", color.x, color.y, color.z);
    /* if (i % width == 0) { */
    /*   printf("\n"); */
    /* } */
    buf[0] = (unsigned char)std::min (255, (int)(color.z * 255));
    buf[1] = (unsigned char)std::min (255, (int)(color.y * 255));
    buf[2] = (unsigned char)std::min (255, (int)(color.x * 255));
    buf[3] = (unsigned char)255;
    ofs.write ((char *)buf, 4);
  }
} // bmp's color is bgra order

bool load_bmp (Vec3* im, int height, int width, double inv_scale, std::string fname) {
  std::ifstream is (fname, std::ios_base::binary);
  if (!is) return false;
  unsigned char buf[54];
  is.read ((char *)buf, sizeof (buf));

  // in bmp header, height could be negtive
  int load_width = *(int *)&buf[18];
  int load_height = abs (*(int *)&buf[22]);
  if (load_width != width && load_height != height) {
    std::cerr << "ERROR: <load_bmp> snapshot width and height not consistent." << std::endl;
    return false;
  }

  int len = width * height;
  for (int i = 0; i < len; i++) {
    is.read ((char *)buf, 4);
    im[i] = Vec3(buf[0] / 255.0f, buf[1] / 255.0f, buf[2] / 255.0f) * inv_scale;
  }
  return true;
}
