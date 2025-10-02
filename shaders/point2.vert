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

uniform vec2 uViewport = vec2(2560.0, 1440.0); // viewport size in pixels
flat out float pointDepth;

void main()
{

	vec3 pos = aPos;

	// Transform to clip space
	gl_Position = projection * view * model * vec4(pos, 1.0);
	gl_PointSize = pointSize * sizeScale;

	// Compute radius in world space from point size

	// Compute 2D pos in pixel of center of point
	vec4 ndc = gl_Position / gl_Position.w;
	vec2 screenPos = (ndc.xy * 0.5 + 0.5) * uViewport;
	screenPos += vec2(gl_PointSize * 0.5);

	// Back to NDC
	vec2 ndc2 = screenPos / uViewport * 2.0 - 1.0;
	// Reconstruct clip z and w roughly by lerping
	float clipW = gl_Position.w;
	float clipZ = gl_Position.z;
	// Inverse of perspective division
	vec4 pos2 = vec4(ndc2 * clipW, clipZ, clipW);
	// Back to world space
	vec4 pos_world = inverse(projection * view * model) * pos2;
	pointDepth = (length(pos - pos_world.xyz) * 0.5) / 100000.0;
	// pointDepth = distance(pos, pos_world.xyz) * 0.5;


	fragWorldPos = pos;
	FragVertexIndex = vertexIndex;
}