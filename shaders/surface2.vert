#version 440 core

// Vertex attributes
layout (location = 0) in vec3 p;
layout (location = 1) in vec3 n;
layout (location = 2) in vec3 b;
layout (location = 3) in vec3 h;
// Indexes of the primitive this vertices belongs to
layout (location = 4) in int vertexIndex;
layout (location = 5) in int localIndex;
layout (location = 6) in int facetIndex;

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
	vec2 viewport;
};


out vec3 fragBary;
out vec3 fragNormal;
out vec3 fragHeights;
out float fragVertexIndex;
flat out int fragFacetIndex;

flat out vec3 fragViewDir;


out vec3 fragWorldPos;

uniform mat4 model;

uniform float meshShrink;



void main()
{


   // Apply shrink
   vec3 sp = p - (p - b) * meshShrink;

   // Transform to clip space
   gl_Position = projection * view * model * vec4(sp, 1.0);
   
   // Compute facet triangles heights

   // To clip space for all corners (for height computation)
   vec4 c = projection * view * model * vec4(sp, 1.0);

   // To NDC
   vec2 ndc = c.xy / c.w;

    // To screen coords (pixels)
   vec2 s = (ndc * 0.5 + 0.5) * viewport;



   vec3 sph = h - (h - b) * meshShrink;
   vec4 ch = projection * view * model * vec4(sph, 1.0);
   vec2 ndcH = ch.xy / ch.w;
   vec2 sh = (ndcH * 0.5 + 0.5) * viewport;

   vec3 heights = vec3(0.);
   // if (localIndex == 0)
      heights[localIndex] = distance(s, sh);
   // else 
      // heights[localIndex] = 1. / 0.;
   
   fragHeights = heights;
   

   fragBary = b;
   fragNormal = n;
   fragFacetIndex = facetIndex;
   fragVertexIndex = vertexIndex;
   fragWorldPos = sp;

   fragViewDir = -vec3(view[0][2], view[1][2], view[2][2]);
}