#version 440 core

// Color output
// out vec4 FragColor;
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
in float fragAttrVal;
in float fragHighlight;
in float fragFilter;
flat in vec3 fragViewDir;

flat in vec3 fragWorldPos;
in vec3 fragWorldPos2;

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
uniform vec2 attributeDataMinMax = vec2(0.f, 1.f);
uniform int colorMode = 0;

vec3 encode_id(int id) {
    int r = id & 0x000000FF;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    return vec3(r / 255.f, g / 255.f, b / 255.f); 
}

void main()
{

    // Initialize color to black
    vec3 col = vec3(0.f, 0.f, 0.f);

    // Check if cell is filtered
    bool is_filter = fragFilter >= .5f;

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
        ref_point = fragWorldPos2;
    }

      float distance = dot(clipping_plane_normal, ref_point - clipping_plane_point) / length(clipping_plane_normal);
      
      if ((invert_clipping == 0 && distance < 0.0) || (invert_clipping == 1 && distance >= 0.0)) {
         is_filter = true;
      }
   }

    /* --- CLIPPING END --- */

    /* --- FILTER --- */

    // If the fragment is filtered, discard it
    if (is_filter) {
        discard;
        return;
    }

    /* --- FILTER END --- */


    if (colorMode == 0) {
        col = vec3(0.8f, 0.f, 0.2f);
    } 
    else {
        col = vec3(texture(fragColorMap, clamp((fragAttrVal - attributeDataMinMax.x) / (attributeDataMinMax.y - attributeDataMinMax.x), 0., 1.)));
    }        
    
    if (fragHighlight >= 1.f && fragHighlight < 2.f) {
        col = mix(col, vec3(1.,1.,1.), 0.8);
    }
    else if (fragHighlight >= 2.f) {
        // a between 0.5 and 0.75 - 0.25 range
        // col = mix(col, vec3(0., 0.22, 1.), 0.5 + fragHighlight * 0.25);
        col = mix(col, vec3(0., 0.22, 1.), 0.8);
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



    if (fragRenderMode == 0 && (fragRenderMeshMode & 2) == 2) {
        // Render wireframe
        if (fragHeights.y < meshSize || fragHeights.z < meshSize) {
            col = vec3(0,0,0);
        }
    }

    // Outputs
    fragFacetIndexOut = vec4(encode_id(fragFacetIndex), 1.f);
    fragCellIndexOut = vec4(encode_id(fragCellIndex), 1.f);
    FragColor = vec4(col, 1.f);
}