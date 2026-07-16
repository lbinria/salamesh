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

uniform bool isLightEnabled;
uniform vec3 lightDir = vec3(-0.5f, -0.8f, 0.2f);

uniform vec3 color;
uniform float meshSize;

uniform int clippingMode = 1; // 0: cell, 1: std, 2: slice
uniform bool isClippingEnabled = false;
uniform vec3 clippingPlaneNormal;
uniform vec3 clippingPlanePoint;
uniform int invertClipping = 0; // 0: normal, 1: inverted

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
	if (clippingMode == 0) {
		// Use the barycenter of the facets to exclude facets
		// that are behind the clipping plane
		ref_point = fragBary;
	} else if (clippingMode == 1) {
		// Use the fragment world position (interpolated) to exclude fragments
		// that are behind the clipping plane
		ref_point = fragWorldPos;
	}

	float distance = dot(clippingPlaneNormal, ref_point - clippingPlanePoint) / length(clippingPlaneNormal);
	
	if ((invertClipping == 0 && distance < 0.0) || (invertClipping == 1 && distance >= 0.0)) {
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
	if (isClippingEnabled)
    	clip(col);

	if (isLightEnabled)
		shading(col);

	wireframe(col);

	// Outputs
	FragColor = vec4(col, 1.f);
}