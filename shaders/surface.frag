#version 440 core

// Primitive indexation
flat in int fragCornerIndex;
flat in int fragFacetIndex;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 fragFacetIndexOut;
layout(location = 4) out vec4 fragMeshIndexOut;

in vec3 fragBary;
in vec3 fragNormal;
in vec3 fragHeights;
flat in vec3 flatFragHeights;

flat in vec3 fragViewDir;

in vec3 fragBarycentric;
flat in vec2 fragV1Pos;
flat in vec2 fragV2Pos;

in vec3 fragWorldPos;

uniform bool is_light_enabled;
uniform bool is_light_follow_view;

uniform float meshSize;

uniform int clipping_mode = 1; // 0: cell, 1: std, 2: slice
uniform bool is_clipping_enabled = false;
uniform vec3 clipping_plane_normal;
uniform vec3 clipping_plane_point;
uniform int invert_clipping = 0; // 0: normal, 1: inverted

uniform vec3 hoverColor = vec3(1.,1.,1.);
uniform vec3 selectColor = vec3(0., 0.22, 1.);

uniform sampler1D fragColorMap;
uniform vec2 attrRange = vec2(0.f, 1.f);
uniform int attrRepeat = 1;
uniform samplerBuffer attributeData;
uniform int attrElement;

uniform samplerBuffer filterBuf;
uniform samplerBuffer highlightBuf;

uniform int colorMode = 0;
uniform vec3 color;

uniform int meshIndex;

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
    bool isFiltered = texelFetch(filterBuf, fragFacetIndex).x >= .5;
    if (isFiltered) {
        discard;
    }

    /* --- CLIPPING --- */

   // Calculate the distance from the cell barycenter to the plane
   if (is_clipping_enabled) {
    vec3 ref_point;
    if (clipping_mode == 0) {
        // Use the barycenter of the facets to exclude facets
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

    if (colorMode == 1 && (attrElement == 2 || attrElement == 8)) {

        int primitiveIndex;
        
        // Which triangle vertex is the nearest ?
        int localI = 0;

        if (fragBarycentric.y > fragBarycentric.x && fragBarycentric.y > fragBarycentric.z) {
            localI = 1;
        } else if (fragBarycentric.z > fragBarycentric.x && fragBarycentric.z > fragBarycentric.y) {
            localI = 2;
        } else {
            if (fragBarycentric.y > fragBarycentric.z)
                localI = 1;
            else
                localI = 2;
        }

        int cornerIdx = fragCornerIndex + localI;

        // Corner attribute
        if (attrElement == 2)
            primitiveIndex = cornerIdx;
        // Cell facet attribute
        else if (attrElement == 8)
            primitiveIndex = fragFacetIndex;
        
        float range = attrRange.y - attrRange.x;
        float rangeRepeat = range / attrRepeat;
        float fragAttrVal = mod(texelFetch(attributeData, primitiveIndex).x - attrRange.x, rangeRepeat + 1);
        vec4 attrCol = texture(fragColorMap, clamp(fragAttrVal / rangeRepeat, 0., 1.));



        if (attrElement == 2) {
            
            vec2 fragVPos = localI == 1 ? fragV1Pos : fragV2Pos;

            float d = length(gl_FragCoord.xy - fragVPos);
            if (d < 15) {
                col = vec3(attrCol);
            }
        } else {

            // Transparency
            if (attrCol.a > 0.)
                col = vec3(attrCol);
            else {
                discard;
            }

        }
    }
    


    // Highlight
    float highlightVal = texelFetch(highlightBuf, fragFacetIndex).x;

    if (highlightVal > 0) {
        // Interpolate between hover / select colors according to highlight value
        float t = highlightVal;
        // Add condition if you want a hard switch between hover and select
        t = step(0.5, highlightVal);
        vec3 hlColor = mix(hoverColor, selectColor, t);
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


    // Wireframe
    // if (fragHeights.x < meshSize || fragHeights.y < meshSize || fragHeights.z < meshSize) {
    //     // col = vec3(0,0,0);
    //     col =  vec3(0,0,0);
    // }
    float f1 = 1. - smoothstep(flatFragHeights.x - meshSize, flatFragHeights.x, flatFragHeights.x - fragHeights.x);
    float f2 = 1. - smoothstep(flatFragHeights.y - meshSize, flatFragHeights.y, flatFragHeights.y - fragHeights.y);
    float f3 = 1. - smoothstep(flatFragHeights.z - meshSize, flatFragHeights.z, flatFragHeights.z - fragHeights.z);
    col *= f1 * f2 * f3;


    // Outputs
    fragFacetIndexOut = vec4(encode_id(fragFacetIndex), 1.);
    fragMeshIndexOut = vec4(encode_id(meshIndex), 1.);
    FragColor = vec4(col, 1.f);
}