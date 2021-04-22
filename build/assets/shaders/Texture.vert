#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_TexIndex;
layout(location = 4) in float a_TileFactor;

layout(set = 0, binding = 0) uniform mat4 u_ViewProjection;

layout(location = 0) out vec2 v_TexCoord;
layout(location = 1) out vec4 v_Color;
layout(location = 2) flat out int v_TexIndex;
layout(location = 3) out float v_TileFactor;

void main()
{

	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_TileFactor = a_TileFactor;
	gl_Position =  u_ViewProjection *
	vec4(a_Position, 1.0);
}