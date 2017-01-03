#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "brdf.h"
#include "bbox.h"
#include "vec3.h"

// struct Texture {};

struct Index {
  // The indexes of three vertices of a triangle
  int pos[3];
  int uv[3];
  int normal[3];
};

struct Object {
  BRDF* brdf;

  Object(BRDF* brdf_): brdf(brdf_) {}

  // Return true when there is a intersection. Travel distance `t` will be stored.
  virtual bool intersect(const Ray& ray, double& t) = 0;

  // Return the axis-aligned bounding box of this object.
  virtual BBox get_bbox() = 0;

  // Return the normal at one pos (world position)
  virtual Vec3 get_normal(Vec3 pos) = 0;

  // Return the centroid
  virtual Vec3 get_centroid() = 0;

  // Return type information
  virtual inline const char* type() const = 0;

};

struct Triangle: Object {
  Vec3 v[3];
  Vec3 normal;
  double area;

  Triangle(Vec3 v1_, Vec3 v2_, Vec3 v3_, BRDF *brdf_): Object(brdf_) {
    v[0] = v1_;
    v[1] = v2_;
    v[2] = v3_;
    normal = (v[1] - v[0]) % (v[2] - v[0]);
    area = normal.norm();
    normal = normal.normalize();
  }

  bool on_plane(Vec3 v) {
    double eps = 1e-5;
    return abs(normal.dot(v[0] - v)) < eps &&  abs(normal.dot(v[1] - v)) < eps;
  }

  bool include(Vec3 v) {
    double eps = 1e-5;
    // check three sub triangle's direction
    double sub_area1 = ((v[0] - v) % (v[1] - v)).dot(normal);
    double sub_area2 = ((v[1] - v) % (v[2] - v)).dot(normal);
    double sub_area3 = ((v[2] - v) % (v[0] - v)).dot(normal);
    if (sub_area1 >= -eps && sub_area2 >= -eps && sub_area3 >= -eps) {
      return true;
    }
    return false;
  }

  virtual bool intersect(const Ray& ray, double& t) {
    // intersection with the plane
    t = normal.dot(v[0] - ray.ori) / normal.dot(ray.dir);
    Vec3 v = ray.ori + ray.dir * t;
    return include(v);
  }

  virtual BBox get_bbox() {
    BBox bbox(v[0]);
    bbox.fit(v[1]);
    bbox.fit(v[2]);
    return bbox;
  }

  virtual Vec3 get_normal(Vec3 pos) {
    return normal;
  }

  virtual Vec3 get_centroid() {
    return (v[0] + v[1] + v[2])/3.;
  }
  virtual inline const char* type() const { return "Triangle"; }
};

struct NormalizedTriangle: Triangle {
  Vec3 v_normal[3];

  NormalizedTriangle(Vec3 v1_, Vec3 v2_, Vec3 v3_, Vec3 n1_, Vec3 n2_, Vec3 n3_, BRDF *brdf_):
    Triangle(v1_, v2_, v3_, brdf_) {
    // true vertex normal
    v_normal[0] = n1_;
    v_normal[1] = n2_;
    v_normal[2] = n3_;
  }

  virtual Vec3 get_normal(Vec3 pos) {
    double sub_area1 = ((v[0] - pos) % (v[1] - pos)).norm();
    double sub_area2 = ((v[1] - pos) % (v[2] - pos)).norm();
    double sub_area3 = ((v[2] - pos) % (v[0] - pos)).norm();
    return v_normal[0] * (sub_area1 / area) + v_normal[1] * (sub_area2 / area) +
      v_normal[2] * (sub_area3 / area);
  }

  virtual inline const char* type() const { return "NormalizedTriangle"; }
};

struct Sphere: Object {
  Vec3 position;
  double radius;

  Sphere(Vec3 origin_, double radius_, BRDF *brdf_): Object(brdf_), origin(origin_), radius(radius_) {}

  virtual bool intersect(const Ray& ray, double& t) {
    // Solve `t^2 - 2*t*(position - ori)^T dir + ||position - ori||^2 - R^2 = 0`
    double eps=1e-4;
    Vec3 ori_to_pos = position - ray.ori;
    double b = ori_to_pos.dot(ray.dir);
    double det= b * b - ori_to_pos.dot(ori_to_pos) + radius * radius;
    if (det < 0)
      return false;
    else det=sqrt(det);
    t = b - det;
    if (t > eps)
      return true;
    t = b + det;
    if (t > eps)
      return true;
    return false;
  }

  virtual BBox get_bbox() {
    return BBox(position - radius, position + radius);
  }

  virtual Vec3 get_normal(Vec3 pos) {
    return (pos - position).normalize();
  }

  virtual Vec3 get_centroid() {
    return position;
  }

  virtual inline const char* type() const { return "Sphere"; }
};

struct MeshObject: Object {
  // Texture texture; // for now, do not handle uv maps
  Vec3 position;
  std::vector<Triangle*> triangles;
  std::vector<Vec3> posBuffer, normalBuffer, uvBuffer;
  std::vector<Index> indexBuffer;

  MeshObject(Vec3 position_, std::string model_fname, BRDF* brdf_): Object(brdf_), position(position_) {
    load_obj_from_file(model_fname);
  }

  void load_obj_from_file(std::string fname) {
    std::ifstream is(fname);
    load_obj_from_stream(is);
  }

  // Borrowed code from cobra.cpp
  void load_obj_from_stream(std::istream& is) {
    float x, y, z;
    std::string str;
    char dummy;
    while (std::getline(is, str)) {
      if (str.length () < 2) continue;
      std::istringstream iss (str);
      std::string token;
      if (str[1] == 't' && str[0] == 'v') { // load uv. "vt -0.05 0.0972793"
        iss >> token >> x >> y;
        uvBuffer.push_back (Vec3(x, y, 1)); // uv coordinate on texture map
      } else if (str[1] == 'n' && str[0] == 'v') { // load normal. "vn -0.981591 -0.162468 0.100411"
        iss >> token >> x >> y >> z;
        normalBuffer.push_back (Vec3(x, y, z)); // normals
      } else if (str[0] == 'v') { // load postion. "v -0.983024 -0.156077 0.0964607"
        iss >> token >> x >> y >> z;
        posBuffer.push_back (Vec3(x, y, z)); // 3-d position of vertex
      } else if (str[0] == 'f') { // load index. keep in mind that uv/normal index are optional.
        Index index = { { 0 } };
        if (str.find ("//") != std::string::npos) { // pos//normal, no uv. "f 181//176 182//182 209//208"
          iss >> token >> index.pos[0] >> dummy >> dummy >> index.normal[0] >>
            index.pos[1] >> dummy >> dummy >> index.normal[1] >>
            index.pos[2] >> dummy >> dummy >> index.normal[2];
        } else {
          size_t count = 0, pos = str.find ('/');
          while (pos != std::string::npos) { count++; pos = str.find ('/', pos + 1); }
          if (count == 6) { // "f 181/292/176 182/250/182 209/210/208"
            iss >> token >> index.pos[0] >> dummy >> index.uv[0] >> dummy >> index.normal[0] >>
              index.pos[1] >> dummy >> index.uv[1] >> dummy >> index.normal[1] >>
              index.pos[2] >> dummy >> index.uv[2] >> dummy >> index.normal[2];
          } else if (count == 3) { // pos/uv, no normal. "f 181/176 182/182 209/208"
            iss >> token >> index.pos[0] >> dummy >> index.uv[0] >> index.pos[1] >> dummy >> index.uv[1] >> index.pos[2] >> dummy >> index.uv[2];
          }
        }
        indexBuffer.push_back (index);
      }
    } // end parsing
    for (auto &index : indexBuffer) {
      for (int i = 0; i < 3; i++) {
        if (index.pos[i] < 0) index.pos[i] += (int)posBuffer.size ();
        if (index.uv[i] < 0) index.uv[i] += (int)uvBuffer.size ();
        if (index.normal[i] < 0) index.normal[i] += (int)normalBuffer.size ();
      } // deal with negative index
      triangles.push_back(new NormalizedTriangle(posBuffer[index.pos[0]] + position,
                                                 posBuffer[index.pos[1]] + position,
                                                 posBuffer[index.pos[2]] + position,
                                                 brdf,
                                                 normalBuffer[index.normal[0]],
                                                 normalBuffer[index.normal[1]],
                                                 normalBuffer[index.normal[2]],));
    }
  }

  virtual bool intersect(const Ray& ray, double& t) {
    // This function should not be called when use BVH.
    for (const auto& tri : triangles) {
      bool intersect = tri->intersect(ray, t);
      if (intersect && (tri->normal).dot(ray.dir) < 0) {
        return true;
      }
    }
    return false;
  }

  virtual BBox get_bbox() {
    BBox bbox;
    for (const auto& tri : triangles) {
      bbox.merge(tri->get_bbox());
    }
    return bbox;
  }

  virtual Vec3 get_normal(Vec3 pos) {
    // stupid: just loop through...
    for (const auto& tri : triangles) {
      // check in which triangle
      if (tri->on_plane(pos) && tri->include(pos)) {
        return tri->get_normal(pos);
      }
    }
    // error
    return Vec3(0, 0, 0);
  }

  virtual Vec3 get_centroid(Vec3 pos) {
    return position;
  }

  virtual inline const char* type() const { return "Mesh"; }
};
