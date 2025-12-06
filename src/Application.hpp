#pragma once

#include "BlackHole.hpp"
#include "Camera.hpp"
#include "Cubemap.hpp"
#include "FinalImage.hpp"
#include <glm/glm.hpp>
#include <memory>
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

  BlackHole *GetBlackHole() { return m_BlackHole.get(); }
  void MainLoop();

private: // Methods
  void Initialize();
  void Shutdown();
  void OnWindowResized(int width, int height);
  void OnFramebufferResized(int width, int height);
  void OnContentScaleChanged(float xScale, float yScale);
  void OnMouseMove(float x, float y);
  void OnMouseButton(int button, int action, int mods);

  void CheckMovement();

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
  int m_MovementSpeed{5};
  float m_DeltaTime{0.f};

  std::unique_ptr<BlackHole> m_BlackHole;
  std::unique_ptr<FinalImage> m_FinalImage;

  glm::vec2 m_LastMousePos{0.f, 0.f};
  long long m_PrevTime{0};
  glm::vec3 m_ClearColor{0.10f, 0.13f, 0.17f};
  std::string m_PrettyExponent;
};
} // namespace Sim
