#version 440 core

// Primitive indexation
flat in int fragCornerIndex;
flat in int fragCornerOff;
flat in int fragFacetIndex;

layout(location = 0) out vec4 FragColor;
layout(location = 3) out vec4 fragFacetIndexOut;
layout(location = 5) out vec4 fragMeshIndexOut;

in vec3 fragBary;
in vec3 fragNormal;
in vec3 fragHeights;
flat in vec3 flatFragHeights;

in vec3 fragWorldPos;

in vec3 fragBarycentric;

struct Light {
	vec3 dir;
	bool enabled;
};

uniform Light light;

struct Clipping {
    int mode; // {0 = cell, 1 = std, 2 = slice}
    vec3 normal;
    vec3 point;
    bool invert;
    bool enabled;
};

uniform Clipping clipping;

struct Style {
   vec3 color;
   float size;
   float shrink;
   bool cornerVisible;
};

uniform Style style;

uniform vec3 hoverColor = vec3(1.,1.,1.);
uniform vec3 selectColor = vec3(0., 0.22, 1.);


uniform bool isCornerVisible;

uniform int meshIndex;

flat in int surfaceType;



struct LayerData {
    int ndims;
    int repeat;
    vec2 range;
    bool activated;
};

uniform samplerBuffer layerBuffers[3][7];
uniform LayerData layers[3][7];
uniform sampler2D colormaps[7];


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

// float fetchLayer(int idx, const int kind) {
//     return texelFetch(layerBuffers[0][kind], idx).x;
//     // if (kind == 0) return texelFetch(layers[0][0], idx).x;
//     // if (kind == 1) return texelFetch(layers[0][1], idx).x;
//     // if (kind == 2) return texelFetch(layers[0][2], idx).x;
//     // if (kind == 3) return texelFetch(layers[0][kind], idx).x;
//     // if (kind == 4) return texelFetch(layers[0][4], idx).x;
//     // if (kind == 5) return texelFetch(layers[0][5], idx).x;
//     // if (kind == 6) return texelFetch(layers[0][6], idx).x;
//     // return 0.;
// }

// vec4 fetchColormap(const int kind, vec2 coords) {
//     return texture(colormaps[kind], coords);
//     // if (kind == 0) return texture(colormaps[0], coords);
//     // if (kind == 1) return texture(colormaps[1], coords);
//     // if (kind == 2) return texture(colormaps[2], coords);
//     // if (kind == 3) return texture(colormaps[3], coords);
//     // if (kind == 4) return texture(colormaps[4], coords);
//     // if (kind == 5) return texture(colormaps[5], coords);
//     // if (kind == 6) return texture(colormaps[6], coords);
//     // return vec4(0.);
// }

float fetchLayer(int idx, const int kind) {
    return texelFetch(layerBuffers[0][kind], idx).x;
    // if (kind == 0) return texelFetch(layerBuffers[0][0], idx).x;
    // if (kind == 1) return texelFetch(layerBuffers[0][1], idx).x;
    // if (kind == 2) return texelFetch(layerBuffers[0][2], idx).x;
    // if (kind == 3) return texelFetch(layerBuffers[0][3], idx).x;
    // if (kind == 4) return texelFetch(layerBuffers[0][4], idx).x;
    // if (kind == 5) return texelFetch(layerBuffers[0][5], idx).x;
    // if (kind == 6) return texelFetch(layerBuffers[0][6], idx).x;
    // return 0.;
}

vec4 fetchColormap(const int kind, vec2 coords) {
    return texture(colormaps[kind], coords);
    // if (kind == 0) return texture(colormaps[0], coords);
    // if (kind == 1) return texture(colormaps[1], coords);
    // if (kind == 2) return texture(colormaps[2], coords);
    // if (kind == 3) return texture(colormaps[3], coords);
    // if (kind == 4) return texture(colormaps[4], coords);
    // if (kind == 5) return texture(colormaps[5], coords);
    // if (kind == 6) return texture(colormaps[6], coords);
    // return vec4(0.);
}

vec4 getLayerColor(int idx, int kind) {

    vec2 range = layers[0][kind].range;
    int nRepeat = layers[0][kind].repeat;
    int nDims = layers[0][kind].ndims;
    // bool automap = getLayerAutomap(layer);

    float rangeLength = range.y - range.x;
    float rangeRepeat = rangeLength / nRepeat;


    vec2 coords = vec2(0.);
    for (int d = 0; d < nDims; d++) {

        float val = fetchLayer(idx * nDims + d, kind).x;
        if (nDims == 1) {
            // Apply range
            // val = (mod(attrVal - attrRange.x, rangeRepeat + 1)) / rangeRepeat;
            val = (val - range.x) / rangeLength; // Simple auto map range version with no repeat
        }
        
        float v = clamp(val, 0., 1.);
        coords[d] = v;
    }
    
    return fetchColormap(kind, coords);

    // float attrVal = fetchLayer(idx, layer).x;
    // // float remapVal = (mod(attrVal - range.x, rangeRepeat + 1)) / rangeRepeat;
    // float remapVal = (attrVal - range.x) / rangeLength;
    // float v = clamp(remapVal, 0., 1.);
    
    // return fetchColormap(layer, vec2(v, 0.));
}

vec4 showCornerAttributes() {
    if (isCornerVisible) {
        // Which triangle vertex is the nearest ?
        // Use barycentric coordinates to get the closest point of the current fragment
        int curPointIdx = getCurrentPointIdx(fragBarycentric);

        int curPointOff = 0;
        // Quad / Polygon case
        if (surfaceType > 0) {
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
        // int nvertsPerFacet = surfaceType == 0 ? 3 : int(texelFetch(nvertsPerFacetBuf, fragFacetIndex).x);

        // TODO here replace surfaceType by nvertsPerFacet given from vertex shader
        // it enable to process regular size polygon in the same way
        int nvertsPerFacet = 0;
        if (surfaceType == 0)
            nvertsPerFacet = 3;
        else if (surfaceType == 1)
            nvertsPerFacet = int(texelFetch(nvertsPerFacetBuf, fragFacetIndex).x);
        else if (surfaceType == 2)
            nvertsPerFacet = 4;



        // Get the current corner index by using the "provoking" vertex corner index
        // plus the current point index
        // int cornerIdx = fragCornerIndex + curPointIdx + curPointOff;
        int cornerIdx = fragCornerOff + (fragCornerIndex + curPointIdx + curPointOff) % nvertsPerFacet;
        vec4 attrCol = getLayerColor(cornerIdx, 1 /* corner */);

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
            p0Col = getLayerColor(fragCornerIndex, 1 /* corner */);
            p1Col = getLayerColor(fragCornerIndex + 1, 1 /* corner */);
            p2Col = getLayerColor(fragCornerIndex + 2, 1 /* corner */);

        } else {
            // Get number of vertex for facet
            int nvertsPerFacet;
            if (surfaceType == 1) 
                nvertsPerFacet = int(texelFetch(nvertsPerFacetBuf, fragFacetIndex).x);
            else if (surfaceType == 2)
                nvertsPerFacet = 4;

            // Surface is a polygon, p0 is bary
            // So color of bary is the average of the colors of all corners
            p0Col = vec4(0.);
            for (int lc = 0; lc < nvertsPerFacet; ++lc)
                p0Col += getLayerColor(fragCornerOff + (fragCornerIndex + lc) % nvertsPerFacet, 1 /* corner */);

            p0Col /= nvertsPerFacet;

            p1Col = getLayerColor(fragCornerOff + fragCornerIndex, 1 /* corner */);
            p2Col = getLayerColor(fragCornerOff + ((fragCornerIndex + 1) % nvertsPerFacet), 1 /* corner */);
        }

        // Compute color from barycentric coords
        return vec4(fragBarycentric.x * p0Col.xyz + fragBarycentric.y * p1Col.xyz + fragBarycentric.z * p2Col.xyz, 1.);
    }
}

vec4 showColormap() {
    if (layers[0 /* colormap */][3 /* facet */].activated) {
        return getLayerColor(fragFacetIndex, 3 /* facet */);
    } else if (layers[0 /* colormap */][1 /* corner */].activated) {
        return showCornerAttributes();
    } else {
        return vec4(0., 0., 0., -1.);
    }
}


void _filter(inout vec3 col) {
    // Check whether layer filter is activated on facet
    if (!layers[2 /* filter */][3 /* facet */].activated)
        return;

    bool filtered = texelFetch(layerBuffers[2][3], fragFacetIndex).x >= .5;

    if (filtered)
        discard;
}


void clip(inout vec3 col) {
    // Calculate the distance from the cell barycenter to the plane
    vec3 ref_point;

    if (clipping.mode == 0) {
        // Use the barycenter of the facets to exclude facets
        // that are behind the clipping plane
        ref_point = fragBary;
    } else if (clipping.mode == 1) {
        // Use the fragment world position (interpolated) to exclude fragments
        // that are behind the clipping plane
        ref_point = fragWorldPos;
    }

    float distance = dot(clipping.normal, ref_point - clipping.point) / length(clipping.normal);

    if (clipping.invert != distance < 0.0)
        discard;
}

void highlight(inout vec3 col) {
    // Check whether layer highlight on facet is activated
    if (!layers[1 /* hightlight */][3 /* facet */].activated)
        return;

    // Highlight
    float highlightVal = texelFetch(layerBuffers[1][3], fragFacetIndex).x;

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
    float diffuse = max((1.f - dot(light.dir, fragNormal)) * .5f + .45f /* ambiant */, 0.f);
    col = col * diffuse;
}

void wireframe(inout vec3 col) {
    float f1 = 1. - smoothstep(flatFragHeights.x - style.size, flatFragHeights.x, flatFragHeights.x - fragHeights.x);
    float f2 = 1. - smoothstep(flatFragHeights.y - style.size, flatFragHeights.y, flatFragHeights.y - fragHeights.y);
    float f3 = 1. - smoothstep(flatFragHeights.z - style.size, flatFragHeights.z, flatFragHeights.z - fragHeights.z);
    col *= f1 * f2 * f3;
    // col *= min(f1, min(f2, f3));
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
    vec3 col = style.color;

    _filter(col);

    if (clipping.enabled)
        clip(col);

    // Show colormap data if activated
    vec4 b = showColormap();

    if (b.a > -1.) {
        if (b.a > 0.)
            col = b.rgb;
        else 
            discard;
    }

    highlight(col);
    if (light.enabled)
        shading(col);
        
    wireframe(col);

    // Outputs
    fragFacetIndexOut = vec4(encode_id(fragFacetIndex), 1.);
    fragMeshIndexOut = vec4(encode_id(meshIndex), 1.);
    FragColor = vec4(col, 1.f);
}