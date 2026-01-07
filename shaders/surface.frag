#version 440 core

// Primitive indexation
flat in int fragCornerIndex;
flat in int fragCornerOff;
flat in int fragFacetIndex;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 fragFacetIndexOut;
layout(location = 4) out vec4 fragMeshIndexOut;

in vec3 fragBary;
in vec3 fragNormal;
in vec3 fragHeights;
flat in vec3 flatFragHeights;

flat in vec3 fragViewDir;

in vec3 fragWorldPos;

in vec3 fragBarycentric;

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

// Note: cannot index samplerBuffer with dynamic indexing !
uniform sampler2D colormap0;
uniform sampler2D colormap1;
uniform sampler2D colormap2;

uniform int colorMode = 0;
uniform vec3 color;

uniform bool isCornerVisible;

uniform int meshIndex;

flat in int surfaceType;

// Note: cannot index samplerBuffer with dynamic indexing !
uniform samplerBuffer filterBuf;
uniform samplerBuffer highlightBuf;
uniform samplerBuffer colormap0Buf;
uniform samplerBuffer colormap1Buf;
uniform samplerBuffer colormap2Buf;

uniform vec2 attrRange[3];
uniform int attrRepeat[3] = {1, 1, 1};
uniform int attrNDims[3] = {1, 1, 1};

uniform int colormapElement[3] = {-1, -1, -1};
uniform int highlightElement;
uniform int filterElement;

uniform samplerBuffer nvertsPerFacetBuf;

vec3 encode_id(int id) {
    int r = id & 0x000000FF;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    return vec3(r / 255.f, g / 255.f, b / 255.f); 
}

// Return the nearest vertex of triangle index
// from barycentric coordinates
int getCurrentPointIdx(vec3 b) {
    for (int i = 0; i < 3; i++) {
        if (b[i] > b[(i + 1) % 3] && b[i] > b[(i + 2) % 3])
            return i;
    }
}

float fetchLayer(int idx, int layer) {
    if (layer == 0) return texelFetch(colormap0Buf, idx).x;
    if (layer == 1) return texelFetch(colormap1Buf, idx).x;
    if (layer == 2) return texelFetch(colormap2Buf, idx).x;
    if (layer == 3) return texelFetch(highlightBuf, idx).x;
    if (layer == 4) return texelFetch(filterBuf, idx).x;
    return 0.;
}

vec4 fetchColormap(int layer, vec2 coords) {
    if (layer == 0) return texture(colormap0, coords);
    if (layer == 1) return texture(colormap1, coords);
    if (layer == 2) return texture(colormap2, coords);
    return vec4(0.);
}

vec4 getLayerColor(int idx, int layer) {

    vec2 range = attrRange[layer];
    int nRepeat = attrRepeat[layer];
    int nDims = attrNDims[layer];
    // bool automap = getLayerAutomap(layer);

    float rangeLength = range.y - range.x;
    float rangeRepeat = rangeLength / nRepeat;


    vec2 coords = vec2(0.);
    for (int d = 0; d < nDims; d++) {

        float val = fetchLayer(idx * nDims + d, layer).x;
        if (nDims == 1) {
            // Apply range
            // val = (mod(attrVal - attrRange.x, rangeRepeat + 1)) / rangeRepeat;
            val = (val - range.x) / rangeLength; // Simple auto map range version with no repeat
        }
        
        float v = clamp(val, 0., 1.);
        coords[d] = v;
    }
    
    return fetchColormap(layer, coords);

    // float attrVal = fetchLayer(idx, layer).x;
    // // float remapVal = (mod(attrVal - range.x, rangeRepeat + 1)) / rangeRepeat;
    // float remapVal = (attrVal - range.x) / rangeLength;
    // float v = clamp(remapVal, 0., 1.);
    
    // return fetchColormap(layer, vec2(v, 0.));
}

// vec4 getAttributeColor(int index) {
//     float range = attrRange.y - attrRange.x;
//     float rangeRepeat = range / attrRepeat;

//     vec2 coords = vec2(0.);
//     for (int d = 0; d < attrNDims; d++) {

//         float attrVal = texelFetch(attrBuf, index * attrNDims + d).x;
//         // TODO uncomment for colormap but comment for texture....
//         float remapVal = (mod(attrVal - attrRange.x, rangeRepeat + 1)) / rangeRepeat;
//         // float remapVal = attrVal;
//         float v = clamp(remapVal, 0., 1.);
//         coords[d] = v;

//     }
    
//     return texture(colormap, coords);
// }

vec4 showCornerAttributes(int layer) {
    if (isCornerVisible) {
        // Which triangle vertex is the nearest ?
        // Use barycentric coordinates to get the closest point of the current fragment
        int curPointIdx = getCurrentPointIdx(fragBarycentric);

        int curPointOff = 0;
        // Polygon case
        if (surfaceType == 1) {
            // In polygons, the point 0 is the barycenter
            // we never want display any corner on it, so snap to other vertices
            // Only the points 1 & 2 (y, z) components represent real corners
            if (curPointIdx == 0) {
                if (fragBarycentric.y > fragBarycentric.z)
                    curPointIdx = 1;
                else 
                    curPointIdx = 2;
            }
            // In polygon
            // Point 1 => localVertex = 0
            // Point 2 => localVertex = 1
            curPointOff = -1;


        }
        
        // Get number of vertex for facet
        int nvertsPerFacet = surfaceType == 0 ? 3 : int(texelFetch(nvertsPerFacetBuf, fragFacetIndex).x);

        // Get the current corner index by using the "provoking" vertex corner index
        // plus the current point index
        // int cornerIdx = fragCornerIndex + curPointIdx + curPointOff;
        int cornerIdx = fragCornerOff + (fragCornerIndex + curPointIdx + curPointOff) % nvertsPerFacet;
        vec4 attrCol = getLayerColor(cornerIdx, layer);

        // Check distance from point is lesser than 0.333 
        // (as we use bary coords: a value of 1. means fragment is on point, a value of 0. means the furthest)
        if (fragBarycentric[curPointIdx] > 0.666 /* hell number ! :japanese_ogre: */)
            return attrCol;
        else 
            return vec4(0., 0., 0., -1.);
    } else {

        
        // Init corners colors
        vec4 p0Col = vec4(0.);
        vec4 p1Col = vec4(0.);
        vec4 p2Col = vec4(0.);

        if (surfaceType == 0) {
            // Surface is a tri
            p0Col = getLayerColor(fragCornerIndex, layer);
            p1Col = getLayerColor(fragCornerIndex + 1, layer);
            p2Col = getLayerColor(fragCornerIndex + 2, layer);

        } else {
            // Get number of vertex for facet
            int nvertsPerFacet = int(texelFetch(nvertsPerFacetBuf, fragFacetIndex).x);

            // Surface is a polygon, p0 is bary
            // So color of bary is the average of the colors of all corners
            p0Col = vec4(0.);
            for (int lc = 0; lc < nvertsPerFacet; ++lc)
                p0Col += getLayerColor(fragCornerOff + (fragCornerIndex + lc) % nvertsPerFacet, layer);

            p0Col /= nvertsPerFacet;

            p1Col = getLayerColor(fragCornerOff + fragCornerIndex, layer);
            p2Col = getLayerColor(fragCornerOff + ((fragCornerIndex + 1) % nvertsPerFacet), layer);
        }

        // Compute color from barycentric coords
        return vec4(fragBarycentric.x * p0Col.xyz + fragBarycentric.y * p1Col.xyz + fragBarycentric.z * p2Col.xyz, 1.);
    }
}

vec4 showColormap(int layer) {
    int kind = colormapElement[layer];
    
    if (kind == 8 /* facets */) {
        return getLayerColor(fragFacetIndex, layer);
    }

    if (kind == 2 /* corners */) {
        return showCornerAttributes(layer);
    }
    
    if (kind == -1 /* Layer deactivated */) {
        return vec4(0., 0., 0., -1.);
    }

    return vec4(1., 0., 0., 1.);
}

void _filter(inout vec3 col) {
    bool filtered = texelFetch(filterBuf, fragFacetIndex).x >= .5;

    if (filtered)
        discard;
}

void clip(inout vec3 col) {
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
}

void highlight(inout vec3 col) {
    // Only highlight facets
    if (highlightElement != 8)
        return;

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
}

void shading(inout vec3 col) {
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
}

void wireframe(inout vec3 col) {
    float f1 = 1. - smoothstep(flatFragHeights.x - meshSize, flatFragHeights.x, flatFragHeights.x - fragHeights.x);
    float f2 = 1. - smoothstep(flatFragHeights.y - meshSize, flatFragHeights.y, flatFragHeights.y - fragHeights.y);
    float f3 = 1. - smoothstep(flatFragHeights.z - meshSize, flatFragHeights.z, flatFragHeights.z - fragHeights.z);
    col *= f1 * f2 * f3;
}

// Mix with alpha discard
vec4 blendMix(vec4 c1, vec4 c2, float t) {
    if (c1.a <= -1.)
        return c2;
    if (c2.a <= -1.)
        return c1;
    // Second term is equals to 0. => discard
    if (c2.a <= 0.)
        return vec4(0.);
    // Else, mix colors
    return mix(c1, c2, t);
}

void main()
{
    vec3 col = color;

    _filter(col);
    clip(col);

    // Attribute mode
    vec4 c[3];
    c[0] = showColormap(0);
    c[1] = showColormap(1);
    c[2] = showColormap(2);

    // Blend
    vec4 b = blendMix(c[0], blendMix(c[1], c[2], .5), .5);

    if (b.a > -1.) {
        if (b.a > 0.)
            col = b.rgb;
        else 
            discard;
    }

    highlight(col);
    shading(col);
    wireframe(col);

    // Outputs
    fragFacetIndexOut = vec4(encode_id(fragFacetIndex), 1.);
    fragMeshIndexOut = vec4(encode_id(meshIndex), 1.);
    FragColor = vec4(col, 1.f);
}