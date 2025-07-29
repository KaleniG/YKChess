#include <chrono>

#include "Core/WindowManager.h"
#include "Core/EventManager.h"
#include "Core/Timestep.h"
#include "Rendering/DebugOverlayManager.h"
#include "Rendering/Renderer.h"

namespace yk
{
  class ChessGame : public yk::EventManager
  {
  public:
    ChessGame()
    {
      yk::DebugOverlayManager::Init();
    }

    void Run()
    {
      while (m_IsRunning)
      {
        yk::Timestep timestep = ChessGame::CalculateTimestep();

        if (!m_IsMinimized)
        {
          yk::DebugOverlayManager::Start();

          ChessGame::Update(timestep);

          yk::Renderer::Render();
        }

        yk::WindowManager::UpdateWindow(timestep);
      }
      yk::Renderer::WaitIdle();
    }

    void Update(Timestep timestep)
    {
      ImGui::ShowDemoWindow();
    }

    void OnWindowClose() final
    {
      m_IsRunning = false;
    }

    void OnWindowIconify(bool iconified) final
    {
      m_IsMinimized = iconified;
    }

    yk::Timestep CalculateTimestep()
    {
      static auto lastTime = std::chrono::high_resolution_clock::now();
      auto currentTime = std::chrono::high_resolution_clock::now();
      float time = std::chrono::duration<float>(currentTime - lastTime).count();
      lastTime = currentTime;
      return { time };
    }

  private:
    bool m_IsRunning = true;
    bool m_IsMinimized = false;
  };
}

int main() 
{
  yk::WindowManager::InitWindow(1280, 720, "YK Chess");
  yk::Renderer::Init();
  

  yk::ChessGame game;
  game.Run();

  yk::DebugOverlayManager::Shutdown();
  yk::Renderer::Shutdown();
  yk::WindowManager::Shutdown();
}