#version 440 core

// Avoid the disable of earlier Depth-testing
layout(depth_less) out float gl_FragDepth;

// Color output
layout(location = 0) out vec4 FragColor;
layout(location = 3) out vec4 FragVertexIndexOut;

flat in int FragVertexIndex;

in vec3 fragWorldPos;

uniform vec3 pointColor;

flat in vec3 fragViewDir;

uniform bool is_light_enabled;
uniform bool is_light_follow_view;

uniform bool is_clipping_enabled = false;
uniform vec3 clipping_plane_normal; // (a, b, c)
uniform vec3 clipping_plane_point;  // A point on the plane
uniform int invert_clipping = 0; // 0: normal, 1: inverted

uniform vec3 hoverColor = vec3(1.,1.,1.);
uniform vec3 selectColor = vec3(0., 0.22, 1.);

uniform int colorMode = 0;

// Note: cannot index samplerBuffer with dynamic indexing !
uniform sampler2D colormap0;
uniform sampler2D colormap1;
uniform sampler2D colormap2;

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

flat in float depthZ;

vec3 encode_id(int id) {
    int r = id & 0x000000FF;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    return vec3(r / 255.f, g / 255.f, b / 255.f); 
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

void _filter(inout vec3 col) {
    if (filterElement == -1)
        return;

    bool filtered = texelFetch(filterBuf, FragVertexIndex).x > 0;

    if (filtered)
        discard;
}

void clip(inout vec3 col) {
   // Calculate the distance from the cell barycenter to the plane
   if (is_clipping_enabled) {
      float distance = dot(clipping_plane_normal, fragWorldPos - clipping_plane_point) / length(clipping_plane_normal);
      
      if ((invert_clipping == 0 && distance < 0.0) || (invert_clipping == 1 && distance >= 0.0)) {
         discard;
      }
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
    if (highlightElement == -1)
        return;

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
}

void shading(vec3 N, inout vec3 col) {
    // Diffuse light
    if (is_light_enabled) {
        vec3 dirLight;

        if (is_light_follow_view)
            dirLight = fragViewDir;
        else
            dirLight = vec3(-0.5f, -0.8f, 0.2f);
        
        float diffuse = max((1.f - dot(N, dirLight)) * .75f + .25f /* ambiant */, 0.f);
        col = col * diffuse;
    }
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
}

vec4 showColormap(int layer) {
    int kind = colormapElement[layer];
    
    if (kind == 1 /* points */) {
        return getLayerColor(FragVertexIndex, layer);
    } 
    
    if (kind == -1 /* no element => layer deactivated */) {
        return vec4(0., 0., 0., -1.);
    }

    return vec4(1., 0., 0., 1.);
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
    clip(col);
    vec3 N = trace(col);

    // Show colormap data if activated
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
    shading(N, col);

    FragVertexIndexOut = vec4(encode_id(FragVertexIndex), 1.);
    FragColor = vec4(col, 1.f);
}