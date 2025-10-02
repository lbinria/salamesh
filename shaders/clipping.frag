#version 330 core
in vec3 vWorldPos;
out vec4 fragColor;

uniform bool is_clipping_enabled;
uniform vec3 clipping_plane_normal; // not necessarily normalized
uniform vec3 clipping_plane_point;
uniform int invert_clipping; // 0 or 1
uniform vec4 planeColor = vec4(1.0, 1.0, 1.0, 0.2); // rgba, alpha < 1 for transparency

void main() {
    vec3 n = normalize(clipping_plane_normal);
    if (invert_clipping == 1) n = -n;
    float d = dot(n, vWorldPos - clipping_plane_point);
    // decide which side to keep: keep fragments where d <= 0 (inside) for example
    if (is_clipping_enabled) {
        // If you want to show the interior (keep inside), discard fragments that are outside
        // Adjust the test sign as required by your coordinate convention.
        if (d > 0.0) discard;
    }
    // Optional: smooth the plane edge with a soft falloff near the plane (small epsilon)
    fragColor = planeColor;
}