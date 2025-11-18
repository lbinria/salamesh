#version 440 core

// Avoid the disable of earlier Depth-testing
layout(depth_less) out float gl_FragDepth;

// Color output
layout(location = 0) out vec4 FragColor;
layout(location = 3) out vec4 FragVertexIndexOut;

flat in int FragHalfedgeIndex;

uniform vec3 pointColor;

uniform int colorMode = 0;

uniform vec3 hoverColor = vec3(1.,1.,1.);
uniform vec3 selectColor = vec3(0., 0.22, 1.);

uniform sampler1D fragColorMap;
uniform vec2 attrRange = vec2(0.f, 1.f);
uniform samplerBuffer attrBuf;

uniform samplerBuffer filterBuf;
uniform samplerBuffer highlightBuf;

in vec2 vLocalUV;  // u in [0..1] across thickness, v in [0..1] along length

uniform vec3 uColorInside = vec3(0.0, 0.97, 0.73);
uniform vec3 uColorOutside = vec3(0.0, 0.6, 0.45);

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

void main()
{
    // Check whether point is filtered
    bool isFiltered = texelFetch(filterBuf, FragHalfedgeIndex).x > 0;
    if (isFiltered)
        discard;

    // dist from border: center is u=0.5
    float d = abs(vLocalUV.x - 0.5) - 0.5;  
    float e = fwidth(d);
    float t = smoothstep(e, -e, d);

    // optional hard discard of outside
    if(t<0.01) discard;

    // vLocalUV is between (0,0) - (1,1) in a square
    // We want to discard outside of circle
    // vec2 uv = vLocalUV * 2. - vec2(1.);
    // if (length(uv) - 1. > 0)
    //     discard;

    // Point coord from [0, 1] to [-1, 1]
    vec2 V = 2.0 * (gl_PointCoord - vec2(0.5, 0.5));
    
    // Distance from line center: center is u=0.5
    float dc = abs(vLocalUV.x - 0.5);
    float z = sqrt(dc);
    vec3 N = vec3(V.x, -V.y, z);

    // Update depth according to radius
    gl_FragDepth = gl_FragCoord.z - 0.00005 * N.z;

    
    // vec3 col = mix(uColorOutside, uColorInside, t);
    float light = 1. - dot(N, vec3(0.35,0.45,1.)) /** .5 + .5*/;
    vec3 col = mix(uColorOutside, uColorInside, t) * (light * 0.5 + 0.5);
    
    // Highlight
    float highlightVal = texelFetch(highlightBuf, FragHalfedgeIndex).x;

    if (highlightVal > 0) {
        // Interpolate between hover / select colors according to highlight value
        float t = highlightVal;
        // Add condition if you want a hard switch between hover and select
        t = step(0.5, highlightVal);
        vec3 hlColor = mix(hoverColor, selectColor, t);
        // Mix with current point color (80%)
        col = mix(col, hlColor, .8);
    }

    FragColor = vec4(col, 1.);

    // FragColor = vec4(0.75, 0.73, 1.0, 1.0); // default white


}