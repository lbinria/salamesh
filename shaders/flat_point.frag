#version 440 core

// Avoid the disable of earlier Depth-testing
layout(depth_less) out float gl_FragDepth;

// Color output
layout(location = 0) out vec4 FragColor;
layout(location = 3) out vec4 FragVertexIndexOut;

flat in int FragVertexIndex;

in vec3 fragWorldPos;

uniform vec3 pointColor;


uniform bool is_clipping_enabled = false;
uniform vec3 clipping_plane_normal; // (a, b, c)
uniform vec3 clipping_plane_point;  // A point on the plane
uniform int invert_clipping = 0; // 0: normal, 1: inverted

uniform vec3 hoverColor = vec3(1.,1.,1.);
uniform vec3 selectColor = vec3(0., 0.22, 1.);

uniform int colorMode = 0;

uniform sampler1D fragColorMap;
uniform vec2 attrRange = vec2(0.f, 1.f);
uniform samplerBuffer attrBuf;

uniform samplerBuffer filterBuf;
uniform samplerBuffer highlightBuf;

flat in float fragZ;


vec3 encode_id(int id) {
    int r = id & 0x000000FF;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    return vec3(r / 255.f, g / 255.f, b / 255.f); 
}

void main()
{
    // Check whether point is filtered
    bool isFiltered = texelFetch(filterBuf, FragVertexIndex).x > 0;
    if (isFiltered)
        discard;


    /* --- CLIPPING --- */

   // Calculate the distance from the cell barycenter to the plane
   if (is_clipping_enabled) {
      float distance = dot(clipping_plane_normal, fragWorldPos - clipping_plane_point) / length(clipping_plane_normal);
      
      if ((invert_clipping == 0 && distance < 0.0) || (invert_clipping == 1 && distance >= 0.0)) {
         discard;
      }
   }

    /* --- CLIPPING END --- */


    vec3 col = pointColor;

    // Check circle distance function
    vec2 V = 2.0 * (gl_PointCoord - vec2(0.5, 0.5));
    float oneMinusR2 = 1. - dot(V,V);
    if (oneMinusR2 < 0.0) {
        discard;
    }


    // Compute normal using point coordinates and radius as Z
    vec3 N = vec3(V.x, -V.y, sqrt(oneMinusR2));

    // float zRange = abs(gl_FragCoord.z - fragZ);
    // gl_FragDepth = gl_FragCoord.z - zRange * N.z;
    gl_FragDepth = gl_FragCoord.z - 0.00005 * N.z;
    
    // Attribute color mode !
    if (colorMode != 0) {
        // TODO add repeat / transparency colormaps
        float fragAttrVal = texelFetch(attrBuf, FragVertexIndex).x;
        col = vec3(texture(fragColorMap, clamp((fragAttrVal - attrRange.x) / (attrRange.y - attrRange.x), 0., 1.)));
    }

    // Light
    col = col * dot(N, vec3(0.0, 0.0, 1.0));

    // Highlight
    float highlightVal = texelFetch(highlightBuf, FragVertexIndex).x;
    if (highlightVal > 0) {
        // Interpolate between hover / select colors according to highlight value
        float t = highlightVal;
        // Add condition if you want a hard switch between hover and select
        t = step(0.5, highlightVal);
        vec3 hlColor = mix(hoverColor, selectColor, t);
        // Mix with current point color (80%)
        col = mix(col, hlColor, .8);
    }

    FragVertexIndexOut = vec4(encode_id(FragVertexIndex), 1.);
    FragColor = vec4(col, 1.f);
}