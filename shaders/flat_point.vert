#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float sizeScale;
layout (location = 2) in int vertexIndex;
layout (location = 3) in vec3 normal;

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
	vec2 viewport;
};

flat out int FragVertexIndex;
out vec3 fragWorldPos;


uniform mat4 model;
uniform float pointSize;


flat out float fragZ;


void main()
{

	// Transform to clip space
	vec4 clip = projection * view * model * vec4(aPos, 1.0);

	vec3 viewDir = vec3(view[0][2], view[1][2], view[2][2]);



	// 2D radius
	// gl_PointSize = pointSize * sizeScale;
	gl_PointSize = pointSize * 1.;

	float r2 = gl_PointSize * 0.5;
	// Compute 3D radius
	// To NDC
	vec4 ndc = clip / clip.w;
	// To Screen coords
	vec2 screenPos = (ndc.xy * 0.5 + 0.5) * viewport;
	// Add radius offset
	screenPos.x += r2;

	// Back to world-space
	vec2 ndc2 = screenPos / viewport * 2.0 - 1.0;
	// Inverse of perspective division, return to clip space
	vec4 clip2 = vec4(ndc2 * clip.w, clip.z, clip.w);
	// Back to world space
	vec4 wh2 = inverse(projection * view * model) * clip2;
	// From homogeneous to cartesian
	vec3 w2 = wh2.xyz / wh2.w;
	// Compute radius in world-space
	float r3 = distance(aPos.xyz, w2.xyz);

	// vec4 posOff = vec4(aPos.x, aPos.y, aPos.z - r3, 1.);
	// // To clip space
	// vec4 clipOff = projection * view * model * posOff;
	// // Perspective division
	// vec3 ndcOff = clipOff.xyz / clipOff.w;
	// fragZ = (ndcOff.z * 0.5 + 0.5);
	// vec3 pos = aPos;


	vec3 pos = aPos + dot(normal, viewDir) * viewDir * r3;
	// vec3 pos = aPos + normal * r3;
	// vec3 pos = aPos + dot(normal, viewDir) * normal * r3;


	gl_Position = projection * view * model * vec4(pos, 1.0);
	fragWorldPos = pos;
	FragVertexIndex = vertexIndex;
}