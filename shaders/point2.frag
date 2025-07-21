#version 440 core

// Color output
layout(location = 0) out vec4 FragColor;
layout(location = 3) out vec4 FragVertexIndexOut;

flat in int FragVertexIndex;

uniform bool is_light_enabled;

uniform vec3 pointColor;

vec3 encode_id(int id) {
    int r = id & 0x000000FF;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    return vec3(r / 255.f, g / 255.f, b / 255.f); 
}

void main()
{
	vec3 col = pointColor;

    vec2 V = 2.0 * (gl_PointCoord - vec2(0.5, 0.5));
    float one_minus_r2 = 1. - dot(V,V);
    if (one_minus_r2 < 0.0) {
        discard;
    }

    // Compute
    vec3 N = vec3(V.x, -V.y, sqrt(one_minus_r2));

    // I don't know which value choose
    // gl_FragDepth = gl_FragCoord.z - 1. * N.z;
    gl_FragDepth = gl_FragCoord.z - 0.00005 * N.z;
    // gl_FragDepth = gl_FragCoord.z - 0.000005 * N.z;
    // gl_FragDepth = gl_FragCoord.z - 0.001 * N.z;

    // Light
    col = col * dot(N, vec3(0.0, 0.0, 1.0));
    FragColor = vec4(col, 1.f);
    FragVertexIndexOut = vec4(encode_id(FragVertexIndex), 1.f);
}