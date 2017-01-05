#include "renderers/photon_mapper.h"

// Photon mapper
PhotonMapper::PhotonMapper(const RendererConf& conf): Renderer(conf) {}

void PhotonMapper::do_render() {}

// Adaptive photon mapper
AdaptivePhotonMapper::AdaptivePhotonMapper(const RendererConf& conf): PhotonMapper(conf),
                                                                      uniform_count(1),
                                                                      mutated_count(0),
                                                                      accepted_count(1) {
  num_passes = std::stoi(find_with_default(conf, "num_passes", "1000"));
  num_photons = std::stoi(find_with_default(conf, "num_photons", "10000"));
  initial_mutation_size = std::stod(find_with_default(conf, "initial_mutation_size", "1.0"));
  target_acceptance = std::stod(find_with_default(conf, "target_acceptance", "0.234"));
  snapshot_interval = std::stoi(find_with_default(conf, "snapshot_interval", "100"));
  snapshot_prefix = std::stoi(find_with_default(conf, "snapshot_prefix", "snapshot_"));
  snapshot_type = find_with_default(conf, "snapshot_type", "bmp");
  //initial_radius = std::stod(find_with_default(conf, "initial_radius", "0.25"));
  photon_grow_rate = std::stod(find_with_default(conf, "photon_grow_rate", "0.7"));
}

bool AdaptivePhotonMapper::trace_eye(Renderer* render, Ray& ray_in, Ray& ray_out, Path& path,
                                     Object* obj, Vec3 intersection, BRDF* brdf, int detph) {
  if (brdf->is_specular()) {
    return true;
  } else {
    // FIXME: shared status used, hard to parallelize now.
    // render->hit_points[render->iy * im_width + render->ix] = Hit
    AdaptivePhotonMapper* t_render = dynamic_cast<AdaptivePhotonMapper*>(render);
    t_render->hit_points.push_back(new HitPoint(intersection,
                                                obj->get_normal(intersection),
                                                ray_in.flux,
                                                t_render->ix, t_render->iy));

    return false;
  }
}

bool AdaptivePhotonMapper::trace_light(Renderer* render, Ray& ray, Ray& ray_out, Path& path,
                                       Object* obj, Vec3 intersection, BRDF* brdf, int depth) {
  if (brdf->is_specular()) {
    return true;
  } else {
    Vec3 normal = obj->get_normal(intersection);
    dynamic_cast<AdaptivePhotonMapper*>(render)->splat_hit_points(intersection, normal, ray.flux);

    // Russian Roulette
    if (depth >= render->rr_depth) {
      double p = ray_out.flux.max();
      if (path.next_value() > p) {
        return false;
      }
      ray_out.flux = ray_out.flux * (1./p);
    }
  }
  return true;
}

void AdaptivePhotonMapper::do_render() {
  Vec3 cx(im_size_ratio);
  Vec3 cy = (cx % env->camera->dir).normalize() * im_size_ratio;
  for (int pass = 0; pass < num_passes; pass++) {
    fprintf(stderr, "Pass #%d:\n", pass);

    // eye tracing pass: shoot ray from camera
    for (iy = 0; iy < im_height; iy++) {
      fprintf(stderr, "\r[eye tracing] Pass #%d: %d / %d rows", pass,
              iy + 1, im_height);
      for (ix = 0; ix < im_width; ix++) {
        double r1 = 2 * uniform_rand();
        // Tent filter: Triangle distribution in [-1, 1)
        double dx = (r1 < 1)? sqrt(r1) - 1: 1 - sqrt(2 - r1);
        double r2 = 2 * uniform_rand();
        double dy = (r2 < 1)? sqrt(r2) - 1: 1 - sqrt(2 - r2);
        Vec3 camera_d = env->camera->dir * im_dist + cx * (ix + dx/2) + cy * (iy + dy/2);
        Ray ray(env->camera->ori, (camera_d - env->camera->ori).normalize());
        Path p; // a stub
        trace(ray, p, AdaptivePhotonMapper::trace_eye, max_depth);
      }
    }
    build_hash_grid();
    // photon tracing pass: shoot photons
    Path cur_path;
    while (!is_visible(cur_path)) {
      cur_path = Path();
    }
    int print_interval = num_photons / 100;
    for (int i = 0; i < num_photons; i++) {
      if (i % print_interval == 0) {
        fprintf(stderr, "\r[photon tracing] Pass #%d: %d / %d photons", pass, i + 1, num_photons);
      }
      Path uni_path = Path();
      if (is_visible(uni_path)) {
        cur_path = uni_path;
        uniform_count += 1;
      } else {
        Path mutate_path = cur_path.mutate(mutation_size);
        mutated_count += 1;
        if (is_visible(mutate_path)) {
          cur_path = mutate_path;
          accepted_count += 1;
        } else {
          is_visible(cur_path);
        }
        mutation_size = mutation_size + (accepted_count / mutated_count - target_acceptance) / mutated_count;
      }
    }
    accumulate_radiance();
    // Progressively store the image
    if (snapshot_interval > 0 && pass > 0 && pass % snapshot_interval == 0) {
      double scale = uniform_count / double(pass * num_photons);
      write_image(im, im_height, im_width, scale, snapshot_prefix + std::to_string(pass) + "." + snapshot_type, snapshot_type);
    }
  }
}

void AdaptivePhotonMapper::accumulate_radiance() {
  for (auto hp : hit_points) {
    double ratio = (hp->N + photon_grow_rate * hp->M) / (hp->N + hp->M);
    hp->radius_sqr = hp->radius_sqr * ratio;
    hp->flux = hp->flux * ratio;
    hp->M = 0; // reset photon count of current photon tracing pass
  }
  // accumulate radiance to image
  int len = im_height * im_width;
  for (int i = 0; i < len; i++) {
    im[i] = hit_points[i]->flux / M_PI / hit_points[i]->radius_sqr;
  }
}

bool AdaptivePhotonMapper::is_visible(Path& path) {
  visible = false;
  path.reset();
  Ray ray = env->light->sample_ray(path);
  trace(ray, path, trace_light, max_depth);
  return visible;
}

void AdaptivePhotonMapper::build_hash_grid() {
  bbox = BBox();
  // calculate initial_radius
  for (auto hp : hit_points) {
    bbox.fit(hp->position);
  }
  initial_radius = (bbox.max - bbox.min).dot(Vec3(1, 1, 1)) / (3. * (im_width + im_height) / 2.) * 2.;
  for (auto hp : hit_points) {
    hp->radius_sqr = initial_radius*initial_radius;
    bbox.fit(hp->position - initial_radius);
    bbox.fit(hp->position + initial_radius);
  }
  hash_scale = 1. / (initial_radius * 2.);
  for (auto hp : hit_points) {
    // Q: this strategy is copy from smallppm_exp.cpp.
    // but why this loop is needed? for handling the edge cases?
    Vec3 bmin = (hp->position - initial_radius - bbox.min) * hash_scale;
    Vec3 bmax = (hp->position + initial_radius - bbox.min) * hash_scale;
    for (int z = uint16_t(bmin.z); z <= uint16_t(bmax.z); z++) {
      for (int y = uint16_t(bmin.y); y <= uint16_t(bmax.y); y++) {
        for (int x = uint16_t(bmin.x); x <= uint16_t(bmax.x); x++) {
          uint64_t key = hash_3int16(x, y, z);
          hash_grid.insert(key, hp);
        }
      }
    }
  }
}

void AdaptivePhotonMapper::splat_hit_points(Vec3 intersection, Vec3 normal, Vec3 flux) {
  Vec3 hh = (intersection - bbox.min) * hash_scale;
  uint16_t x = abs(int(hh.x));
  uint16_t y = abs(int(hh.y));
  uint16_t z = abs(int(hh.z));
  auto hp = hash_grid.find(hash_3int16(x, y, z));
  while (hp != NULL) {
    HitPoint* hitpoint = hp->value;
    // avoid edge brightning
    double normal_eps = 1e-3;
    Vec3 dis = hitpoint->position - intersection;
    if ((hitpoint->normal.dot(normal) > normal_eps) && dis.dot(dis) <= hitpoint->radius_sqr) {
      // accumulate unormalized flux on this hit point
      hitpoint->flux = hitpoint->flux + flux;
      hitpoint->M = hitpoint->M + 1;
      hitpoint->N = hitpoint->N + 1;
    }
  }
}
