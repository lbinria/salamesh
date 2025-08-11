#version 440 core

layout (location = 0) in vec3 aP0;
layout (location = 1) in vec3 aP1;
layout (location = 2) in float aSide;
layout (location = 3) in float aEnd;
layout (location = 4) in int vertexIndex;

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
};

uniform mat4 model;
uniform vec2 uViewport = vec2(2560.0, 1440.0); // viewport size in pixels
uniform float uThickness = 10.0; // in pixels



out vec2 vLocalUV; // (u,v) in [0..1] for fragment
flat out int FragVertexIndex;

out float pointSize;
out vec2 nst0;
out vec2 nst1;

void main()
{

    // 1) World→view→clip for both endpoints
    vec4 c0 = projection * view * model * vec4(aP0, 1.0);
    vec4 c1 = projection * view * model * vec4(aP1, 1.0);
    vec4 cb = (c0 + c1) / 2.0;

    // 2) NDC
    vec2 ndc0 = c0.xy / c0.w;
    vec2 ndc1 = c1.xy / c1.w;

    // 3) to screen coords (pixels)
    vec2 s0 = (ndc0 * 0.5 + 0.5) * uViewport;
    vec2 s1 = (ndc1 * 0.5 + 0.5) * uViewport;

    // Bary in screen coords
	vec2 sb = (s0 + s1) / 2.0;


	

    // 4) screen-space direction & perp
    // vec2 dir  = normalize(s1 - s0);
    // vec2 perp = vec2(-dir.y, dir.x);

	// Compute point size
    vec2 dir  = s1 - s0;
	float size = max(abs(dir.x), abs(dir.y));
    pointSize = size /*+ uThickness*/;
	gl_PointSize = pointSize;
	// gl_PointSize = 25;

    // // Get 0,0 screen coordinate of the point square
    // vec2 origin = sb - vec2(pointSize, pointSize) / 2.;
    // float t0 = length((s0 - origin) / pointSize);
    // float t1 = length((s1 - origin) / pointSize);

    // nst0 = ((s1 - sb) / uViewport) * 2. - 1.;
    // nst1 = ((s0 - sb) / uViewport) * 2. - 1.;

    // float clipW0 = mix(c0.w, cb.w, t0);
    // float clipZ0 = mix(c0.z, cb.z, t0);
    // cst0 = vec4(nst0 * clipW0, clipZ0, clipW0);

    // float clipW1 = mix(c1.w, cb.w, t1);
    // float clipZ1 = mix(c1.z, cb.z, t1);
    // cst1 = vec4(nst1 * clipW1, clipZ1, clipW1);
    
    // nst0 = s0 / uViewport * 2. - vec2(1.);
    // nst1 = s1 / uViewport * 2. - vec2(1.);
    // nst0 = (s0 / uViewport - vec2(.5)) * 2.;
    // nst1 = (s1 / uViewport - vec2(.5)) * 2.;
    nst0 = s0;
    nst1 = s1;
    // nst1 = s1 / uViewport;



    // // 5) half thickness in pixels
    // float halfT = uThickness * 0.5;

    // // 6) pick endpoint and move it sideways
    // vec2 base = mix(s0, s1, aEnd);      // if aEnd=0 → s0, if 1→ s1
    // vec2 pos  = base + perp * (aSide * halfT);

    // 7) back to NDC
    // vec2 ndc  = pos / uViewport * 2.0 - 1.0;
    // // 8) reconstruct clip z and w roughly by lerping
    // float clipW = mix(c0.w, c1.w, aEnd);
    // float clipZ = mix(c0.z, c1.z, aEnd);

    // gl_Position = vec4(ndc * clipW, clipZ, clipW);
    gl_Position = cb;






    // // 9) build local UV for shading:
    // //    u = side→ 0 at -1, 1 at +1  ;  v = along the segment
    // vLocalUV = vec2( (aSide + 1.0)*0.5, aEnd );
}