#version 440 core

layout(location = 0) out vec4 FragColor;
layout(location = 3) out vec4 fragFacetIndexOut;
layout(location = 5) out vec4 fragMeshIndexOut;

// Primitive indexation
flat in int fragCornerIndex;
flat in int fragCornerOff;
flat in int fragFacetIndex;

in vec3 fragBary;
in vec3 fragNormal;
in vec3 fragHeights;
flat in vec3 flatFragHeights;

in vec3 fragWorldPos;
in vec3 fragBarycentric;

uniform bool is_light_enabled;
uniform vec3 lightDir = vec3(-0.5f, -0.8f, 0.2f);

uniform vec3 color;
uniform float meshSize;

uniform int clipping_mode = 1; // 0: cell, 1: std, 2: slice
uniform bool is_clipping_enabled = false;
uniform vec3 clipping_plane_normal;
uniform vec3 clipping_plane_point;
uniform int invert_clipping = 0; // 0: normal, 1: inverted

// uniform vec3 hoverColor = vec3(1.,1.,1.);
// uniform vec3 selectColor = vec3(0., 0.22, 1.);

// // Note: cannot index samplerBuffer with dynamic indexing !
// uniform sampler2D colormap0;
// uniform sampler2D colormap1;
// uniform sampler2D colormap2;


// uniform bool isCornerVisible;

// uniform int meshIndex;

// flat in int surfaceType;

// // Note: cannot index samplerBuffer with dynamic indexing !
// uniform samplerBuffer filterBuf;
// uniform samplerBuffer highlightBuf;
// uniform samplerBuffer colormap0Buf;
// uniform samplerBuffer colormap1Buf;
// uniform samplerBuffer colormap2Buf;

// uniform vec2 attrRange[3];
// uniform int attrRepeat[3] = {1, 1, 1};
// uniform int attrNDims[3] = {1, 1, 1};

// uniform int colormapElement[3] = {-1, -1, -1};
// uniform int highlightElement;
// uniform int filterElement;
// uniform bool activatedLayers[5 /* layer */][7 /* element kind */];

void clip(inout vec3 col) {
	// Calculate the distance from the cell barycenter to the plane

	vec3 ref_point;
	if (clipping_mode == 0) {
		// Use the barycenter of the facets to exclude facets
		// that are behind the clipping plane
		ref_point = fragBary;
	} else if (clipping_mode == 1) {
		// Use the fragment world position (interpolated) to exclude fragments
		// that are behind the clipping plane
		ref_point = fragWorldPos;
	}

		float distance = dot(clipping_plane_normal, ref_point - clipping_plane_point) / length(clipping_plane_normal);
		
		if ((invert_clipping == 0 && distance < 0.0) || (invert_clipping == 1 && distance >= 0.0)) {
			discard;
		}
}

void shading(inout vec3 col) {
	float diffuse = max((1.f - dot(lightDir, fragNormal)) * .5f + .45f /* ambiant */, 0.f);
	col *= diffuse;
}

void wireframe(inout vec3 col) {
	float f1 = 1. - smoothstep(flatFragHeights.x - meshSize, flatFragHeights.x, flatFragHeights.x - fragHeights.x);
	float f2 = 1. - smoothstep(flatFragHeights.y - meshSize, flatFragHeights.y, flatFragHeights.y - fragHeights.y);
	float f3 = 1. - smoothstep(flatFragHeights.z - meshSize, flatFragHeights.z, flatFragHeights.z - fragHeights.z);
	col *= f1 * f2 * f3;
	// col *= min(f1, min(f2, f3));
}


void main()
{
	vec3 col = color;
    // _filter(col);
	if (is_clipping_enabled)
    	clip(col);

	if (is_light_enabled)
		shading(col);

	wireframe(col);

	// Outputs
	FragColor = vec4(col, 1.f);
}