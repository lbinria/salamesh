#version 440 core

// Avoid the disable of earlier Depth-testing
layout(depth_less) out float gl_FragDepth;

// Color output
layout(location = 0) out vec4 FragColor;
layout(location = 3) out vec4 FragVertexIndexOut;

flat in int FragVertexIndex;

uniform vec3 pointColor;

uniform int colorMode = 0;

uniform sampler1D fragColorMap;
uniform vec2 attrRange = vec2(0.f, 1.f);
uniform samplerBuffer attrBuf;

uniform samplerBuffer filterBuf;
uniform samplerBuffer highlightBuf;

in vec2 vLocalUV;  // u in [0..1] across thickness, v in [0..1] along length

uniform vec4 uColorInside = vec4(0.0, 0.97, 0.73, 1.);
uniform vec4 uColorOutside = vec4(0.0, 0.6, 0.45, 1.);
// uniform vec4 uColorInside = vec4(0.0, 0.97, 0.73, 1.0);
// uniform vec4 uColorOutside = vec4(1.0, 0.0, 0.0, 1.0);

vec3 encode_id(int id) {
    int r = id & 0x000000FF;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    return vec3(r / 255.f, g / 255.f, b / 255.f); 
}

void main()
{
    // dist from border: center is u=0.5
    float d = abs(vLocalUV.x - 0.5) - 0.5;  
    float e = fwidth(d);
    float t = smoothstep(e, -e, d);

    // optional hard discard of outside
    if(t<0.01) discard;

    // Point coord from [0, 1] to [-1, 1]
    vec2 V = 2.0 * (gl_PointCoord - vec2(0.5, 0.5));
    
    // Distance from line center: center is u=0.5
    float dc = abs(vLocalUV.x - 0.5);
    float z = sqrt(dc);
    vec3 N = vec3(V.x, -V.y, z);

    // Update depth according to radius
    gl_FragDepth = gl_FragCoord.z - 0.00005 * N.z;

    // inside = uColorInside, outside = uColorOutside
    // vec4 col = mix(uColorOutside, uColorInside, t);
    float light = 1. - dot(N, vec3(0.35,0.45,1.)) * .5 + .5;
    vec4 col =  mix(uColorOutside, uColorInside, t) * light;

    // FragColor = vec4(col, 1.);
    FragColor = col;
    // FragColor = vec4(0.75, 0.73, 1.0, 1.0); // default white


}