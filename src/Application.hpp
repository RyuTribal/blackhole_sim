#pragma once

#include "Camera.hpp"
#include "Cubemap.hpp"
#include <string>

struct GLFWwindow;

namespace Sim {
class Application {
public:
  Application(std::string title, int width, int height);
  ~Application();

  Application(const Application &) = delete;
  Application &operator=(const Application &) = delete;

  void Run();

private: // Methods
  void Initialize();
  void Shutdown();
  void OnWindowResized(int width, int height);
  void OnFramebufferResized(int width, int height);
  void OnContentScaleChanged(float xScale, float yScale);

private: // Members
  std::string m_Title;
  int m_Width;
  int m_Height;
  GLFWwindow *m_Window{nullptr};
  bool m_GlfwInitialized{false};
  bool m_ImguiInitialized{false};
  float m_ContentScaleX{1.0f};
  float m_ContentScaleY{1.0f};
  Camera m_Camera{};
  int m_FramebufferWidth{0};
  int m_FramebufferHeight{0};
};
} // namespace Sim
