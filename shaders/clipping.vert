#version 440 core
layout(location = 0) in vec3 p; // model-space quad vertex

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
	vec2 viewport;
};


uniform mat4 model;
out vec3 vWorldPos;

void main() {
	vWorldPos = vec3(model * vec4(p, 1.0));
    gl_Position = projection * view * model * vec4(p, 1.0);
}