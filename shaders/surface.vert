#version 440 core

// Vertex attributes
layout (location = 1) in vec3 p0;
layout (location = 2) in vec3 p1;
layout (location = 3) in vec3 p2;
// Indexes of the primitive this vertices belongs to
// layout (location = 5) in int vertexIndex;
layout (location = 4) in int facetIndex;
layout (location = 5) in int localIndex;
layout (location = 6) in int cornerIndex;


layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
   vec2 viewport;
};


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

uniform mat4 model;

uniform float meshShrink;

flat out int surfaceType; /* 0 => triangle, 1 => polygon */


void main()
{
   vec3 bary = (p0 + p1 + p2) / 3.;
   vec3 n = normalize(cross(p1 - p0, p2 - p0));

   // Apply shrink
   vec3 sp0 = p0 - (p0 - bary) * meshShrink;
   vec3 sp1 = p1 - (p1 - bary) * meshShrink;
   vec3 sp2 = p2 - (p2 - bary) * meshShrink;

   vec3 points[3] = vec3[3](sp0, sp1, sp2);
   vec3 sp = points[localIndex];

   // Transform to clip space
   gl_Position = projection * view * model * vec4(sp, 1.0);
   
   // Compute facet triangles heights

   // To clip space for all corners (for height computation)
   vec4 c0 = projection * view * model * vec4(sp0, 1.0);
   vec4 c1 = projection * view * model * vec4(sp1, 1.0);
   vec4 c2 = projection * view * model * vec4(sp2, 1.0);

   // To NDC
   vec2 ndc0 = c0.xy / c0.w;
   vec2 ndc1 = c1.xy / c1.w;
   vec2 ndc2 = c2.xy / c2.w;

    // To screen coords (pixels)
   vec2 s0 = (ndc0 * 0.5 + 0.5) * viewport;
   vec2 s1 = (ndc1 * 0.5 + 0.5) * viewport;
   vec2 s2 = (ndc2 * 0.5 + 0.5) * viewport;

   // Compute triangle heights in screen space (pixels)
   // using Heron's formula:
   // 1. Compute side lengths
   float a = length(s1 - s0);
   float b = length(s2 - s1);
   float c = length(s2 - s0);
   float s = (a + b + c) * 0.5;
   
   // Max with zero to avoid negative sqrt 
   // (that is possible with degenerated triangles)
   // sqrt(x) with x < 0 gives NaN in glsl
   float arg = max(0.0, s * (s - a) * (s - b) * (s - c)); 
   float area = sqrt(arg);

   float h0 = 2. * area / a;
   float h1 = 2. * area / b;
   float h2 = 2. * area / c;

   vec3 heights = vec3(h0, h1, h2);
   
   fragHeights = vec3(0);
   fragHeights[localIndex] = heights[(localIndex + 1) % 3];
   flatFragHeights = fragHeights;


   fragBary = bary;
   fragNormal = n;
   fragFacetIndex = facetIndex;
   fragWorldPos = sp;

   fragBarycentric = vec3(0.);
   fragBarycentric[localIndex] = 1.;
   fragCornerIndex = cornerIndex;
   fragCornerOff = facetIndex * 3;

   fragViewDir = -vec3(view[0][2], view[1][2], view[2][2]);

   surfaceType = 0;
}