#version 440 core

// Color output
out vec4 FragColor;
// Primitive indexation
flat in int fragCellIndex;
flat in int fragFacetIndex;
in float fragVertexIndex;

in vec3 fragBary;
in vec3 fragNormal;
in vec3 fragHeights;
in float fragAttrVal;
in float fragHighlight;
in float fragFilter;

flat in vec3 fragWorldPos;
in vec3 fragWorldPos2;

uniform bool is_light_enabled;

uniform int fragRenderMode = 0;
uniform int fragRenderMeshMode = 2;
uniform float meshSize;

uniform int clipping_mode = 0; // 0: cell, 1: std, 2: slice
uniform bool is_clipping_enabled = false;
uniform vec3 clipping_plane_normal = vec3(0.2f, 0.6f, 0.0f); // (a, b, c)
uniform vec3 clipping_plane_point = vec3(0.0f, 0.0f, 0.0);  // A point on the plane

uniform sampler1D fragColorMap;
uniform vec2 attributeDataMinMax = vec2(0.f, 1.f);
uniform int colorMode = 0;

uniform vec3 point = vec3(0.0f, 0.0f, 0.0f); // Point to check distance from

vec3 encode_id(int id) {
    int r = id & 0x000000FF;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    return vec3(r / 255.f, g / 255.f, b / 255.f); 
}

void main()
{

    vec3 col = vec3(0.f, 0.f, 0.f);


    bool is_filter = fragFilter >= .5f;
   // Calculate the distance from the cell barycenter to the plane
   if (is_clipping_enabled) {
    vec3 ref_point;
    if (clipping_mode == 0) {
        ref_point = fragBary;
    } else if (clipping_mode == 1) {
        ref_point = fragWorldPos2;
    }

      float distance = dot(clipping_plane_normal, ref_point - clipping_plane_point) / length(clipping_plane_normal);
      
      if (distance < 0.0) {
         is_filter = true;
      }
   }


    if (is_filter) {
        discard;
        return;
    }

    // Render mode color
    if (fragRenderMode == 0) {


        if (colorMode == 0) {
            col = vec3(0.8f, 0.f, 0.2f);
        } 
        else {
            col = vec3(texture(fragColorMap, clamp((fragAttrVal - attributeDataMinMax.x) / (attributeDataMinMax.y - attributeDataMinMax.x), 0., 1.)));
        }
        
        // // Clipping
        
        // // Calculate the distance from the fragment to the plane
        // if (is_clipping_enabled) {
        //     float distance = dot(planeNormal, fragWorldPos - planePoint) / length(planeNormal);
            
        //     if (distance < 0.0) {
        //         discard; // Discard the fragment if it's behind the clipping plane
        //     }
        // }


        
        
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
            vec3 dirLight = vec3(-0.5f, -0.8f, 0.2f);
            float diffuse = (1.f - dot(dirLight, fragNormal)) * .5f + .5f;
            col = col * diffuse;
        }

    // Render mode facet index
    } else if (fragRenderMode == 2) {
        col = encode_id(fragFacetIndex);
    // Render mode cell index
    } else if (fragRenderMode == 3)
        col = encode_id(fragCellIndex);


    if ((fragRenderMeshMode & 2) == 2) {
        // Render wireframe
        if (fragHeights.y < meshSize || fragHeights.z < meshSize) {
            col = vec3(0,0,0);
        }
    }

    FragColor = vec4(col, 1.f);
}