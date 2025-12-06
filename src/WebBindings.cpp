#ifdef IS_WEB
#include "Application.hpp"
#include <emscripten/bind.h>

using namespace emscripten;

extern std::unique_ptr<Sim::Application> app;

void SetMass(float mass) {
  if (app && app->GetBlackHole())
    app->GetBlackHole()->SetMass(mass);
}

void SetPosition(float x, float y, float z) {
  if (app && app->GetBlackHole())
    app->GetBlackHole()->m_Position = glm::vec3(x, y, z);
}

void SetShowDisk(bool show) {
  if (app && app->GetBlackHole())
    app->GetBlackHole()->m_ShowDisk = show;
}

void SetDiskParams(float height, float intensity, float alpha, float bend) {
  if (app && app->GetBlackHole()) {
    auto *bh = app->GetBlackHole();
    bh->m_DiskHeight = height;
    bh->m_DiskIntensity = intensity;
    bh->m_DiskAlpha = alpha;
    bh->m_BendFactor = bend;
  }
}

void SetSimParams(float minStep, float maxStep, int maxSteps) {
  if (app && app->GetBlackHole()) {
    auto *bh = app->GetBlackHole();
    bh->m_StepMin = minStep;
    bh->m_StepMax = maxStep;
    bh->m_MaxSteps = maxSteps;
  }
}

EMSCRIPTEN_BINDINGS(my_module) {
  function("SetMass", &SetMass);
  function("SetPosition", &SetPosition);
  function("SetShowDisk", &SetShowDisk);
  function("SetDiskParams", &SetDiskParams);
  function("SetSimParams", &SetSimParams); // <--- Added
}
#endif
