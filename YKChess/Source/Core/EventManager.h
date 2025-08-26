#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Core/KeyCodes.h"
#include "Core/MouseCodes.h"

namespace yk
{
  class EventManager
  {
  public:
    EventManager();
    virtual ~EventManager();

    virtual void OnWindowClose() {}
    virtual void OnWindowIconify(bool iconified) {}
    virtual void OnWindowMaximize(bool maximized) {}
    virtual void OnWindowFocus(bool focused) {}
    virtual void OnWindowResize(int32_t width, int32_t height) {}
    virtual void OnWindowMove(int32_t xpos, int32_t ypos) {}
    virtual void OnFramebufferResize(int32_t width, int32_t height) {}
    virtual void OnKeyPress(KeyCode key, bool repeat) {}
    virtual void OnKeyRelease(KeyCode key) {}
    virtual void OnCharacterType(char32_t character) {}
    virtual void OnMouseMove(double xpos, double ypos) {}
    virtual void OnMouseButtonPress(MouseCode button) {}
    virtual void OnMouseButtonRelease(MouseCode button) {}
    virtual void OnMouseScroll(double xoffset, double yoffset) {}

    glm::uvec2 MouseNormalizedToPixel(double xpos, double ypos);
  private:
    void UpdateCallbacks();

  private:
    static std::vector<EventManager*> s_GlobalEventManagers;
  };
}