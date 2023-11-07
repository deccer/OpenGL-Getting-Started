#version 460 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_uv;

layout (location = 0) out gl_PerVertex
{
    vec4 gl_Position;
};
layout(location = 0) out vec2 v_uv;

void main()
{
    gl_Position = vec4(i_position, 1.0);
    v_uv = i_uv;
}