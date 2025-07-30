#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float sizeScale;
layout (location = 2) in int vertexIndex;

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
};

flat out int FragVertexIndex;
out vec3 fragWorldPos;


uniform mat4 model;
uniform float pointSize;

void main()
{

	vec3 pos = aPos;

	// Transform to clip space
	gl_Position = projection * view * model * vec4(pos, 1.0);
	gl_PointSize = pointSize * sizeScale;

	fragWorldPos = pos;
	FragVertexIndex = vertexIndex;
}