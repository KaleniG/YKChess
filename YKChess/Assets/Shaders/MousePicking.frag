#version 450

layout(location = 0) in flat uint inID;

layout(location = 0) out uint outID;

void main() 
{
  outID = inID;
}