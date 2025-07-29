#version 450

layout(binding = 0) uniform UniformBufferObject 
{
  vec2 mousePos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec2 mousePos;

void main() 
{

  //vec4 worldPos = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
  //gl_Position = worldPos;
  //fragColor = inColor;
  //fragTexCoord = inTexCoord;
}