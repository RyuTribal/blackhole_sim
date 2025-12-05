#include "Application.hpp"
#include "BlackHole.hpp"

#if defined(IS_WEB)
#include <emscripten.h>
#endif

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <utility>

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#if defined(IS_WEB)
#include <glad/gles2.h>
#else
#include <glad/gl.h>
#endif

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdio>

namespace {
void glfw_error_callback(int error, const char *description) {
  std::fprintf(stderr, "GLFW error (%d): %s\n", error,
               description ? description : "no message");
}
} // namespace

namespace Sim {

std::string to_superscript(int number) {
  const std::vector<std::string> supers = {
      "\u2070", "\u00B9", "\u00B2", "\u00B3", "\u2074",
      "\u2075", "\u2076", "\u2077", "\u2078", "\u2079"};

  std::string num_str = std::to_string(number);
  std::string result = "";

  for (char c : num_str) {
    int digit = c - '0';
    result += supers[digit];
  }
  return result;
}

Application::Application(std::string title, int width, int height)
    : m_Title(std::move(title)), m_Width(width), m_Height(height) {
  Initialize();
  m_BlackHole = std::make_unique<BlackHole>();
  m_FinalImage = std::make_unique<FinalImage>(m_Width, m_Height);
  m_PrevTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::high_resolution_clock::now().time_since_epoch())
                   .count();
  m_PrettyExponent = to_superscript(30);
}

Application::~Application() { Shutdown(); }

void Application::Initialize() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW.");
  }
  m_GlfwInitialized = true;

#if defined(IS_WEB)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#endif
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

  m_Window =
      glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
  if (!m_Window) {
    throw std::runtime_error("Failed to create GLFW window.");
  }

  glfwMakeContextCurrent(m_Window);
  glfwSwapInterval(1);

  glfwSetWindowUserPointer(m_Window, this);
#if defined(IS_WEB)
  if (!gladLoadGLES2(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
    throw std::runtime_error("Failed to initialize GLAD.");
  }
#else
  if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
    throw std::runtime_error("Failed to initialize GLAD.");
  }
#endif

  glEnable(GL_DEPTH_TEST);

  glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width,
                                         int height) {
    auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    if (app) {
      app->OnWindowResized(width, height);
    }
  });

  glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow *window, int width,
                                              int height) {
    auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    if (app) {
      app->OnFramebufferResized(width, height);
    }
  });
  glfwSetWindowContentScaleCallback(m_Window, [](GLFWwindow *window,
                                                 float xScale, float yScale) {
    auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    if (app) {
      app->OnContentScaleChanged(xScale, yScale);
    }
  });

  glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double x,
                                        double y) {
    auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    if (app) {
      app->OnMouseMove(x, y);
      app->m_LastMousePos = {x, y};
    }
  });

  glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button,
                                          int action, int mods) {
    auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    if (app) {
      app->OnMouseButton(button, action, mods);
    }
  });

  glfwSwapInterval(0);

  float xScale = 1.0f;
  float yScale = 1.0f;
  glfwGetWindowContentScale(m_Window, &xScale, &yScale);
  OnContentScaleChanged(xScale, yScale);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::StyleColorsDark();
  ImFontConfig config;
  config.MergeMode = true;

  static const ImWchar ranges[] = {
      0x0020, 0x00FF, // Standard Text
      0x2070, 0x209F, // Superscripts (0, 4, 5, 6, 7, 8, 9)
      0,              // Null Terminator
  };

  io.Fonts->AddFontFromFileTTF("assets/fonts/JuliaMono/JuliaMono-Regular.ttf",
                               18.f, NULL, ranges);

  if (!ImGui_ImplGlfw_InitForOpenGL(m_Window, true)) {
    throw std::runtime_error("Failed to initialize Dear ImGui GLFW backend.");
  }
#if defined(IS_WEB)
  if (!ImGui_ImplOpenGL3_Init("#version 300 es")) {
    throw std::runtime_error("Failed to initialize Dear ImGui OpenGL backend.");
  }
#else
  if (!ImGui_ImplOpenGL3_Init("#version 410")) {
    throw std::runtime_error("Failed to initialize Dear ImGui OpenGL backend.");
  }
#endif

  m_ImguiInitialized = true;
}

void Application::MainLoop() {
  if (glfwWindowShouldClose(m_Window)) {
#if defined(IS_WEB)
    emscripten_cancel_main_loop();
#endif
    return;
  }

  glfwPollEvents();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin(m_Title.c_str());
  ImGui::Text("Black hole settings");
  ImGui::DragFloat3("Position", &m_BlackHole->m_Position.x, 0.1f);
  float mass = m_BlackHole->GetMass();
  if (ImGui::DragFloat("Solar Mass", &mass, 0.1f)) {
    m_BlackHole->SetMass(mass);
  }
  ImGui::Text((const char *)u8"NOTE: 1 Solar mass = Mass of our Sun \u2248 "
                            u8"1.989 \u00D7 10%s kg",
              m_PrettyExponent.c_str());
  ImGui::Text("Event horizon: %.6f", m_BlackHole->GetEventHorizon());
  ImGui::Dummy(ImVec2(0.0f, 20.0f));

  ImGui::Text("Accretion Disc");
  ImGui::Checkbox("Show Disc", &m_BlackHole->m_ShowDisk);

  if (m_BlackHole->m_ShowDisk) {
    ImGui::DragFloat("Gravity Bend", &m_BlackHole->m_BendFactor, 0.1f, 0.0f,
                     15.0f);
    ImGui::DragFloat("Disc Height", &m_BlackHole->m_DiskHeight, 0.01f, 0.01f,
                     2.0f);
    ImGui::DragFloat("Intensity", &m_BlackHole->m_DiskIntensity, 0.5f, 0.0f,
                     200.0f);
    ImGui::DragFloat("Density", &m_BlackHole->m_DiskAlpha, 0.1f, 0.0f, 20.0f);
  }
  ImGui::Dummy(ImVec2(0.0f, 20.0f));

  ImGui::Text("Simulation settings");
  ImGui::DragFloat("Min step", &m_BlackHole->m_StepMin, 0.001f);
  ImGui::DragFloat("Max step", &m_BlackHole->m_StepMax, 0.1f);
  ImGui::DragInt("Max steps", &m_BlackHole->m_MaxSteps);
  ImGui::Dummy(ImVec2(0.0f, 20.0f));

  ImGui::Text("Controls");
  float camera_sensitivity = m_Camera.GetSensitivity();
  if (ImGui::DragFloat("Camera sensitivity", &camera_sensitivity, 0.1f)) {
    m_Camera.SetSensitivity(camera_sensitivity);
  }
  bool camera_inverse_controls = m_Camera.GetInverseCamera();
  if (ImGui::Checkbox("Inverse Camera Controls", &camera_inverse_controls)) {
    m_Camera.SetInverseCamera(camera_inverse_controls);
  }
  ImGui::DragInt("Movement Speed", &m_MovementSpeed);
  auto camera_pos = m_Camera.GetPosition();
  ImGui::Text("Position X: %.2f, Y: %.2f, Z: %.2f", camera_pos.x,
              camera_pos.y, camera_pos.z);
  ImGui::Dummy(ImVec2(0.0f, 20.0f));

  ImGui::Text("Renderer info");
  ImGui::Text("Renderer: %s",
              reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
  ImGui::Text("OpenGL: %s",
              reinterpret_cast<const char *>(glGetString(GL_VERSION)));
  ImGui::End();

  ImGui::Render();

  glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  CheckMovement();

  m_BlackHole->Draw(m_Camera, *m_FinalImage);

  m_FinalImage->Draw(m_Camera);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(m_Window);

  auto curr_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::high_resolution_clock::now().time_since_epoch())
          .count();

  m_DeltaTime = curr_time - m_PrevTime;
  m_PrevTime = curr_time;
}

void Application::Run() {
  if (!m_Window) {
    throw std::runtime_error("Application window is not available.");
  }

#if defined(IS_WEB)
  emscripten_set_main_loop_arg(
      [](void *arg) { static_cast<Application *>(arg)->MainLoop(); }, this, 0,
      1);
#else
  while (!glfwWindowShouldClose(m_Window)) {
    MainLoop();
  }
#endif
}

void Application::Shutdown() {
  glFinish();

  if (m_ImguiInitialized) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    m_ImguiInitialized = false;
  } else if (ImGui::GetCurrentContext()) {
    ImGui::DestroyContext();
  }
  if (m_Window) {
    glfwDestroyWindow(m_Window);
    m_Window = nullptr;
  }
}

void Application::CheckMovement() {
  glm::vec3 move_dir{0.f};

  glm::vec3 forward = m_Camera.GetForward();
  glm::vec3 right = m_Camera.GetRight();
  glm::vec3 up = m_Camera.GetUp();

  if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
    move_dir += forward;

  if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
    move_dir -= forward;

  if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
    move_dir -= right;

  if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
    move_dir += right;

  if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
    move_dir += up;

  if (glfwGetKey(m_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    move_dir -= up;

  if (glm::length(move_dir) > 0.001f) {

    move_dir = glm::normalize(move_dir);

    float dt = m_DeltaTime / 1000.0f;

    glm::vec3 displacement =
        move_dir * static_cast<float>(m_MovementSpeed) * dt;

    m_Camera.Move(displacement);
  }
}

void Application::OnMouseMove(float x, float y) {

  if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
    glm::vec2 delta_pos = {x - m_LastMousePos.x, y - m_LastMousePos.y};

    m_Camera.Rotate(delta_pos.x, delta_pos.y);
  }
}

void Application::OnMouseButton(int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_2) {
    if (action == GLFW_PRESS) {
      glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
      glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }
}

void Application::OnWindowResized(int width, int height) {
  m_Width = width;
  m_Height = height;

  int framebufferWidth = 0;
  int framebufferHeight = 0;
  glfwGetFramebufferSize(m_Window, &framebufferWidth, &framebufferHeight);
  OnFramebufferResized(framebufferWidth, framebufferHeight);
}

void Application::OnFramebufferResized(int width, int height) {
  if (width <= 0 || height <= 0) {
    return;
  }

  m_FramebufferWidth = width;
  m_FramebufferHeight = height;

  glViewport(0, 0, width, height);
  auto viewport =
      glm::vec2{static_cast<float>(width), static_cast<float>(height)};
  m_Camera.OnResize(viewport);
}

void Application::OnContentScaleChanged(float xScale, float yScale) {
  m_ContentScaleX = xScale > 0.0f ? xScale : 1.0f;
  m_ContentScaleY = yScale > 0.0f ? yScale : 1.0f;

  int framebufferWidth = 0;
  int framebufferHeight = 0;
  glfwGetFramebufferSize(m_Window, &framebufferWidth, &framebufferHeight);
  OnFramebufferResized(framebufferWidth, framebufferHeight);
}
} // namespace Sim
