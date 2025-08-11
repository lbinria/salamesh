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

uniform vec3 uColorInside = vec3(0.0, 0.97, 0.73);
uniform vec3 uColorOutside = vec3(0.0, 0.6, 0.45);

in float pointSize;
in vec2 nst0;
in vec2 nst1;


vec3 encode_id(int id) {
    int r = id & 0x000000FF;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    return vec3(r / 255.f, g / 255.f, b / 255.f); 
}

float sdfEquilateralTriangle(vec2 p) {
    float k = sqrt(3.);
    p.x = abs(p.x) - 1.0;
    p.y = p.y + 1.0 / k;
    if (p.x+k*p.y > 0) p = vec2(p.x - k*p.y, -k*p.x-p.y) / 2.;
    p.x -= clamp(p.x, -2., 0.);
    return -length(p)*sign(p.y);
}

// SDF to a thick line (capsule) from point a to point b in 2D
float sdThickLine(vec2 p, vec2 a, vec2 b, float r) {
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    vec2 proj = a + ba * h;
    return length(p - proj) - r;
}

void main()
{

    // Point coord from [0, 1] to [-1, 1]
	// Re-center
    vec2 V = 2.0 * (gl_PointCoord - vec2(0.5, 0.5));
	

    float sdf = sdThickLine(gl_FragCoord.xy, nst0, nst1, 1.);
    if (sdf < 0) {

        // Distance from line center: center is u=0.5
        float dc = abs(sdf / pointSize);
        float z = sqrt(dc);
        vec3 N = vec3(V.x, -V.y, z);

        // Update depth according to radius
        // gl_FragDepth = gl_FragCoord.z - 0.00005 * N.z;

        FragColor = vec4(0.08, 1.0, 0.0, 1.0);
        // FragColor = vec4(0.08, 1.0, 0.0, 1.0) * (1. - dot(N, vec3(0.45, 0.32, 1.)));
    } else 
        discard;
    
}