#version 450 core

layout(location = 0) in vec2 v_TexCoord;
layout(location = 1) in vec4 v_Color;
layout(location = 2) flat in int v_TexIndex;
layout(location = 3) in float v_TileFactor;

layout(location = 0) out vec4 color;

//layout(binding = 0) uniform sampler2D u_Textures[32];


void main()
{
	color = v_Color;
}