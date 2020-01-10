#version core 330

layout (location=0) in vec3 aPos;

uniform mat4 model;

void main()
{
    gl_Position = model * vec3(aPos, 1.0);
}