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
uniform isamplerBuffer bufFacetIndexes;
uniform isamplerBuffer bufOffsets;

out vec3 fragColor;

void main()
{
	int fi = texelFetch(bufFacetIndexes, gl_VertexID).x;
   int soff = texelFetch(bufOffsets, fi).x;
	int off = texelFetch(bufOffsets, fi + 1).x;
	int nverts = off - soff;
	int npts = nverts * 3;

	vec3 bary = vec3(0);
	for (int i = 0; i < nverts; ++i) {
		// Get verts of facet fi
		int vi = texelFetch(bufFacets, soff + i).x;
		vec3 p = texelFetch(bufPoints, vi).xyz;
		bary += p;
	}

	bary /= nverts;


	// Retrieve local point in facet (int divide)
	int lp = gl_VertexID - soff * 3;
	// Retrieve local tri in facet (int divide)
	int lt = lp / 3;

	int lvt = lp % 3;
	// int lv = lvt == 0 ? -1 : (lvt + lt - 1) % nverts;
	// vec3 p = lv == -1 ? bary : texelFetch(bufPoints, lv).xyz; // point to process
   vec3 p = lvt == 0 ? bary : texelFetch(bufPoints, (lvt + lt - 1) % nverts).xyz; // point to process


	gl_Position = projection * view * model * vec4(p, 1.);
	fragColor = vec3(0.0, 0.32, 1.0);

   // vec3 p0 = bary;
   // vec3 p1 = p[lv];
   // vec3 p2 = p[(lv + 1) % nverts];

   // int cornerIndex = fi * nverts;

   // vec3 n = normalize(cross(p1 - p0, p2 - p0));


   // // Apply shrink
   // vec3 sp0 = p0 - (p0 - bary) * meshShrink;
   // vec3 sp1 = p1 - (p1 - bary) * meshShrink;
   // vec3 sp2 = p2 - (p2 - bary) * meshShrink;

   // vec3 points[3] = vec3[3](sp0, sp1, sp2);
   // vec3 sp = points[lv];

   // // Transform to clip space
   // gl_Position = projection * view * model * vec4(sp, 1.0);
   
   // // Compute facet triangles heights

   // // To clip space for all corners (for height computation)
   // vec4 c0 = projection * view * model * vec4(sp0, 1.0);
   // vec4 c1 = projection * view * model * vec4(sp1, 1.0);
   // vec4 c2 = projection * view * model * vec4(sp2, 1.0);

   // // To NDC
   // vec2 ndc0 = c0.xy / c0.w;
   // vec2 ndc1 = c1.xy / c1.w;
   // vec2 ndc2 = c2.xy / c2.w;

   //  // To screen coords (pixels)
   // vec2 s0 = (ndc0 * 0.5 + 0.5) * viewport;
   // vec2 s1 = (ndc1 * 0.5 + 0.5) * viewport;
   // vec2 s2 = (ndc2 * 0.5 + 0.5) * viewport;

   // // Compute triangle heights in screen space (pixels)
   // // using Heron's formula:
   // // 1. Compute side lengths
   // float a = length(s1 - s0);
   // float b = length(s2 - s1);
   // float c = length(s2 - s0);
   // float s = (a + b + c) * 0.5;
   
   // // Max with zero to avoid negative sqrt 
   // // (that is possible with degenerated triangles)
   // // sqrt(x) with x < 0 gives NaN in glsl
   // float arg = max(0.0, s * (s - a) * (s - b) * (s - c)); 
   // float area = sqrt(arg);

   // float h0 = 2. * area / a;
   // float h1 = 2. * area / b;
   // float h2 = 2. * area / c;

   // vec3 heights = vec3(h0, h1, h2);
   
   // fragHeights = vec3(0);
   // fragHeights[lv] = heights[(lv + 1) % 3];
   // flatFragHeights = fragHeights;


   // fragBary = bary;
   // fragNormal = n;
   // fragFacetIndex = fi;
   // fragWorldPos = sp;

   // fragBarycentric = vec3(0.);
   // fragBarycentric[lv] = 1.;
   // fragCornerIndex = cornerIndex;
   // fragCornerOff = fi * 3;

   // fragViewDir = -vec3(view[0][2], view[1][2], view[2][2]);

   // surfaceType = 0;
}