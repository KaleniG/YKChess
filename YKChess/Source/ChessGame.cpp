#include <algorithm>
#include <chrono>
#include <memory>

#include <glm/glm.hpp>

#include "Core/WindowManager.h"
#include "Core/EventManager.h"
#include "Core/Timestep.h"
#include "GameLogic/Chess/Game.h"
#include "Rendering/DebugOverlayManager.h"
#include "Rendering/Renderer.h"

namespace yk
{
  class ChessGame : public EventManager
  {
  public:
    ChessGame()
    {
      DebugOverlayManager::Init();
      m_ChessGame = Chess::Game::Create();
    }

    void Run()
    {
      while (m_IsRunning)
      {
        Timestep timestep = ChessGame::CalculateTimestep();

        if (!m_IsMinimized)
        {
          DebugOverlayManager::Start();

          ChessGame::Update(timestep);

          Renderer::Render();
        }

        WindowManager::UpdateWindow(timestep);
      }
      Renderer::WaitIdle();
    }

    void Update(Timestep timestep)
    {
      
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

    std::unique_ptr<Chess::Game> m_ChessGame;
  };
}

int main() 
{
  yk::WindowManager::InitWindow(1280, 720, "YK Chess");
  yk::WindowManager::SetWindowIcon
    ({ 
    "Assets/Icons/YKChess_x1.png", 
    "Assets/Icons/YKChess_x2.png", 
    "Assets/Icons/YKChess_x3.png", 
    "Assets/Icons/YKChess_x4.png" 
    });
  yk::Renderer::Init();
  
  yk::ChessGame game;
  game.Run();

  yk::DebugOverlayManager::Shutdown();
  yk::Renderer::Shutdown();
  yk::WindowManager::Shutdown();
}