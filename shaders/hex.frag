#version 440 core

// Color output
out vec4 FragColor;
// Primitive indexation
flat in int fragCellIndex;
flat in int fragFacetIndex;
flat in int fragEdgeIndex;

in vec3 fragNormal;
in vec3 fragHeights;
in float fragAttrVal;
in float fragHighlight;
in float fragFilter;

flat in vec3 fragWorldPos;

uniform bool is_light_enabled;
uniform bool is_clipping_enabled = false;

uniform int fragRenderMode = 0;
uniform int fragRenderMeshMode = 2;
uniform float meshSize;

uniform vec3 planeNormal = vec3(0.2f, 0.6f, 0.0f); // (a, b, c)
uniform vec3 planePoint = vec3(0.0f, 0.3f, 0.60);  // A point on the plane

uniform sampler1D fragColorMap;
uniform vec2 attributeDataMinMax = vec2(0.f, 1.f);
uniform int colorMode = 0;

uniform vec3 point = vec3(0.0f, 0.0f, 0.0f); // Point to check distance from

// // All components are in the range [0…1], including hue.
// vec3 rgb2hsv(vec3 c)
// {
//     vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
//     vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
//     vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

//     float d = q.x - min(q.w, q.y);
//     float e = 1.0e-10;
//     return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
// }

// // All components are in the range [0…1], including hue.
// vec3 hsv2rgb(vec3 c)
// {
//     vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
//     vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
//     return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
// }


vec3 encode_id(int id) {
    int r = id & 0x000000FF;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    return vec3(r / 255.f, g / 255.f, b / 255.f); 
}

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{

    vec3 col = vec3(0.f, 0.f, 0.f);

    if (fragFilter >= .5) {
        discard;
        return;
    }

    // Render mode color
    if (fragRenderMode == 0) {


        if (colorMode == 0) {
            col = vec3(0.8f, 0.f, 0.2f);
        } 
        else {
            col = vec3(texture(fragColorMap, clamp((fragAttrVal - attributeDataMinMax.x) / (attributeDataMinMax.y - attributeDataMinMax.x), 0., 1.)));
        }
        
        // Clipping
        
        // Calculate the distance from the fragment to the plane
        if (is_clipping_enabled) {
            float distance = dot(planeNormal, fragWorldPos - planePoint) / length(planeNormal);
            
            if (distance < 0.0) {
                discard; // Discard the fragment if it's behind the clipping plane
            }
        }


        
        
        if (fragHighlight >= 1.f && fragHighlight < 2.f) {
            col = mix(col, vec3(1.,1.,1.), 0.8);
        }
        else if (fragHighlight >= 2.f) {
            // a between 0.5 and 0.75 - 0.25 range
            // col = mix(col, vec3(0., 0.22, 1.), 0.5 + fragHighlight * 0.25);
            col = mix(col, vec3(0., 0.22, 1.), 0.8);
        }

        // Diffuse light
        if (is_light_enabled) {
            vec3 dirLight = vec3(-0.5f, -0.8f, 0.2f);
            float diffuse = (1.f - dot(dirLight, fragNormal)) * .5f + .5f;
            col = col * diffuse;
        }

    // Render mode facet index
    } else if (fragRenderMode == 2) {
        col = encode_id(fragFacetIndex);
    // Render mode cell index
    } else if (fragRenderMode == 3)
        col = encode_id(fragCellIndex);


    if ((fragRenderMeshMode & 2) == 2) {
        // Render wireframe
        if (fragHeights.y < meshSize || fragHeights.z < meshSize) {
            col = vec3(0,0,0);
        }
    }

    // vec3 depth = vec3(LinearizeDepth(gl_FragCoord.z)); 

    if (distance(fragWorldPos, point) < 0.02) {
        col = vec3(1.f, 1.f, 3.f);
    }

    FragColor = vec4(col, 1.f);
}