#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aTexCoord;
layout (location = 3) in vec3 aHeights;
layout (location = 4) in float facetIndex;

out vec3 ourColor;
out float TexCoord;
out vec3 heights;
out float ourFacetIndex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragWorldPos; // Output to fragment shader

void main()
{
   fragWorldPos = vec3(model * vec4(aPos, 1.0)); // Transform to world space

   gl_Position = projection * view * model * vec4(aPos, 1.0);

   ourColor = aColor;
   TexCoord = aTexCoord;
   heights = aHeights;
   ourFacetIndex = facetIndex;
}