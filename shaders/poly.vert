#version 440 core

// Vertex attributes
layout (location = 0) in vec3 p;
layout (location = 1) in vec3 p0; // bary
layout (location = 2) in vec3 p1; 
layout (location = 3) in vec3 p2; 
layout (location = 4) in vec3 n;
// Indexes of the primitive this vertices belongs to
layout (location = 5) in int vertexIndex;
layout (location = 6) in int localIndex;
layout (location = 7) in int cornerIndex;
layout (location = 8) in int facetIndex;

uniform mat4 model;

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
out float fragVertexIndex;
flat out int fragFacetIndex;

flat out vec3 fragViewDir;

out vec3 fragBarycentric;
flat out int fragCornerIndex;


out vec3 fragWorldPos;


uniform float meshShrink;

flat out int surfaceType; /* 0 => triangle, 1 => polygon */

void main()
{
   // Apply shrink
   vec3 world = p - (p - p0) * meshShrink;
   mat4 pvm = projection * view * model;

   // To clip space
   vec4 clip = pvm * vec4(world, 1.0);
   gl_Position = clip;

   float h = 0; // TODO compute h

   if (localIndex == 0) {
      // Compute triangle height for current ref point
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

   } else {
      // inf
      h = 1. / 0.;
   }

   fragHeights = vec3(0.);
   fragHeights[localIndex] = h;
   flatFragHeights = fragHeights;
   
   fragBarycentric = vec3(0.);
   fragBarycentric[localIndex] = 1.;
   fragCornerIndex = cornerIndex;

   fragBary = p0;
   fragNormal = n;
   fragFacetIndex = facetIndex;
   fragVertexIndex = vertexIndex;

   fragWorldPos = world;


   fragViewDir = -vec3(view[0][2], view[1][2], view[2][2]);

   surfaceType = 1;
}