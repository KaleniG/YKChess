#version 450

layout(binding = 0) uniform UniformBufferObject 
{
  mat4 Projection;
  vec2 MousePos;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec2 mousePos;

void main() 
{
  gl_Position = ubo.Projection * vec4(inPosition, 0.0, 1.0);
  fragTexCoord = inTexCoord;
}