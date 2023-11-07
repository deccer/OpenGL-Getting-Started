#version 460 core

layout(location = 0) in vec2 v_uv;

layout(location = 0) out vec4 o_color;

void main()
{
    o_color = vec4(v_uv, 0.0f, 1.0f);
}