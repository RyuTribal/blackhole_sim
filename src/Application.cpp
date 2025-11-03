#include "Application.hpp"
#include "BlackHole.hpp"
#include <stdexcept>
#include <utility>

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include <glad/gl.h>

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

Application::Application(std::string title, int width, int height)
    : m_Title(std::move(title)), m_Width(width), m_Height(height) {
  Initialize();
}

Application::~Application() { Shutdown(); }

void Application::Initialize() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW.");
  }
  m_GlfwInitialized = true;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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
  if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
    throw std::runtime_error("Failed to initialize GLAD.");
  }

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

  int framebufferWidth = 0;
  int framebufferHeight = 0;
  glfwGetFramebufferSize(m_Window, &framebufferWidth, &framebufferHeight);
  OnFramebufferResized(framebufferWidth, framebufferHeight);

  float xScale = 1.0f;
  float yScale = 1.0f;
  glfwGetWindowContentScale(m_Window, &xScale, &yScale);
  OnContentScaleChanged(xScale, yScale);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::StyleColorsDark();

  if (!ImGui_ImplGlfw_InitForOpenGL(m_Window, true)) {
    throw std::runtime_error("Failed to initialize Dear ImGui GLFW backend.");
  }
  if (!ImGui_ImplOpenGL3_Init("#version 410")) {
    throw std::runtime_error("Failed to initialize Dear ImGui OpenGL backend.");
  }

  m_ImguiInitialized = true;
}

void Application::Run() {
  if (!m_Window) {
    throw std::runtime_error("Application window is not available.");
  }

  glm::vec3 clearColor{0.10f, 0.13f, 0.17f};

  Cubemap skybox{"assets/cubemap"};
  BlackHole black_hole{};

  while (!glfwWindowShouldClose(m_Window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Hello, ImGui");
    ImGui::Text("Welcome to %s", m_Title.c_str());
    ImGui::ColorEdit3("Clear Color", glm::value_ptr(clearColor));
    ImGui::Text("Renderer: %s",
                reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
    ImGui::Text("OpenGL: %s",
                reinterpret_cast<const char *>(glGetString(GL_VERSION)));
    ImGui::End();

    ImGui::Render();

    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    black_hole.Draw(m_Camera);

    skybox.Draw(m_Camera);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_Window);
  }
}

void Application::Shutdown() {
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

  if (m_GlfwInitialized) {
    glfwTerminate();
    m_GlfwInitialized = false;
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
  m_Camera.Update(viewport);
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
