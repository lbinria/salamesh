#version 450 core

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

struct Light {
	vec3 dir;
	bool enabled;
};

uniform Light light;

struct Clipping {
    int mode; // {0 = cell, 1 = std, 2 = slice}
    vec3 normal;
    vec3 point;
    bool invert;
    bool enabled;
};

uniform Clipping clipping;


struct Style {
   vec3 color;
   float size;
   float shrink;
   bool corner_visible;
};

uniform Style style;

// uniform vec3 hoverColor = vec3(1.,1.,1.);
// uniform vec3 selectColor = vec3(0., 0.22, 1.);

// Note: cannot index samplerBuffer with dynamic indexing !
uniform sampler2D colormap0;
uniform sampler2D colormap1;
uniform sampler2D colormap2;

// Note: cannot index samplerBuffer with dynamic indexing !
uniform samplerBuffer filterBuf;
uniform samplerBuffer highlightBuf;
uniform samplerBuffer colormap0Buf;
uniform samplerBuffer colormap1Buf;
uniform samplerBuffer colormap2Buf;

// uniform int meshIndex;

// flat in int surfaceType;



// uniform vec2 attrRange[3];
// uniform int attrRepeat[3] = {1, 1, 1};
// uniform int attrNDims[3] = {1, 1, 1};

// uniform int colormapElement[3] = {-1, -1, -1};
// uniform int highlightElement;
// uniform int filterElement;
// uniform bool activatedLayers[5 /* layer */][7 /* element kind */];

float fetchLayer(int idx, int layer) {
	if (layer == 0) return texelFetch(colormap0Buf, idx).x;
	if (layer == 1) return texelFetch(colormap1Buf, idx).x;
	if (layer == 2) return texelFetch(colormap2Buf, idx).x;
	if (layer == 3) return texelFetch(highlightBuf, idx).x;
	if (layer == 4) return texelFetch(filterBuf, idx).x;
	return 0.;
}

vec4 fetchColormap(int layer, vec2 coords) {
	if (layer == 0) return texture(colormap0, coords);
	if (layer == 1) return texture(colormap1, coords);
	if (layer == 2) return texture(colormap2, coords);
	return vec4(0.);
}

// vec4 getLayerColor(int idx, int layer) {
// 	vec2 range = attrRange[layer];
// 	int nRepeat = attrRepeat[layer];
// 	int nDims = attrNDims[layer];
// 	// bool automap = getLayerAutomap(layer);

// 	float rangeLength = range.y - range.x;
// 	float rangeRepeat = rangeLength / nRepeat;

// 	vec2 coords = vec2(0.);
// 	for (int d = 0; d < nDims; d++) {

// 		float val = fetchLayer(idx * nDims + d, layer).x;
// 		if (nDims == 1) {
// 			// Apply range
// 			// val = (mod(attrVal - attrRange.x, rangeRepeat + 1)) / rangeRepeat;
// 			val = (val - range.x) / rangeLength; // Simple auto map range version with no repeat
// 		}
		
// 		float v = clamp(val, 0., 1.);
// 		coords[d] = v;
// 	}

// 	return fetchColormap(layer, coords);
// }

void clip(inout vec3 col) {
	// Calculate the distance from the cell barycenter to the plane

	vec3 ref_point;
	if (clipping.mode == 0) {
		// Use the barycenter of the cells to exclude cells
		// that are behind the clipping plane
		ref_point = fragBary;
	} else if (clipping.mode == 1) {
		// Use the fragment world position (interpolated) to exclude fragments
		// that are behind the clipping plane
		ref_point = fragWorldPos;
	}

	float distance = dot(clipping.normal, ref_point - clipping.point) / length(clipping.normal);
	
	// if ((!clipping.invert && distance < 0.0) || (clipping.invert && distance >= 0.0)) {
	// 	discard;
	// }
	if (clipping.invert != distance < 0.)
		discard;
}

void shading(inout vec3 col) {
	float diffuse = max((1.f - dot(light.dir, fragNormal)) * .5f + .45f /* ambiant */, 0.f);
	col *= diffuse;
}

void wireframe(inout vec3 col) {
	float f1 = 1. - smoothstep(flatFragHeights.x - style.size, flatFragHeights.x, flatFragHeights.x - fragHeights.x);
	float f2 = 1. - smoothstep(flatFragHeights.y - style.size, flatFragHeights.y, flatFragHeights.y - fragHeights.y);
	float f3 = 1. - smoothstep(flatFragHeights.z - style.size, flatFragHeights.z, flatFragHeights.z - fragHeights.z);
	col *= f1 * f2 * f3;
	// col *= min(f1, min(f2, f3));
}


void main()
{
	vec3 col = style.color;
    // _filter(col);
	if (clipping.enabled)
    	clip(col);

	if (light.enabled)
		shading(col);

	wireframe(col);

	// Outputs
	FragColor = vec4(col, 1.f);
}