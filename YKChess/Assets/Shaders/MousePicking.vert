#version 450

layout(binding = 0) uniform UniformBufferObject 
{
  mat4 Projection;
} ubo;

layout(location = 0) in vec3 inPos;
layout(location = 2) in uint inID;

layout(location = 0) out flat uint outID;

void main() 
{
  gl_Position = ubo.Projection * vec4(inPos, 1.0);
  outID = inID;
}