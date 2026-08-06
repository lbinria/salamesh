#version 440 core

// Avoid the disable of earlier Depth-testing
layout(depth_less) out float gl_FragDepth;

// Color output
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragVertexIndexOut;
layout(location = 5) out vec4 fragMeshIndexOut;

flat in int FragVertexIndex;

in vec3 fragWorldPos;

uniform vec3 pointColor;


uniform bool isLightEnabled;

struct Clipping {
    int mode; // {0 = cell, 1 = std, 2 = slice}
    vec3 normal;
    vec3 point;
    bool invert;
    bool enabled;
};

uniform Clipping clipping;

uniform vec3 hoverColor = vec3(1.,1.,1.);
uniform vec3 selectColor = vec3(0., 0.22, 1.);


flat in float depthZ;

uniform int meshIndex;

uniform samplerBuffer layers[3][7];
uniform sampler2D colormaps[7];
uniform int ndims[3][7];
uniform int repeats[3][7];
uniform vec2 ranges[3][7];
uniform bool activateds[3][7];


vec3 encode_id(int id) {
    int r = id & 0x000000FF;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    return vec3(r / 255.f, g / 255.f, b / 255.f); 
}

float fetchLayer(int idx, const int kind) {
    return texelFetch(layers[0][kind], idx).x;
    // if (kind == 0) return texelFetch(layers[0][0], idx).x;
    // if (kind == 1) return texelFetch(layers[0][1], idx).x;
    // if (kind == 2) return texelFetch(layers[0][2], idx).x;
    // if (kind == 3) return texelFetch(layers[0][kind], idx).x;
    // if (kind == 4) return texelFetch(layers[0][4], idx).x;
    // if (kind == 5) return texelFetch(layers[0][5], idx).x;
    // if (kind == 6) return texelFetch(layers[0][6], idx).x;
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

void _filter(inout vec3 col) {
    // Check whether layer filter is activated on facet
    if (!activateds[2 /* filter */][0 /* point */])
        return;

    bool filtered = texelFetch(layers[2][0], FragVertexIndex).x >= .5;

    if (filtered)
        discard;
}

void clip(inout vec3 col) {
    // Calculate the distance from the cell barycenter to the plane
    float distance = dot(clipping.normal, fragWorldPos - clipping.point) / length(clipping.normal);

    if (clipping.invert != distance < 0.0) {
        discard;
    }
}

vec3 trace(inout vec3 col) {
    // Check circle distance function
    vec2 V = 2.0 * (gl_PointCoord - vec2(0.5, 0.5));
    float oneMinusR2 = 1. - dot(V,V);
    if (oneMinusR2 < 0.0) {
        discard;
    }

    // Compute sphere radius in frag coords unit
    float deltaDepth = abs(gl_FragCoord.z - depthZ);

    // Compute normal using point coordinates and radius as Z
    vec3 N = vec3(V.x, -V.y, sqrt(oneMinusR2));
    // Update depth according to radius
    gl_FragDepth = gl_FragCoord.z - deltaDepth * N.z;

    return N;
}

void highlight(inout vec3 col) {
    // Check whether layer highlight on point is activated
    if (!activateds[1 /* hightlight */][0 /* point */])
        return;

    // Highlight
    float highlightVal = texelFetch(layers[1][0], FragVertexIndex).x;

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

void shading(vec3 N, inout vec3 col) {
    // Diffuse light
    if (isLightEnabled) {
        vec3 dirLight;

        dirLight = vec3(-0.5f, -0.8f, 0.2f);
        
        float diffuse = max((1.f - dot(N, dirLight)) * .75f + .25f /* ambiant */, 0.f);
        col = col * diffuse;
    }
}

vec4 getLayerColor(int idx, int kind) {

    vec2 range = ranges[0][kind];
    int nRepeat = repeats[0][kind];
    int nDims = ndims[0][kind];

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
}

vec4 showColormap() {
    if (activateds[0 /* colormap */][0 /* point */]) {
        return getLayerColor(FragVertexIndex, 0 /* point */);
    } else {
        return vec4(0., 0., 0., -1.);
    }
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
    vec3 col = pointColor;
    _filter(col);
    if (clipping.enabled)
        clip(col);
        
    vec3 N = trace(col);

    // Show colormap data if activated
    vec4 b = showColormap();

    if (b.a > -1.) {
        if (b.a > 0.)
            col = b.rgb;
        else 
            discard;
    }

    highlight(col);
    shading(N, col);

    FragVertexIndexOut = vec4(encode_id(FragVertexIndex), 1.);
    fragMeshIndexOut = vec4(encode_id(meshIndex), 1.);
    FragColor = vec4(col, 1.f);
}