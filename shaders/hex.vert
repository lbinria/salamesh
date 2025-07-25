#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float size;
layout (location = 6) in vec3 normal;
layout (location = 3) in vec3 aHeights;
// Indexes of the primitive this vertices belongs to
layout (location = 4) in int facetIndex;
layout (location = 5) in int cellIndex;
layout (location = 7) in int vertexIndex;

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
};


out vec3 fragBary;
out vec3 fragNormal;
out vec3 fragHeights;
flat out int fragCellIndex;
flat out int fragFacetIndex;
out float fragVertexIndex;
flat out vec3 fragViewDir;


flat out vec3 fragWorldPos;
out vec3 fragWorldPos2;

uniform mat4 model;

uniform int attr_element = -1;

uniform samplerBuffer bary;

// Highlights
uniform samplerBuffer highlight;
out float fragHighlight;
// Filters
uniform samplerBuffer _filter;
out float fragFilter;

uniform float meshShrink;

void main()
{
   vec3 bary = texelFetch(bary, cellIndex).xyz;

   // Shrink
   vec3 pos = aPos - (aPos - bary) * meshShrink;

   // Transform to clip space
   gl_Position = projection * view * model * vec4(pos, 1.0);

   fragHighlight = texelFetch(highlight, cellIndex).x;
   fragFilter = texelFetch(_filter, cellIndex).x;


   fragBary = bary;
   fragNormal = normal;
   fragHeights = aHeights;
   fragCellIndex = cellIndex;
   fragFacetIndex = facetIndex;
   fragVertexIndex = vertexIndex;
   fragWorldPos = pos;
   fragWorldPos2 = pos;

   fragViewDir = -vec3(view[0][2], view[1][2], view[2][2]);
   // fragViewDir = vec3(view[2][0], view[2][1], view[2][2]);

}