// Quad vertex computation
// Can be adapted easily to compute any Polygon with a regular number of verts per facet
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

out vec3 fragColor;

flat out int surfaceType; /* 0 => triangle, 1 => polygon */


void main()
{
	const int nverts = 4; // n verts in facet
	const int npts = nverts * 3;  // n points in facet 4 tri * 3 points per tri
	int fi = gl_VertexID / npts; // Retrieve current quad facet index

	// Retrieve local point in facet \in [0, 11]
	const int lp = gl_VertexID % npts;
	// Retrieve local tri in facet \in [0, 3] (int divide)
	const int lt = lp / 3;

	// Get verts of quad facet fi
	int v[4] = {
		texelFetch(bufFacets, fi * nverts).x,
		texelFetch(bufFacets, fi * nverts + 1).x,
		texelFetch(bufFacets, fi * nverts + 2).x,
		texelFetch(bufFacets, fi * nverts + 3).x
	};

	vec3 pq[4] = {
		texelFetch(bufPoints, v[0]).xyz,
		texelFetch(bufPoints, v[1]).xyz,
		texelFetch(bufPoints, v[2]).xyz,
		texelFetch(bufPoints, v[3]).xyz
	};

	vec3 bary = (pq[0] + pq[1] + pq[2] + pq[3]) / 4.;

	// Local vertex in current tri in facet \in [0,2]
	// const int lvt = lp % 3; // Can be replaced by gl_VertexID % 3 it's equivalent
	const int lvt = gl_VertexID % 3; // Can be replaced by gl_VertexID % 3 it's equivalent

	mat4 pvm = projection * view * model;

	// Current point
	vec3 p = lvt == 0 ? bary : pq[(lvt + lt - 1) % 4];
	p = p - (p - bary) * meshShrink;

	gl_Position = pvm * vec4(p, 1.);

	fragColor = vec3(1., 0, 0);

	// Get verts of current tri in facet
	vec3 p0 = bary;
	vec3 p1 = pq[lt];
	vec3 p2 = pq[(lt + 1) % 4];

	// Compute normal
	vec3 n = vec3(0);
	for (int i = 0; i < nverts; ++i) {
		n += cross(
			pq[i]-bary,
			pq[(i + 1) % nverts]-bary
		);
	}
	n = normalize(n);

	// Compute height
	float h = 0;

	// Compute triangle height for from barycenter (point is bary when lvt=0)
	if (lvt == 0) {

		// Apply shrink (no need to apply on bary as shrinking to bary...)
		p1 = p1 - (p1 - bary) * meshShrink;
		p2 = p2 - (p2 - bary) * meshShrink;
		
		// To clip space
		vec4 clip0  = pvm * vec4(p0, 1.0);
		vec4 clip1  = pvm * vec4(p1, 1.0);
		vec4 clip2  = pvm * vec4(p2, 1.0);

		// To NDC
		vec2 ndc0 = clip0.xy / clip0.w;
		vec2 ndc1 = clip1.xy / clip1.w;
		vec2 ndc2 = clip2.xy / clip2.w;

		// To screen coords (pixels)
		vec2 scr0 = (ndc0 * 0.5 + 0.5) * viewport;
		vec2 scr1 = (ndc1 * 0.5 + 0.5) * viewport;
		vec2 scr2 = (ndc2 * 0.5 + 0.5) * viewport;

		// Compute triangle heights in screen space (pixels)
		// using Heron's formula:
		// 1. Compute side lengths
		float a = length(scr1 - scr0);
		float b = length(scr2 - scr1);
		float c = length(scr2 - scr0);
		float s = (a + b + c) * 0.5;
		
		// Max with zero to avoid negative sqrt 
		// (that is possible with degenerated triangles)
		// sqrt(x) with x < 0 gives NaN in glsl
		float arg = max(0.0, s * (s - a) * (s - b) * (s - c)); 
		float area = sqrt(arg);

		h = 2. * area / b;
		// h = 1.;

	} else {
		// inf
		h = 1. / 0.;
	}

	// For mesh computation
	fragHeights = vec3(0.);
	fragHeights[lvt] = h;
	flatFragHeights = fragHeights;

	// For corner color interpolation computation
	fragCornerIndex = lt;
	fragCornerOff = fi * nverts;
	fragBarycentric = vec3(0.);
	fragBarycentric[lvt] = 1.;

	// Other info
	fragWorldPos = p;
	fragBary = p0;
	fragNormal = n;
	fragFacetIndex = fi;

	// For light orientation
	fragViewDir = -vec3(view[0][2], view[1][2], view[2][2]);

	surfaceType = 1;
}