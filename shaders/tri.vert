#version 440 core

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
	vec2 viewport;
};

uniform mat4 model;


out vec3 fragBary;
out vec3 fragNormal;
out vec3 fragHeights;
flat out vec3 flatFragHeights;

flat out int fragFacetIndex;

flat out vec3 fragViewDir;

out vec3 fragBarycentric;
flat out int fragCornerIndex;
flat out int fragCornerOff;


out vec3 fragWorldPos;


uniform float meshShrink;


uniform samplerBuffer bufPoints;
uniform isamplerBuffer bufFacets;

flat out int surfaceType; /* 0 => triangle, 1 => polygon */

void main()
{
	const int facetSize = 3;
 	int fi = gl_VertexID / facetSize;
	int lv = gl_VertexID % 3;

	int v0 = texelFetch(bufFacets, fi * facetSize).x;
	int v1 = texelFetch(bufFacets, fi * facetSize + 1).x;
	int v2 = texelFetch(bufFacets, fi * facetSize + 2).x;

	vec3 p[3] = {
		texelFetch(bufPoints, v0).xyz,
		texelFetch(bufPoints, v1).xyz,
		texelFetch(bufPoints, v2).xyz
	};
	vec3 bary = (p[0] + p[1] + p[2]) / 3.;

	// Apply shrink
	p[0] = p[0] - (p[0] - bary) * meshShrink;
	p[1] = p[1] - (p[1] - bary) * meshShrink;
	p[2] = p[2] - (p[2] - bary) * meshShrink;

	// Compute normal
	vec3 n = normalize(cross(p[1] - p[0], p[2] - p[0]));

	// To clip space for all corners (for height computation)
	vec4 clip[3] = {
		projection * view * model * vec4(p[0], 1.0),
		projection * view * model * vec4(p[1], 1.0),
		projection * view * model * vec4(p[2], 1.0)
	};

	
	gl_Position = clip[lv];

	// Compute facet triangles heights
	// To NDC
	vec2 ndc0 = clip[0].xy / clip[0].w;
	vec2 ndc1 = clip[1].xy / clip[1].w;
	vec2 ndc2 = clip[2].xy / clip[2].w;

	// To screen coords (pixels)
	vec2 s0 = (ndc0 * 0.5 + 0.5) * viewport;
	vec2 s1 = (ndc1 * 0.5 + 0.5) * viewport;
	vec2 s2 = (ndc2 * 0.5 + 0.5) * viewport;

	// Compute triangle heights in screen space (pixels)
	// using Heron's formula:
	// 1. Compute side lengths
	float l[3] = {
		length(s1 - s0),
		length(s2 - s1),
		length(s2 - s0)
	};
	float s = (l[0] + l[1] + l[2]) * 0.5;

	// Max with zero to avoid negative sqrt 
	// (that is possible with degenerated triangles)
	// sqrt(x) with x < 0 gives NaN in glsl
	float arg = max(0.0, s * (s - l[0]) * (s - l[1]) * (s - l[2])); 
	float area = sqrt(arg);

	float h = 2. * area / l[(lv + 1) % 3];

	fragHeights = vec3(0);
	fragHeights[lv] = h;
	flatFragHeights = fragHeights;

	fragBary = bary;
	fragNormal = n;
	fragFacetIndex = fi;
	fragWorldPos = p[lv];

	fragBarycentric = vec3(0.);
	fragBarycentric[lv] = 1.;
	fragCornerIndex = fi * 3;
	fragCornerOff = fi * 3;

	fragViewDir = -vec3(view[0][2], view[1][2], view[2][2]);

	surfaceType = 0;
}