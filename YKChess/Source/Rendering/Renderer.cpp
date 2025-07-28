#include "YKLib.h"

#include "Rendering/Renderer.h"
#include "Rendering/Utils.h"

namespace yk
{
  void Renderer::Init()
  {
    YK_ASSERT(!Renderer::IsInitialized(), "The renderer has already been initialized");

    Renderer::Get().s_Device = std::shared_ptr<Device>();

    Renderer::Get().s_Initialized = true;
  }

  void Renderer::Shutdown()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
  }

  void Renderer::DrawImage(glm::vec2 position, glm::vec2 size, std::shared_ptr<ImageResource> image)
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
  }

  void Renderer::DrawText(const std::string& text, glm::vec2 size, glm::vec2 position, glm::vec4 color, std::shared_ptr<FontResource> font)
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
  }

  void Renderer::RenderBatch()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");

  }

  bool Renderer::IsInitialized()
  {
    return Renderer::Get().s_Initialized;
  }

  std::shared_ptr<yk::Device> Renderer::GetDevice()
  {
    YK_ASSERT(Renderer::IsInitialized(), "The renderer hasn't been initialized yet");
    return Renderer::Get().s_Device;
  }

}