#version 440 core

// Vertex attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in float size;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 aHeights;

// Indexes of the primitive this vertices belongs to
layout (location = 4) in int vertexIndex;
layout (location = 5) in int facetIndex;
layout (location = 6) in int cellIndex;

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
flat out int fragCellIndex;

flat out vec3 fragViewDir;


out vec3 fragWorldPos;

uniform mat4 model;

uniform samplerBuffer bary;

uniform float meshShrink;

void main()
{
   vec3 bary = texelFetch(bary, cellIndex).xyz;

   // Shrink
   vec3 pos = aPos - (aPos - bary) * meshShrink;

   // Transform to clip space
   gl_Position = projection * view * model * vec4(pos, 1.0);



   fragBary = bary;
   fragNormal = normal;
   fragHeights = aHeights;
   fragCellIndex = cellIndex;
   fragFacetIndex = facetIndex;
   fragVertexIndex = vertexIndex;
   fragWorldPos = pos;

   fragViewDir = -vec3(view[0][2], view[1][2], view[2][2]);
}