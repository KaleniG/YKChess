#version 450

layout(binding = 1) uniform sampler2D texSampler1;
layout(binding = 2) uniform sampler2D texSampler2;

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in flat uint inTexSlot;

layout(location = 0) out vec4 outColor;

void main() 
{
  if (inTexSlot == 1)
    outColor = texture(texSampler1, inTexCoord);
  if (inTexSlot == 2)
    outColor = texture(texSampler2, inTexCoord);
}