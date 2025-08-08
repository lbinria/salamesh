#version 440 core

// Primitive indexation
flat in int fragCellIndex;
flat in int fragFacetIndex;
in float fragVertexIndex;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 fragFacetIndexOut;
layout(location = 2) out vec4 fragCellIndexOut;

in vec3 fragBary;
in vec3 fragNormal;
in vec3 fragHeights;

flat in vec3 fragViewDir;

in vec3 fragWorldPos;

uniform bool is_light_enabled;
uniform bool is_light_follow_view;

uniform int fragRenderMode = 0;
uniform int fragRenderMeshMode = 2;
uniform float meshSize;

uniform int clipping_mode = 0; // 0: cell, 1: std, 2: slice
uniform bool is_clipping_enabled = false;
uniform vec3 clipping_plane_normal = vec3(0.2f, 0.6f, 0.0f); // (a, b, c)
uniform vec3 clipping_plane_point = vec3(0.0f, 0.0f, 0.0);  // A point on the plane
uniform int invert_clipping = 0; // 0: normal, 1: inverted

uniform sampler1D fragColorMap;
uniform vec2 attrRange = vec2(0.f, 1.f);
uniform samplerBuffer attributeData;
uniform int attrElement;

uniform samplerBuffer filterBuf;
uniform samplerBuffer highlightBuf;
uniform samplerBuffer facetHighlightBuf;

uniform int colorMode = 0;
uniform vec3 color;

vec3 encode_id(int id) {
    int r = id & 0x000000FF;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    return vec3(r / 255.f, g / 255.f, b / 255.f); 
}

void main()
{

    // Initialize color to black
    vec3 col = color;


    // Check if cell is filtered
    bool isFiltered = texelFetch(filterBuf, fragCellIndex).x >= .5;
    if (isFiltered) {
        discard;
    }

    /* --- CLIPPING --- */

   // Calculate the distance from the cell barycenter to the plane
   if (is_clipping_enabled) {
    vec3 ref_point;
    if (clipping_mode == 0) {
        // Use the barycenter of the cells to exclude cells
        // that are behind the clipping plane
        ref_point = fragBary;
    } else if (clipping_mode == 1) {
        // Use the fragment world position (interpolated) to exclude fragments
        // that are behind the clipping plane
        ref_point = fragWorldPos;
    }

      float distance = dot(clipping_plane_normal, ref_point - clipping_plane_point) / length(clipping_plane_normal);
      
      if ((invert_clipping == 0 && distance < 0.0) || (invert_clipping == 1 && distance >= 0.0)) {
         discard;
      }
   }

    /* --- CLIPPING END --- */

    /* --- FILTER END --- */

    if (colorMode == 1) {

        int primitiveIndex;
        // Cell facet attribute
        if (attrElement == 4)
            primitiveIndex = fragFacetIndex;
        // Cell attribute
        else if (attrElement == 3)
            primitiveIndex = fragCellIndex;

        float fragAttrVal = texelFetch(attributeData, primitiveIndex).x;
        col = vec3(texture(fragColorMap, clamp((fragAttrVal - attrRange.x) / (attrRange.y - attrRange.x), 0., 1.)));
    }
    


    // Highlight
    float cellHighlightVal = texelFetch(highlightBuf, fragCellIndex).x;
    float facetHighlightVal = texelFetch(facetHighlightBuf, fragFacetIndex).x;
    float highlightVal = max(cellHighlightVal, facetHighlightVal);

    if (highlightVal > 0) {
        // Interpolate between hover / select colors according to highlight value
        vec3 hovColor = vec3(1.,1.,1.);
        vec3 selColor = vec3(0., 0.22, 1.);
        vec3 hlColor = mix(hovColor, selColor, highlightVal);
        // Mix with current point color (80%)
        col = mix(col, hlColor, .8);
    }

    // Diffuse light
    if (is_light_enabled) {
        vec3 dirLight;

        if (is_light_follow_view)
            dirLight = fragViewDir;
        else
            dirLight = vec3(-0.5f, -0.8f, 0.2f);
        
        float diffuse = max((1.f - dot(dirLight, fragNormal)) * .5f + .45f /* ambiant */, 0.f);
        col = col * diffuse;
    }



    // if (fragRenderMode == 0 && (fragRenderMeshMode & 2) == 2) {
    //     // Render wireframe
    //     if (fragHeights.y < meshSize || fragHeights.z < meshSize) {
    //         col = vec3(0,0,0);
    //     }
    // }

    // Outputs
    fragFacetIndexOut = vec4(encode_id(fragFacetIndex), 1.f);
    fragCellIndexOut = vec4(encode_id(fragCellIndex), 1.f);
    FragColor = vec4(col, 1.f);
}