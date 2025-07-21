#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float sizeScale;

layout (location = 5) in int cellIndex;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float pointSize;

uniform samplerBuffer bary;
uniform samplerBuffer attributeData;

uniform float meshShrink;


void main()
{
	vec3 bary = texelFetch(bary, cellIndex).xyz;

	// Shrink
	vec3 pos = aPos - (aPos - bary) * meshShrink;

	// Transform to clip space
	gl_Position = projection * view * model * vec4(pos, 1.0);
	gl_PointSize = pointSize * sizeScale;
    
}