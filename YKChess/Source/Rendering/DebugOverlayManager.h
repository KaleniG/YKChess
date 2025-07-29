#pragma once

#include <memory>

#include "Rendering/EngineComponents/CommandBuffer.h"

namespace yk
{
  class DebugOverlayManager
  {
  public:
    static void Init();
    static void Shutdown();

    static void Start();
    static void Render(std::shared_ptr<CommandBuffer> buffer);

  private:
    DebugOverlayManager() = delete;
    DebugOverlayManager(const DebugOverlayManager&) = delete;
    DebugOverlayManager& operator=(const DebugOverlayManager&) = delete;
    DebugOverlayManager(DebugOverlayManager&&) = delete;
    DebugOverlayManager& operator=(DebugOverlayManager&&) = delete;
  };
}