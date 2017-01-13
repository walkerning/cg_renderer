#include "renderers/photon_mapper.h"
#include <cstdio> // for dbg

// Photon mapper
PhotonMapper::PhotonMapper(const RendererConf& conf): Renderer(conf) {}

void PhotonMapper::do_render() {}

// Adaptive photon mapper
AdaptivePhotonMapper::AdaptivePhotonMapper(const RendererConf& conf): PhotonMapper(conf),
                                                                      uniform_count(1),
                                                                      mutated_count(0),
                                                                      accepted_count(1),
								      total_photons(0) {
  num_passes = std::stoi(find_with_default(conf, "num_passes", "1000"));
  num_photons = std::stoi(find_with_default(conf, "num_photons", "10000"));
  mutation_size = std::stod(find_with_default(conf, "initial_mutation_size", "1.0"));
  target_acceptance = std::stod(find_with_default(conf, "target_acceptance", "0.234"));
  snapshot_interval = std::stoi(find_with_default(conf, "snapshot_interval", "100"));
  snapshot_prefix = find_with_default(conf, "snapshot_prefix", "snapshot_");
  snapshot_type = find_with_default(conf, "snapshot_type", "bmp");
  initial_snapshot = find_with_default(conf, "intitial_snapshot", "");
  initial_snapshot_scale = std::stod(find_with_default(conf, "initial_snapshot_scale", "0"));
  initial_radius = std::stod(find_with_default(conf, "initial_radius", "0.2"));
  photon_grow_rate = std::stod(find_with_default(conf, "photon_grow_rate", "0.7"));

  // load initial snapshot
  // FIXME: only support bmp... should modify to the same as write_image
  if (!initial_snapshot.empty()) {
    int succ = load_bmp(im, im_height, im_width, initial_snapshot_scale, initial_snapshot + ".bmp");
    if (!succ) {
      std::cerr << "ERROR: <adaptive_photon_mapper> load initial snapshot failed." << std::endl;
    }
    // load state
    auto state = read_conf(initial_snapshot + ".state");
    total_photons = std::stoi(find_with_default(state, "total_photons", "0"));
    uniform_count = std::stoi(find_with_default(state, "uniform_count", "1"));
    mutated_count = std::stoi(find_with_default(state, "mutated_count", "0"));
    accepted_count = std::stoi(find_with_default(state, "accepted_count", "1"));
    mutation_size = std::stoi(find_with_default(state, "mutation_size", std::to_string(mutation_size)));
  }
}

bool AdaptivePhotonMapper::trace_eye(Renderer* render, Ray& ray_in, Ray& ray_out, Path& path,
                                     Object* obj, Vec3 intersection, BRDF* brdf, int detph) {
  // printf("trace eye, intersect with object type %s!\n", obj->type());
  if (brdf->is_specular()) {
    return true;
  } else {
    // FIXME: shared status used, hard to parallelize now.
    // render->hit_points[render->iy * im_width + render->ix] = Hit
    AdaptivePhotonMapper* t_render = dynamic_cast<AdaptivePhotonMapper*>(render);
    int index = t_render->iy * t_render->im_width + t_render->ix;
    t_render->hit_points[index]->position = intersection;
    t_render->hit_points[index]->normal = obj->get_normal(intersection),
    t_render->hit_points[index]->weight = ray_in.flux;
    // t_render->hit_points[index]->pixel_x = t_render->ix;
    // t_render->hit_points[index]->pixel_y = t_render->iy;

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

// for debug
std::ostream& operator<<(std::ostream& out, Vec3& v) { out << "Vec3(" << v.x << ", " << v.y << ", " << v.z << ")"; }

void AdaptivePhotonMapper::do_render() {
  Vec3 cx(im_size_ratio);
  Vec3 cy = (cx % env->camera->dir).normalize() * im_size_ratio;
  std::cerr << "cx: " << cx << "\tcy:" << cy << std::endl; 
  // FIXME: Calculate_initial_radius... the initial radius calculate heurestic need bbox of hitpoints. 
  // but it's difficult to calculate the exact extent bounding box
  // of hitpoints when hitpoints perturb every pass...
  // Can specify the space size... 
  for (iy = 0; iy < im_height; iy++) {
    for (ix = 0; ix < im_width; ix++) {
      HitPoint* hp = new HitPoint(ix, iy);
      hp->radius_sqr = initial_radius * initial_radius;
      hit_points.push_back(hp);
    }
  }
  for (int pass = 0; pass < num_passes; pass++) {
    fprintf(stderr, "Pass #%d:\n", pass);
    // eye tracing pass: shoot ray from camera
    for (iy = 0; iy < im_height; iy++) {
      fprintf(stderr, "\r[eye tracing] Pass #%d: %d / %d rows", pass,
              iy + 1, im_height);
      for (ix = 0; ix < im_width; ix++) {
        // Tent filter: Triangle distribution in [-1, 1)
        double r1 = 2 * uniform_rand();
        double dx = (r1 < 1)? sqrt(r1) - 1: 1 - sqrt(2 - r1);
        double r2 = 2 * uniform_rand();
        double dy = (r2 < 1)? sqrt(r2) - 1: 1 - sqrt(2 - r2);
        Vec3 camera_d = env->camera->dir * im_dist + cx * ((ix + dx/2) - im_width/2) + cy * ((iy + dy/2) - im_height/2);
        Ray ray(env->camera->ori, camera_d.normalize());
        //printf("\nray dir: ");
        //ray.dir.print();
        Path p; // a stub
        trace(ray, p, AdaptivePhotonMapper::trace_eye, max_depth);
      }
    }
    fprintf(stderr, " ... FINISHED\n");

    fprintf(stderr, "[building hash] Pass #%d", pass);
    build_hash_grid();
    fprintf(stderr, " ... FINISHED\n");

    // dbg: print hash grid
    printf("hit points number: %d\n", hit_points.size());
    // for (auto hp : hit_points) {
    //   hp->position.print();
    //   printf("\n");
    // }
    printf("hash gird size: %d\n", hash_grid.size());
    // photon tracing pass: shoot photons
    // uniform initialize a MCMC path
    Path cur_path;
    int try_times = 1;
    while (!is_visible(cur_path)) {
      try_times++;
      fprintf(stderr, "\r[initialize MCMC path] Pass #%d: trying #%d times", pass, try_times);
      cur_path = Path();
    }
    fprintf(stderr, " ... FINISHED ... %d time tried.\n", try_times);

    int print_interval = num_photons / 100;
    for (int i = 0; i < num_photons; i++) {
      if ((i + 1) % print_interval == 0) {
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
        mutation_size = mutation_size + (double(accepted_count) / mutated_count - target_acceptance) / double(mutated_count);
	// clip mutation size to avoid mutation_size < 0
	mutation_size = std::max(mutation_size, 1e-4);
      }
    }
    fprintf(stderr, " ... FINISHED\n");

    // progressively accumulate the radiance, reduce the flux and radius of hit points
    accumulate_radiance();
    printf("accumulate finish\n");

    total_photons += num_photons;
    // Progressively store the image
    if (snapshot_interval > 0 && (pass + 1) % snapshot_interval == 0) {
      // double scale = uniform_count / double(total_photons);
      double scale = 1; // FIXME: no meaning, because of the auto exposure
      std::string fname = snapshot_prefix + std::to_string(pass + 1) + "." + snapshot_type;
      std::string state_fname = snapshot_prefix + std::to_string(pass + 1) + ".state";
      fprintf(stderr, "[write image] writing image to %s. scale %lf", fname.c_str(), scale);
      write_image(im, im_height, im_width, scale, fname, snapshot_type);
      write_conf(dump_state(), state_fname);
      fprintf(stderr, " ... FINISHED\n");
    }
  }
}

RendererConf AdaptivePhotonMapper::dump_state() {
  RendererConf state = {
    {"total_photons", std::to_string(total_photons)},
    {"uniform_count", std::to_string(uniform_count)},
    {"mutated_count", std::to_string(mutated_count)},
    {"accepted_count", std::to_string(accepted_count)},
    {"mutation_size", std::to_string(mutation_size)}
  };
  return state;
}

void AdaptivePhotonMapper::accumulate_radiance() {
  int ht_num = 0;
  for (auto hp : hit_points) {
    if (hp->M != 0) {
      ht_num += 1;
    }
  }
  // for (auto hp : hit_points) {
  //   if (hp->M != 0) {
  //     // printf("N: %d; M: %d\n", hp->N, hp->M);
  //     double ratio = (double)(hp->N + photon_grow_rate * hp->M) / (hp->N + hp->M);
  //     hp->radius_sqr = hp->radius_sqr * ratio;
  //     hp->flux = hp->flux * ratio;
  //     hp->N = hp->M + hp->N; // FIXME: not saved here!!!
  //     hp->M = 0; // reset photon count of current photon tracing pass
  //     ht_num += 1;
  //   }
  // }
  fprintf(stderr, "hited hitpoint: %d\n", ht_num);
  // accumulate radiance to image
  int len = hit_points.size();
  for (int i = 0; i < len; i++) {
    int im_ind = hit_points[i]->pixel_y * im_width + hit_points[i]->pixel_x;
    //printf("(%d, %d), ", hit_points[i]->pixel_y, hit_points[i]->pixel_x);
    // if (i % 50 == 0) {
    //   printf("\n");
    // }
    // im[im_ind] = im[im_ind] + hit_points[i]->flux / M_PI / hit_points[i]->radius_sqr;
    im[im_ind] = hit_points[i]->flux / M_PI / hit_points[i]->radius_sqr;
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
  hash_grid.reset();
  // bbox = BBox();
  // // calculate initial_radius
  // for (auto hp : hit_points) {
  //   bbox.fit(hp->position);
  // }

  // initial_radius = (bbox.max - bbox.min).dot(Vec3(1, 1, 1)) / (3. * (im_width + im_height) / 2.) * 2.;
  //Vec3 extent = bbox.max - bbox.min;
  //initial_radius = extent[bbox.max_dim()] / MIN(im_height, im_width) * 2;
  // for (auto hp : hit_points) {
  //   // hp->radius_sqr = initial_radius*initial_radius;
  //   bbox.fit(hp->position - initial_radius);
  //   bbox.fit(hp->position + initial_radius);
  // }

  // Hash cube edge length should be the diameter of the hitpoint circle
  // As the radius of hitpoints are reducing, later pass should use bigger hash_scale for efficiency.
  // But for now. do not change the hash scale! TODO
  // hash_scale = 1. / (initial_radius * 2.); 
  hash_scale = 1. / (initial_radius * 2.); 

  for (auto hp : hit_points) {
    // This loop is neccesary to make every position in hp->position +-initial_radius
    // could find `hp` in the hashing link list.
    // Vec3 bmin = (hp->position - initial_radius - bbox.min) * hash_scale;
    // Vec3 bmax = (hp->position + initial_radius - bbox.min) * hash_scale;
    double rad = sqrt(hp->radius_sqr);
    Vec3 bmin = (hp->position - rad) * hash_scale;
    Vec3 bmax = (hp->position + rad) * hash_scale;
    for (int z = uint16_t(bmin.z); z <= uint16_t(bmax.z); z++) {
      for (int y = uint16_t(bmin.y); y <= uint16_t(bmax.y); y++) {
        for (int x = uint16_t(bmin.x); x <= uint16_t(bmax.x); x++) {
          uint64_t key = hash_3int16_2(x, y, z);
          hash_grid.insert(key, hp);
        }
      }
    }
  }
}

void AdaptivePhotonMapper::splat_hit_points(Vec3 intersection, Vec3 normal, Vec3 flux) {
  //for dbg
  // printf("intersection: "); intersection.print(); printf("\n");
  //Vec3 hh = (intersection - bbox.min) * hash_scale;
  Vec3 hh = intersection * hash_scale;
  uint16_t x = abs(int(hh.x));
  uint16_t y = abs(int(hh.y));
  uint16_t z = abs(int(hh.z));
  auto hp = hash_grid.find(hash_3int16_2(x, y, z));
  while (hp != NULL) {
    HitPoint* hitpoint = hp->value;
    // avoid edge brightning
    double normal_eps = 1e-4;
    Vec3 dis = hitpoint->position - intersection;
    //printf("dis_sqr: %lf; radius_sqr: %lf\n", dis.dot(dis), hitpoint->radius_sqr);
    if ((hitpoint->normal.dot(normal) > normal_eps) && dis.dot(dis) <= hitpoint->radius_sqr) {
      // accumulate unormalized flux on this hit point
      // printf("visible! hitpoint hit!\n");
      visible = true;
      double ratio = (double)(hitpoint->M + 1) / (hitpoint->M + 1. / photon_grow_rate);
      hitpoint->radius_sqr *= ratio;
      hitpoint->flux = (hitpoint->flux + flux * hitpoint->weight) * ratio;
      // printf("hitpoint weight: ");
      // hitpoint->weight.print();
      // std::cerr << "hitpoint weight: " << hitpoint->weight << "accumulated flux: " << hitpoint->flux << "\tray in flux: " << flux << std::endl;
      //hitpoint->flux.print();
      // printf(";  ray in flux: ");
      // flux.print();
      hitpoint->M = hitpoint->M + 1;
      //hitpoint->N = hitpoint->N + 1;
    }
    hp = hp->next;
  }
}

AdaptivePhotonMapper::~AdaptivePhotonMapper() {
  for (auto& hp : hit_points) {
    delete hp;
  }
}
