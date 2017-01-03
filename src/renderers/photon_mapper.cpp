#include "renderers/photon_mapper.h"

// Photon mapper
PhotonMapper::PhotonMapper(const RendererConf& conf): Renderer(conf) {}

void PhotonMapper::trace(Ray& ray, Path& path, bool backward_trace) {

}

void PhotonMapper::do_render() {}

// Adaptive photon mapper
AdaptivePhotonMapper::AdaptivePhotonMapper(const RendererConf& conf): Renderer(conf) {}

void AdaptivePhotonMapper::do_render() {
  // shoot ray from camera
}
