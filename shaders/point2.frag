#version 440 core

// Color output
out vec4 FragColor;

uniform bool is_light_enabled;

uniform mat4 inverse_projection_view_model;
uniform mat4 model_view_projection_matrix;

// Point
flat in float fragSize;

void main()
{
	vec3 col = vec3(0.f, 0.f, 0.f);

    vec2 V = 2.0 * (gl_PointCoord - vec2(0.5, 0.5));
    float one_minus_r2 = 1. - dot(V,V);
    if (one_minus_r2 < 0.0) {
        discard;
    }

    vec3 N = vec3(V.x, -V.y, sqrt(one_minus_r2));
    // gl_FragDepth = gl_FragCoord.z- 1.;
    // gl_FragDepth = gl_FragCoord.z - 0.00005 * N.z;
    gl_FragDepth = gl_FragCoord.z - 0.001 * N.z;

	col = vec3(0.8, 0.2, 0.2) * dot(N, vec3(0.0, 0.0, 1.0));
	FragColor = vec4(col, 1.f);
}