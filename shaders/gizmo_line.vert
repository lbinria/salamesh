#version 440 core

layout (location = 0) in vec3 p;
layout (location = 1) in vec3 color;

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
	vec2 viewport;
};

uniform mat4 model;

out vec3 fragColor;

void main()
{
	gl_Position = projection * view * model * vec4(p, 1.0);
	fragColor = color;
}