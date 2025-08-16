#version 450

layout(location = 0) in flat uint i_ID;

layout(location = 0) out uint o_ID;

void main() 
{
  o_ID = i_ID;
}