#version 450

layout(binding = 0, std140) uniform Camera
{
    mat4 View;
    mat4 Projection;
} _38;

layout(location = 1) out vec4 v_Color;
layout(location = 1) in vec4 a_Color;
layout(location = 0) out vec2 v_TexCoord;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 2) out float v_TexIndex;
layout(location = 3) in float a_TexIndex;
layout(location = 3) out float v_TileFactor;
layout(location = 4) in float a_TileFactor;
layout(location = 0) in vec3 Position;

void main()
{
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    v_TileFactor = a_TileFactor;
    gl_Position = (_38.Projection * _38.View) * vec4(Position, 1.0);
}

