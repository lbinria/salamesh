#version 440 core

layout (location = 0) in vec3 p;
layout (location = 1) in float sizeScale;
layout (location = 2) in int vertexIndex;

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
	vec2 viewport;
};

uniform mat4 model;

uniform float pointSize;

flat out int FragVertexIndex;
out vec3 fragWorldPos;
flat out float depthZ;

void main()
{
	vec3 pos = p;

	// To clip space
	mat4 pvm = projection * view * model;
	gl_Position = pvm * vec4(pos, 1.0);

	vec3 viewDir = -vec3(view[0][2], view[1][2], view[2][2]);

	gl_PointSize = pointSize * sizeScale;

	// Compute radius in world space from point size

	// Compute 2D pos of center of point in pixel
	vec4 ndc = gl_Position / gl_Position.w;
	vec2 screenPos = (ndc.xy * 0.5 + 0.5) * viewport;

	// Add offset
	screenPos.x += gl_PointSize * .5;

	// Back to NDC
	vec2 ndc2 = screenPos / viewport * 2.0 - 1.0;
	// Use clip z and w from point center
	float clipW = gl_Position.w;
	float clipZ = gl_Position.z;
	// Inverse of perspective division, return to clip space
	vec4 clip2 = vec4(ndc2 * clipW, clipZ, clipW);
	// Back to world space (homogeneous coordinates)
	vec4 worldH2 = inverse(pvm) * clip2;
	// Back to world space (cartesian coordinates)
	vec3 world2 = worldH2.xyz / worldH2.w;
	// Compute distance
	float d = distance(world2, pos);
	world2 = pos - d * viewDir;

	vec4 clip3 = pvm * vec4(world2, 1.0);
	float ndcZ = clip3.z / clip3.w; // range [-1,1]
	depthZ = ndcZ * 0.5 + 0.5;

	fragWorldPos = pos;
	FragVertexIndex = vertexIndex;
}