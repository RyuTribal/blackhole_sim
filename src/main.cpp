#include "Application.hpp"
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <memory> // Required for std::unique_ptr

#ifdef IS_WEB
#include <emscripten.h>
#endif

std::unique_ptr<Sim::Application> app;

void UpdateLoop() {
  if (app) {
    app->MainLoop();
  }
}

int main() {
  try {
    app = std::make_unique<Sim::Application>("Black hole simulator", 1280, 720);

#ifdef IS_WEB
    emscripten_set_main_loop(UpdateLoop, 0, 1);
#else
    app->Run();
#endif

  } catch (const std::exception &e) {
    std::fprintf(stderr, "Fatal error: %s\n", e.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
