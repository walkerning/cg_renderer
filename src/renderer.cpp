#include "render.h"

Renderer* Renderer::get_renderer(const RendererConf& conf) {
  std::string type;
  auto got = conf.find("type");
  if (got == conf.end()) {
    type = "adaptive_photon_mapper"; // default type
  } else {
    type = got->second;
  }
  return RenderRegistry[type](conf);
}
