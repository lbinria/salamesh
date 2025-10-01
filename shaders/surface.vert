#version 440 core

// Vertex attributes
layout (location = 0) in vec3 p;
layout (location = 1) in vec3 p0;
layout (location = 2) in vec3 p1;
layout (location = 3) in vec3 p2;
layout (location = 4) in vec3 p3;
layout (location = 5) in vec3 normal;
layout (location = 6) in vec3 aHeights;
// Indexes of the primitive this vertices belongs to
layout (location = 7) in int vertexIndex;
layout (location = 8) in int facetIndex;
layout (location = 9) in int localIndex;

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
};

uniform vec2 uViewport = vec2(2560.0, 1440.0); // viewport size in pixels


out vec3 fragBary;
out vec3 fragNormal;
out vec3 fragHeights;
out float fragVertexIndex;
flat out int fragFacetIndex;

flat out vec3 fragViewDir;


out vec3 fragWorldPos;

uniform mat4 model;

uniform samplerBuffer bary;

uniform float meshShrink;

void main()
{
   vec3 bary = texelFetch(bary, facetIndex).xyz;
   // vec3 bary = (p0 + p1 + p2) / 3.;

   // Shrink
   vec3 sp = p - (p - bary) * meshShrink;
   vec3 sp0 = p0 - (p0 - bary) * meshShrink;
   vec3 sp1 = p1 - (p1 - bary) * meshShrink;
   vec3 sp2 = p2 - (p2 - bary) * meshShrink;
   vec3 sp3 = p3 - (p3 - bary) * meshShrink;

   // Transform to clip space
   gl_Position = projection * view * model * vec4(sp, 1.0);
   
   // To clip space for all corners (for height computation)
   vec4 c0 = projection * view * model * vec4(sp0, 1.0);
   vec4 c1 = projection * view * model * vec4(sp1, 1.0);
   vec4 c2 = projection * view * model * vec4(sp2, 1.0);
   vec4 c3 = projection * view * model * vec4(sp3, 1.0);

   // To NDC
   vec2 ndc0 = c0.xy / c0.w;
   vec2 ndc1 = c1.xy / c1.w;
   vec2 ndc2 = c2.xy / c2.w;
   vec2 ndc3 = c3.xy / c3.w;

    // To screen coords (pixels)
   vec2 s0 = (ndc0 * 0.5 + 0.5) * uViewport;
   vec2 s1 = (ndc1 * 0.5 + 0.5) * uViewport;
   vec2 s2 = (ndc2 * 0.5 + 0.5) * uViewport;
   vec2 s3 = (ndc3 * 0.5 + 0.5) * uViewport;

   // Compute triangle heights in screen space (pixels)
   // using Heron's formula:
   // 1. Compute side lengths
   float a = length(s1 - s0);
   float b = length(s2 - s1);
   float c = length(s2 - s0);
   // float s = (a + b + c) * 0.5;
   float s = (a + b + c);
   float area = sqrt(s * (s - a) * (s - b) * (s - c));
   // float height0 = 2.0 * area / a;
   // float height1 = 2.0 * area / b;
   // float height2 = 2.0 * area / c;
   float h0 = area / (0.5 * a);
   float h1 = area / (0.5 * b);
   float h2 = area / (0.5 * c);
   vec3 heights = vec3(h0, h1, h2);
   
   fragHeights = vec3(0);
   fragHeights[localIndex] = heights[(localIndex + 1) % 3];

		// const double side_lengths[] = {a, b, c};
		// const double s = a + b + c;
		// const double area = sqrt(s * (s - a) * (s - b) * (s - c));
			// const double h = area / (.5 * side_lengths[(lv + 1) % 3]);


   fragBary = bary;
   fragNormal = normal;
   // fragHeights = aHeights;
   fragFacetIndex = facetIndex;
   fragVertexIndex = vertexIndex;
   fragWorldPos = sp;

   fragViewDir = -vec3(view[0][2], view[1][2], view[2][2]);
}