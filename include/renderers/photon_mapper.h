#pragma once
#include "renderer.h"

// Traditional photon mapper method for comparison use.
struct PhotonMapper: Renderer {
  PhotonMapper(const RendererConf& conf);
  // When shoot ray from eye `backward_trace` should be set to `true`.
  void trace(Ray& ray, Path& path, bool backward_trace=false);
  virtual void do_render();
};

struct AdaptivePhotonMapper: PhotonRenderer {
  AdaptivePhotonMapper(const RendererConf& conf);
  virtual void do_render();
};

REGISTER_RENDERER(PhotonMapper, "photon_mapper");
REGISTER_RENDERER(PhotonMapper, "ada_photon_mapper");
