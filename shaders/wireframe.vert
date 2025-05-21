#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float size;
layout (location = 3) in vec3 aHeights;
// Indexes of the primitive this vertices belongs to
// layout (location = 5) in int cellIndex;

// out vec3 fragHeights;
// flat out int fragCellIndex;
// flat out int fragFacetIndex;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// uniform int attr_element = -1;
// out float fragAttrVal;

// uniform samplerBuffer bary;
// uniform samplerBuffer attributeData;

// // Highlights
// uniform samplerBuffer highlight;
// out float fragHighlight;
// // Filters
// uniform samplerBuffer _filter;
// out float fragFilter;

// uniform float meshShrink;

void main()
{
   // vec3 bary = texelFetch(bary, cellIndex).xyz;

   // Shrink
   // vec3 pos = aPos - (aPos - bary) * meshShrink;
   vec3 pos = aPos;

   // Transform to clip space
   gl_Position = projection * view * model * vec4(pos, 1.0);

   // fragAttrVal = texelFetch(attributeData, cellIndex).x;
   // fragHighlight = texelFetch(highlight, cellIndex).x;
   // fragFilter = texelFetch(_filter, cellIndex).x;

   // fragHeights = aHeights;
   // fragCellIndex = cellIndex;
}