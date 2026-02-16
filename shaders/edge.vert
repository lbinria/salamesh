#version 440 core

layout (location = 0) in vec3 aP0;
layout (location = 1) in vec3 aP1;
layout (location = 2) in float aSide;
layout (location = 3) in float aEnd;
layout (location = 4) in int halfedgeIndex;
layout (location = 5) in vec3 bary;

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
    vec2 viewport;
};

uniform mat4 model;
uniform float thickness; // in pixels
uniform float spacing;
uniform float padding;

out vec2 vLocalUV; // (u,v) in [0..1] for fragment
flat out int FragHalfedgeIndex;
out vec3 fragWorldPos;


void main()
{

    
    // Padding is the space ratio between the border of facet and its barycenter
    // It's a sort of shrinking
    // 0 => line are on the facet's border
    // 1 => line are on the facet's barycenter
    vec3 p0 = aP0 - (aP0 - bary) * padding;
    vec3 p1 = aP1 - (aP1 - bary) * padding;

    // Spacing is the space between line extremities and points
    // 0 => line extremities lying to points
    // 1 => line extremities lying to the center of line
    vec3 d = p1 - p0;
    p0 += d * spacing * .5;
    p1 -= d * spacing * .5;

    // 1) World→view→clip for both endpoints
    vec4 c0 = projection * view * model * vec4(p0, 1.0);
    vec4 c1 = projection * view * model * vec4(p1, 1.0);

    // 2) NDC
    vec2 ndc0 = c0.xy / c0.w;
    vec2 ndc1 = c1.xy / c1.w;

    // 3) to screen coords (pixels)
    vec2 s0 = (ndc0 * 0.5 + 0.5) * viewport;
    vec2 s1 = (ndc1 * 0.5 + 0.5) * viewport;

    // 4) screen-space direction & perp
    vec2 dir  = normalize(s1 - s0);
    vec2 perp = vec2(-dir.y, dir.x);

    // 5) half thickness in pixels
    float halfT = thickness * 0.5;

    // 6) pick endpoint and move it sideways
    vec2 base = mix(s0, s1, aEnd);      // if aEnd=0 → s0, if 1→ s1
    vec2 pos  = base + perp * (aSide * halfT);

    // 7) back to NDC
    vec2 ndc  = pos / viewport * 2.0 - 1.0;

    // 8) reconstruct clip z and w roughly by lerping
    float clipW = mix(c0.w, c1.w, aEnd);
    float clipZ = mix(c0.z, c1.z, aEnd);

    gl_Position = vec4(ndc * clipW, clipZ, clipW);

    // // 9) build local UV for shading:
    // //    u = side→ 0 at -1, 1 at +1  ;  v = along the segment
    vLocalUV = vec2( (aSide + 1.0)*0.5, aEnd );

    fragWorldPos = aP0;
    FragHalfedgeIndex = halfedgeIndex;
}