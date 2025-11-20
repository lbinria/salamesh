#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float sizeScale;
layout (location = 2) in int vertexIndex;

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
	vec2 viewport;
};

flat out int FragVertexIndex;
out vec3 fragWorldPos;
flat out vec3 fragViewDir;


uniform mat4 model;
uniform float pointSize;

// uniform vec2 viewport = vec2(2560.0, 1440.0); // viewport size in pixels
flat out float pointDepth;
flat out float pointZ;

// int pointSizeUnit = 0; // 0 = pixel, 1 = world space

void main()
{

	vec3 pos = aPos;

	// Transform to clip space
	gl_Position = projection * view * model * vec4(pos, 1.0);

	gl_PointSize = pointSize * sizeScale;

	// Compute radius in world space from point size

	// Compute 2D pos in pixel of center of point
	vec4 ndc = gl_Position / gl_Position.w;
	vec2 screenPos = (ndc.xy * 0.5 + 0.5) * viewport;


	// Add offset
	screenPos += vec2(gl_PointSize * 0.5);
	// Back to NDC
	vec2 ndc2 = screenPos / viewport * 2.0 - 1.0;
	// Reconstruct clip z and w roughly by lerping
	float clipW = gl_Position.w;
	float clipZ = gl_Position.z;
	// Inverse of perspective division, return to clip space
	vec4 clipNPos = vec4(ndc2 * clipW, clipZ, clipW);
	// Back to world space
	vec4 worldNPos = inverse(projection * view * model) * clipNPos;
	

	float d = length(worldNPos.xyz - pos);
	worldNPos.z -= d;
	// To clip space
	vec4 clipNPos2 = projection * view * model * worldNPos;
	// Perspective division
	vec3 ndc3 = clipNPos2.xyz / clipNPos2.w;
	pointZ = (ndc3.z * 0.5 + 0.5);

    fragViewDir = -vec3(view[0][2], view[1][2], view[2][2]);
	fragWorldPos = pos;
	FragVertexIndex = vertexIndex;
}