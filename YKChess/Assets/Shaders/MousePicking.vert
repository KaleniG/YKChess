#version 450

layout(binding = 0) uniform UniformBufferObject 
{
  mat4 Projection;
  vec2 MousePos;
} ubo;

layout(location = 0) in vec3 i_Pos;
layout(location = 2) in uint i_ID;

layout(location = 0) out flat uint o_ID;

void main() 
{
  gl_Position = ubo.Projection * vec4(i_Pos, 1.0);
  o_ID = i_ID;
}