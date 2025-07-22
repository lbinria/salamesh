#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float sizeScale;
layout (location = 2) in int vertexIndex;
layout (location = 3) in int cellIndex;

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
};

flat out int FragVertexIndex;

uniform mat4 model;
uniform float pointSize;

uniform samplerBuffer bary;
uniform samplerBuffer attributeData;

out float fragAttrVal;

uniform float meshShrink;


void main()
{
	vec3 bary = texelFetch(bary, cellIndex).xyz;
	fragAttrVal = texelFetch(attributeData, vertexIndex).x;


	// Shrink
	vec3 pos = aPos - (aPos - bary) * meshShrink;

	// Transform to clip space
	gl_Position = projection * view * model * vec4(pos, 1.0);
	gl_PointSize = pointSize * sizeScale;

	FragVertexIndex = vertexIndex;
}