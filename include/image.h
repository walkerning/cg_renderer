#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "transform.h"

enum ImageType {
  BMP = 1
};

std::unordered_map<std::string, ImageType> image_type_map = {{"bmp", BMP}};

void write_bmp(Vec3* im, int height, int width, std::string fname);

void write_image(Vec3* im, int height, int width, double scale, std::string fname, std::string type) {
  auto got = image_type_map.find(type);
  if (got == image_type_map.end()) {
    std::cerr << "Error: <image> Unknown image type " << type << std::endl;
    return;
  }
  write_image(im, height, width, fname, got->second);
}

void write_image(Vec3* im, int height, int width, double scale, std::string fname, ImageType type) {
  // copy to im_write
  int len = height * width;
  Vec3* im_write = new Vec3[len];
  for (int i = 0; i < len; i++) {
    im_write[i] = im[i];
  }

  // get normalized radiance
  image_scale(im_write, height, width, scale);
  // gamma correction
  image_gamma(im_write, height, width);

  switch (type) {
  case BMP:
    write_bmp(im_write, height, width, fname);
    break;
  }

  delete[] im_write;
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
    buf[0] = (unsigned char)std::min (255, (int)(color.z * 255));
    buf[1] = (unsigned char)std::min (255, (int)(color.y * 255));
    buf[2] = (unsigned char)std::min (255, (int)(color.x * 255));
    buf[3] = (unsigned char)255;
    ofs.write ((char *)buf, 4);
  }
} // bmp's color is bgra order


