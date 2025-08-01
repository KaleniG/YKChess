#include <chrono>
#include <memory>

#include "Core/WindowManager.h"
#include "Core/EventManager.h"
#include "Core/Timestep.h"
#include "Rendering/DebugOverlayManager.h"
#include "Rendering/ImageResource.h"
#include "Rendering/Renderer.h"

namespace yk
{
  class ChessGame : public yk::EventManager
  {
  public:
    ChessGame()
    {
      yk::DebugOverlayManager::Init();

      m_ChessAtlas = ImageResource::Create("Assets/Textures/ChessAtlas.png", 24, 24);
      Renderer::SetImageSlot(1, m_ChessAtlas);

      Renderer::DrawImage(glm::vec2(-0.7f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(-0.5f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(-0.3f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(-0.1f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.1f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.3f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.5f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.7f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));

      Renderer::DrawImage(glm::vec2(-0.7f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(-0.5f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(-0.3f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(-0.1f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.1f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.3f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.5f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.7f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));

      Renderer::DrawImage(glm::vec2(-0.7f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(-0.5f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(-0.3f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(-0.1f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.1f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.3f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.5f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.7f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));

      Renderer::DrawImage(glm::vec2(-0.7f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(-0.5f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(-0.3f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(-0.1f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.1f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.3f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.5f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.7f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));

      Renderer::DrawImage(glm::vec2(-0.7f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(-0.5f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(-0.3f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(-0.1f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.1f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.3f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.5f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.7f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));

      Renderer::DrawImage(glm::vec2(-0.7f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(-0.5f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(-0.3f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(-0.1f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.1f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.3f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.5f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.7f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));

      Renderer::DrawImage(glm::vec2(-0.7f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(-0.5f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(-0.3f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(-0.1f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.1f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.3f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.5f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.7f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));

      Renderer::DrawImage(glm::vec2(-0.7f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(-0.5f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(-0.3f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(-0.1f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.1f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.3f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));
      Renderer::DrawImage(glm::vec2(0.5f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0));
      Renderer::DrawImage(glm::vec2(0.7f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1));

      Renderer::EndBatch();
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

    std::shared_ptr<ImageResource> m_ChessAtlas;
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