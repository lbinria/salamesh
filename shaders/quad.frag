#version 440 core
out vec4 FragColor;

in vec3 ourColor;
in float TexCoord;
in vec3 heights;
in float ourFacetIndex;

uniform sampler1D ourTexture;
uniform float meshSize;

in vec3 fragWorldPos; // Input from vertex shader
uniform vec3 planeNormal = vec3(0.2f, 0.6f, 0.0f); // (a, b, c)
uniform vec3 planePoint = vec3(0.0f, 0.3f, 0.60);  // A point on the plane

void main()
{




    // FragColor = texture(ourTexture, TexCoord);
    // FragColor = texture(ourTexture, ourFacetIndex / 229956.);
    FragColor = texture(ourTexture, clamp(ourFacetIndex, 0., 1.));
    // FragColor = texture(ourTexture, clamp(1.0, 0., 1.));
    
    


    if (heights.y < meshSize || heights.z < meshSize)
        FragColor = vec4(0,0,0,1.);

    
    // Clipping
    
    // Calculate the distance from the fragment to the plane
    float distance = dot(planeNormal, fragWorldPos - planePoint) / length(planeNormal);
    
    /*
    if (distance < 0.0) {
        discard; // Discard the fragment if it's behind the clipping plane
    }
    */

    // Diffuse light

}