#version 440 core

// Vertex attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 aHeights;
// Indexes of the primitive this vertices belongs to
layout (location = 3) in int vertexIndex;
layout (location = 4) in int facetIndex;

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
};


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

   // Shrink
   vec3 pos = aPos - (aPos - bary) * meshShrink;

   // Transform to clip space
   gl_Position = projection * view * model * vec4(pos, 1.0);

   fragBary = bary;
   fragNormal = normal;
   fragHeights = aHeights;
   fragFacetIndex = facetIndex;
   fragVertexIndex = vertexIndex;
   fragWorldPos = pos;

   fragViewDir = -vec3(view[0][2], view[1][2], view[2][2]);
}