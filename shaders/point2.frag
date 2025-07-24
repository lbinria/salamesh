#version 440 core

layout(depth_less) out float gl_FragDepth;

// Color output
layout(location = 0) out vec4 FragColor;
layout(location = 3) out vec4 FragVertexIndexOut;

flat in int FragVertexIndex;

uniform bool is_light_enabled;

uniform vec3 pointColor;

uniform int colorMode = 0;

// TODO see if uniform can be global to all shaders
uniform sampler1D fragColorMap;
uniform vec2 attributeDataMinMax = vec2(0.f, 1.f);
uniform samplerBuffer attributeData;

uniform samplerBuffer filterBuf;

vec3 encode_id(int id) {
    int r = id & 0x000000FF;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    return vec3(r / 255.f, g / 255.f, b / 255.f); 
}

void main()
{
    bool is_filtered = texelFetch(filterBuf, FragVertexIndex).x > 0;
    if (is_filtered) {
        discard;
    }
    
    vec3 col = pointColor;

    vec2 V = 2.0 * (gl_PointCoord - vec2(0.5, 0.5));
    float one_minus_r2 = 1. - dot(V,V);
    if (one_minus_r2 < 0.0) {
        discard;
    }

    // Compute
    vec3 N = vec3(V.x, -V.y, sqrt(one_minus_r2));
    gl_FragDepth = gl_FragCoord.z - 0.00005 * N.z;
    
    if (colorMode == 0) {
        // Light
        col = col * dot(N, vec3(0.0, 0.0, 1.0));
        FragColor = vec4(col, 1.f);

    } else {

        float fragAttrVal = texelFetch(attributeData, FragVertexIndex).x;
        vec3 col = vec3(texture(fragColorMap, clamp((fragAttrVal - attributeDataMinMax.x) / (attributeDataMinMax.y - attributeDataMinMax.x), 0., 1.)));
        col *= dot(N, vec3(0.0, 0.0, 1.0));
        FragColor = vec4(col, 1.f);
    }

    FragVertexIndexOut = vec4(encode_id(FragVertexIndex), 1.);
}