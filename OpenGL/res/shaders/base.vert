#version 450 core

layout(location = 0) in vec4 position;
uniform float u_LongerTime;

void main()
{
    gl_Position = vec4(position.x, position.y * 10*sin(u_LongerTime), position.za);
};