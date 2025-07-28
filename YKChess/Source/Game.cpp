#include <chrono>

#include "Core/WindowManager.h"
#include "Core/EventManager.h"
#include "Core/Timestep.h"

namespace yk
{
  class ChessGame : public yk::EventManager
  {
  public:
    void Run()
    {
      while (m_IsRunning)
      {
        yk::Timestep timestep = ChessGame::CalculateTimestep();
        yk::WindowManager::UpdateWindow(timestep);
      }
    }

    void OnWindowClose() final
    {
      m_IsRunning = false;
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
  yk::ChessGame game;
  game.Run();
  yk::WindowManager::Shutdown();
}