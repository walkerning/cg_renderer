#pragma once
#include <cstdio>
#include <iostream>
#include <functional>
#include <cstdint>
#include "bbox.h"
#include "renderer.h"
#include "path.h"
#include "utils/hash.h"
#include "utils/rand.h"
#include "utils/image.h"
#include "renderers/hitpoint.h"

// Traditional photon mapper method for comparison use.
struct PhotonMapper: Renderer {
  PhotonMapper(const RendererConf& conf);
  virtual void do_render();
};

struct AdaptivePhotonMapper: PhotonMapper {
  // Configurations
  int num_passes; // # of passes
  int num_photons; // # of photon per pass
  // double initial_mutation_size; // this conf will be stored in mutation_size
  double target_acceptance;

  int snapshot_interval; // interval of snapshoting image to disk. default to `100`.
  std::string snapshot_prefix; // default to `snapshot_`
  std::string snapshot_type; // the suffix of snapshot images. default to `bmp`.

  double initial_radius; // inital radius of hitpoint. will also be used to build the hash map.
  // FIX: calculate using simple heuristic

  double photon_grow_rate; // expected photon grow rate, used to calculate the radius reduce rate. `alpha` in the paper.

  std::string initial_snapshot;
  double initial_snapshot_scale;
  
  // MCMC render attributes
  vector<HitPoint*> hit_points;
  int total_photons;
  int uniform_count;
  int mutated_count;
  int accepted_count;
  double mutation_size;
  // FIXME: hard to parallelize different eye tracing ray (can use lambda expression + closure)
  int ix, iy; // current ray tracing pixel corrdinate.

  // Attributes for hashing: find nearby hitpoints
  BBox bbox; // the bounding box of hit points
  HashMap<uint64_t, HitPoint*> hash_grid;
  double hash_scale; // calculate using radius.
  bool visible; // FIXME: instance shared status hard to parallelize different path

  // TODO: maybe rebuild hash map

  AdaptivePhotonMapper(const RendererConf& conf);

  virtual void do_render();

  void splat_hit_points(Vec3 intersection, Vec3 normal, Vec3 flux);

  bool is_visible(Path& path);

  // progressively accumulate the radiance, reduce the flux and radius of hit points
  void accumulate_radiance();

  RendererConf dump_state();

  static bool trace_eye(Renderer* render, Ray& ray_in, Ray& ray_out, Path& path,
                        Object* obj, Vec3 intersection, BRDF* brdf, int detph);

  static bool trace_light(Renderer* render, Ray& ray_in, Ray& ray_out, Path& path,
                          Object* obj, Vec3 intersection, BRDF* brdf, int detph);

  void build_hash_grid();
};

REGISTER_RENDERER(PhotonMapper, photon_mapper);
REGISTER_RENDERER(AdaptivePhotonMapper, ada_photon_mapper);
