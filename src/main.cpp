#include "Application.hpp"

#include <cstdio>
#include <cstdlib>
#include <exception>

int main() {
  try {
    Sim::Application app("Black hole simulator", 1280, 720);
    app.Run();
  } catch (const std::exception &e) {
    std::fprintf(stderr, "Fatal error: %s\n", e.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
