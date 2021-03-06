#include "bvh.h"

BVH::BVH(const vector<Object*>& objects_): objects(objects_), build_objs(NULL) {
  leaf_size = 1;
}

void BVH::build() {
  if (build_objs) {
    delete[] build_objs;
    build_objs = NULL;
  }
  int num_objs = objects.size();
  build_objs = new Object*[num_objs];
  for (int i = 0; i < num_objs; i++) {
    build_objs[i] = objects[i];
  }
  root = build_node(0, num_objs);
}

// A small decorator wrapper for recording BVHNode* in a flat vector. for destruct use.
BVHNode* BVH::build_node(int start, int end) {
  BVHNode* nodep = build_node_t(start, end);
  if (nodep != NULL) {
    bvh_nodes.push_back(nodep);
  }
  return nodep;
}

BVHNode* BVH::build_node_t(int start, int end) {
  if (end - start == 0) { // empty
    return NULL;
  }
  BBox bbox;
  for (int i = start; i < end; i++) {
    bbox.merge(build_objs[i]->get_bbox());
  }
  if (end - start <= leaf_size) { // leaf
    BVHNode* nodep = new BVHNode(start, end - start, bbox);
    return nodep;
  }
  BVHNode* nodep = new BVHNode(-1, end - start, bbox);

  // find good split dimension
  int split_dim = bbox.max_dim();
  nodep->split_dim = split_dim;
  double split_coord = (bbox.min[split_dim] + bbox.max[split_dim]) / 2.;
  nodep->split_coord = split_coord;

  // partition O(N)
  int mid = start;
  for (int ind = start; ind < end; ind++) {
    if (build_objs[ind]->get_centroid()[split_dim] < split_coord) {
      Object* tmp = build_objs[ind];
      build_objs[ind] = build_objs[mid];
      build_objs[mid] = tmp;
      mid++;
    }
  }

  // very bad situation, make a equal-size split.
  // also avoid dead loop
  if (mid == start || mid == end) {
    mid = start + (end - start) / 2;
  }

  // and conquer
  BVHNode* left_child = build_node(start, mid);
  BVHNode* right_child = build_node(mid, end);
  nodep->left = left_child;
  nodep->right = right_child;
  return nodep;
}

struct _WorkingNode {
  _WorkingNode() {}

  _WorkingNode(BVHNode* nodep_, double t_): nodep(nodep_), bbox_t(t_) {}

  BVHNode* nodep;

  // For pruning bbox whose intersection distance is bounded by
  // current minimum object intersection distance
  double bbox_t;
};

bool BVH::intersect(const Ray& ray, Object* &obj, double& t) {
  // FIXME: here a magic number 64... constrain the max expanding number
  // Simulate a working stack for depth-first search
  obj = NULL;
  _WorkingNode working[64];
  int working_index = 0;
  working[0].nodep = root;
  working[0].bbox_t = 1e10; // FIXME: magic number..
  t = 1e10;

  while (working_index >= 0) {
    _WorkingNode w_node = working[working_index];
    working_index--;
    if (w_node.bbox_t > t) {
      // this node's intersect distance is larger than current finded true object intersection distance `t`
      continue;
    }
    BVHNode* nodep = w_node.nodep;
    if (nodep->offset != -1) { // leaf node
      for (int i = nodep->offset; i < nodep->offset + nodep->size; i++) {
        double dist;
        Object* cur_obj = build_objs[i];
        bool intersected = cur_obj->intersect(ray, dist);
        if (intersected && dist < t) {
          obj = cur_obj;
          t = dist;
        }
      }
    } else { // non-leaf node
      BVHNode* left_nodep = nodep->left;
      BVHNode* right_nodep = nodep->right;
      double left_dist, right_dist;
      bool left_int = left_nodep->bbox.intersect(ray, left_dist);
      bool right_int = right_nodep->bbox.intersect(ray, right_dist);
      if (left_int && right_int) {
        // A optimization: push closer-intersected bbox last.
        if (left_dist < right_dist) {
          working[++working_index] = _WorkingNode(right_nodep,
                                                  right_dist);
          working[++working_index] = _WorkingNode(left_nodep,
                                                  left_dist); // close one
        } else {
          working[++working_index] = _WorkingNode(left_nodep,
                                                  left_dist);
          working[++working_index] = _WorkingNode(right_nodep,
                                                  right_dist); // close one
        }
      } else if (left_int) {
        working[++working_index] = _WorkingNode(left_nodep,
                                                left_dist);
      } else if (right_int) {
        working[++working_index] = _WorkingNode(right_nodep,
                                                right_dist);
      }
    }
  }
  return obj != NULL;
}

BVH::~BVH() {
  if (build_objs) {
    delete[] build_objs;
  }
  for (auto nodepp : bvh_nodes) {
    delete nodepp;
  }
  bvh_nodes.clear();
}


void BVH::print_bvh_tree() {}
