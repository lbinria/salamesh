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

uniform sampler2D colormap;
uniform sampler2D colormap0;
uniform sampler2D colormap1;
uniform sampler2D colormap2;

uniform vec2 attrRange = vec2(0.f, 1.f);
// uniform vec2[3] attrRanges;
uniform int attrRepeat = 1;
uniform samplerBuffer attrBuf;
uniform int attrElement;
uniform int attrNDims;

uniform samplerBuffer filterBuf;
uniform samplerBuffer highlightBuf;


uniform int highlightElement;
uniform int filterElement;


uniform int colorMode = 0;
uniform vec3 color;

uniform bool isCornerVisible;

uniform int meshIndex;

flat in int surfaceType;

uniform samplerBuffer colormap0Buf;
uniform samplerBuffer colormap1Buf;
uniform samplerBuffer colormap2Buf;

uniform vec2 attrRange0;
uniform vec2 attrRange1;
uniform vec2 attrRange2;

uniform int colormapElement0 = -1;
uniform int colormapElement1 = -1;
uniform int colormapElement2 = -1;

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

vec2 getLayerRange(int layer) {
    if (layer == 0) return attrRange0;
    if (layer == 1) return attrRange1;
    if (layer == 2) return attrRange2;
    return vec2(0.);
}

int getSelectedAttrElement(int layer) {
    if (layer == 0) return colormapElement0;
    if (layer == 1) return colormapElement1;
    if (layer == 2) return colormapElement2;
    if (layer == 3) return highlightElement;
    if (layer == 4) return filterElement;
    return -1;
}

vec4 getAttributeColor2(int idx, int layer) {
    vec2 range = getLayerRange(layer);
    
    float rangeLength = range.y - range.x;
    float rangeRepeat = rangeLength / attrRepeat;

    // vec2 coords = vec2(0.);
    // for (int d = 0; d < attrNDims; d++) {

    //     float attrVal = fetch(idx * attrNDims + d, bufIdx).x;
    //     // TODO uncomment for colormap but comment for texture....
    //     float remapVal = (mod(attrVal - attrRange.x, rangeRepeat + 1)) / rangeRepeat;
    //     // float remapVal = attrVal;
    //     float v = clamp(remapVal, 0., 1.);
    //     coords[d] = v;
    // }
    
    // return texture(colormap, coords);

    float attrVal = fetchLayer(idx, layer).x;
    // float remapVal = (mod(attrVal - range.x, rangeRepeat + 1)) / rangeRepeat;
    float remapVal = (attrVal - range.x) / rangeLength;
    float v = clamp(remapVal, 0., 1.);
    
    // return texture(colormap, vec2(v, 0.));
    return fetchColormap(layer, vec2(v, 0.));
}

vec4 getAttributeColor(int index) {
    float range = attrRange.y - attrRange.x;
    float rangeRepeat = range / attrRepeat;

    vec2 coords = vec2(0.);
    for (int d = 0; d < attrNDims; d++) {

        float attrVal = texelFetch(attrBuf, index * attrNDims + d).x;
        // TODO uncomment for colormap but comment for texture....
        float remapVal = (mod(attrVal - attrRange.x, rangeRepeat + 1)) / rangeRepeat;
        // float remapVal = attrVal;
        float v = clamp(remapVal, 0., 1.);
        coords[d] = v;

    }
    
    return texture(colormap, coords);
}

void showCornerAttributes(inout vec3 col) {
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
        vec4 attrCol = getAttributeColor2(cornerIdx, 0);

        // Check distance from point is lesser than 0.333 
        // (as we use bary coords: a value of 1. means fragment is on point, a value of 0. means the furthest)
        if (fragBarycentric[curPointIdx] > 0.666 /* hell number ! :japanese_ogre: */)
            col = vec3(attrCol);
    } else {

        
        // Init corners colors
        vec4 p0Col = vec4(0.);
        vec4 p1Col = vec4(0.);
        vec4 p2Col = vec4(0.);

        if (surfaceType == 0) {
            // Surface is a tri
            p0Col = getAttributeColor2(fragCornerIndex, 0);
            p1Col = getAttributeColor2(fragCornerIndex + 1, 0);
            p2Col = getAttributeColor2(fragCornerIndex + 2, 0);

        } else {
            // Get number of vertex for facet
            int nvertsPerFacet = int(texelFetch(nvertsPerFacetBuf, fragFacetIndex).x);

            // Surface is a polygon, p0 is bary
            // So color of bary is the average of the colors of all corners
            p0Col = vec4(0.);
            for (int lc = 0; lc < nvertsPerFacet; ++lc)
                p0Col += getAttributeColor2(fragCornerOff + (fragCornerIndex + lc) % nvertsPerFacet, 0);

            p0Col /= nvertsPerFacet;

            p1Col = getAttributeColor2(fragCornerOff + fragCornerIndex, 0);
            p2Col = getAttributeColor2(fragCornerOff + ((fragCornerIndex + 1) % nvertsPerFacet), 0);
        }

        // Compute color from barycentric coords
        col = fragBarycentric.x * p0Col.xyz + fragBarycentric.y * p1Col.xyz + fragBarycentric.z * p2Col.xyz;
    }
}

void attributeCompute(inout vec3 col) {
    if (colorMode != 1)
        return;

    vec4 attrCol = vec4(0.);

    if (attrElement == 2) {
        showCornerAttributes(col);
    }
    // Facet attribute
    else if (attrElement == 8) {
        attrCol = getAttributeColor2(fragFacetIndex, 0);

        // Transparency
        if (attrCol.a > 0.)
            col = vec3(attrCol);
        else {
            discard;
        }
    }
}

vec4 showColormap(int layer) {
    int kind = getSelectedAttrElement(layer);
    if (kind == 8 /* facets */)
        return getAttributeColor2(fragFacetIndex, layer);
    else if (kind == 2 /* corners */) {
        // TODO implement
        return vec4(0., 0., 0., 1.);
    }
    else if (kind == -1 /* Layer deactivated */) {
        return vec4(0.);
    }

    return vec4(1., 0., 0., 1.);
}

void _filter(inout vec3 col) {
    // Check if cell is filtered
    bool isFiltered = texelFetch(filterBuf, fragFacetIndex).x >= .5;
    if (isFiltered) {
        discard;
    }
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

void main()
{

    vec3 col = color;

    _filter(col);
    clip(col);
    // attributeCompute(col);

    if (colorMode == 1) {
        vec4 c0 = showColormap(0);
        vec4 c1 = showColormap(1);
        vec4 c2 = showColormap(2);

        // Blend
        vec4 c = vec4(0.);

        if (c0.a > 0.)
            c = c0;
        if (c1.a > 0.)
            c = mix(c, c1, .5);
        if (c2.a > 0.)
            c = mix(c, c2, .5);

        if (c.a <= 0.)
            discard;
        else 
            col = c.xyz;
    }

    highlight(col);
    shading(col);
    wireframe(col);

    // Outputs
    fragFacetIndexOut = vec4(encode_id(fragFacetIndex), 1.);
    fragMeshIndexOut = vec4(encode_id(meshIndex), 1.);
    FragColor = vec4(col, 1.f);
}