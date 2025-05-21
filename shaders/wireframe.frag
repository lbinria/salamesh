#version 440 core

// Color output
out vec4 FragColor;
// // Primitive indexation
// flat in int fragCellIndex;
// flat in int fragFacetIndex;

// in vec3 fragHeights;
// in float fragAttrVal;
// in float fragHighlight;
// in float fragFilter;

// uniform bool is_light_enabled;
// uniform bool is_clipping_enabled = false;

// uniform int fragRenderMode = 0;
uniform float meshSize;

// uniform vec3 planeNormal = vec3(0.2f, 0.6f, 0.0f); // (a, b, c)
// uniform vec3 planePoint = vec3(0.0f, 0.3f, 0.60);  // A point on the plane

// uniform sampler1D fragColorMap;
// uniform vec2 attributeDataMinMax = vec2(0.f, 1.f);
// uniform int colorMode = 0;

// vec3 encode_id(int id) {
//     int r = id & 0x000000FF;
//     int g = (id & 0x0000FF00) >> 8;
//     int b = (id & 0x00FF0000) >> 16;
//     return vec3(r / 255.f, g / 255.f, b / 255.f); 
// }

void main()
{

    vec3 col = vec3(0.f, 1.f, 0.f);

    // if (fragFilter >= .5) {
    //     discard;
    //     return;
    // }

    // if (fragRenderMode == 0) {


    //     // Render wireframe
    //     if (fragHeights.y < meshSize || fragHeights.z < meshSize) {
    //         col = vec3(0,0,0);
    //     } else {
	// 		// col = vec3(1.,1.,0.);
	// 		discard;
	// 	}

    //     // if (colorMode == 0) {
    //     //     col = vec3(0.8f, 0.f, 0.2f);
    //     // } 
    //     // else {
    //     //     col = vec3(texture(fragColorMap, clamp((fragAttrVal - attributeDataMinMax.x) / (attributeDataMinMax.y - attributeDataMinMax.x), 0., 1.)));
    //     // }
        
    //     // // Clipping
        
    //     // // Calculate the distance from the fragment to the plane
    //     // if (is_clipping_enabled) {
    //     //     float distance = dot(planeNormal, fragWorldPos - planePoint) / length(planeNormal);
            
    //     //     if (distance < 0.0) {
    //     //         discard; // Discard the fragment if it's behind the clipping plane
    //     //     }
    //     // }
        
        
    //     // if (fragHighlight > 0.1f && fragHighlight < 0.5f) {
    //     //     col = mix(col, vec3(1.,1.,1.), 0.8);
    //     // }
    //     // else if (fragHighlight >= 0.5f) {
    //     //     // a between 0.5 and 0.75 - 0.25 range
    //     //     // col = mix(col, vec3(0., 0.22, 1.), 0.5 + fragHighlight * 0.25);
    //     //     col = mix(col, vec3(0., 0.22, 1.), 0.8);
    //     // }

    //     // // Diffuse light
    //     // if (is_light_enabled) {
    //     //     vec3 dirLight = vec3(-0.5f, -0.8f, 0.2f);
    //     //     float diffuse = (1.f - dot(dirLight, fragNormal)) * .5f + .5f;
    //     //     col = col * diffuse;
    //     // }



    // } else if (fragRenderMode == 2) {
    //     col = encode_id(fragFacetIndex);
    // } else if (fragRenderMode == 3)
    //     col = encode_id(fragCellIndex);

    FragColor = vec4(col, 1.f);

}