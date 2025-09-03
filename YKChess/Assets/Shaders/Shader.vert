#version 450

layout(binding = 0) uniform UniformBufferObject 
{
  mat4 Projection;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 3) in uint inTexSlot;

layout(location = 0) out vec2 outTextCoord;
layout(location = 1) out flat uint outTexSlot;

void main() 
{
  gl_Position = ubo.Projection * vec4(inPosition, 1.0);
  outTextCoord = inTexCoord;
  outTexSlot = inTexSlot;
}