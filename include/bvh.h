#pragma once
#include "object.h"

struct BVHNode {
  BVHNode(int offset_, int size_, BBox bbox_): offset(offset_), size(size_), bbox(bbox_),
                                               left(NULL), right(NULL) {}

  int offset; // offset = -1 for non leaf nodes.
  int size; // leaf size. now, all `size` == 1
  int split_dim; // split coordinate dimension
  double split_coord;
  BBox bbox;
  BVHNode* left;
  BVHNode* right;
};

struct BVH {
  const vector<Object*>& objects;
  vector<BVHNode*> bvh_nodes; // for delete

  BVHNode* root;
  Object** build_objs;
  int leaf_size;

  BVH(const vector<Object*>& objects_);

  // build the BVH
  void build();

  BVHNode* build_node(int start, int end);
  BVHNode* build_node_t(int start, int end);

  // Return intersect or not; the intersection object is in `obj`,
  // travel distance of ray is in `t`
  bool intersect(const Ray& ray, Object* &obj, double& t);

  void print_bvh_tree();

  ~BVH();
};
