#version 440 core
out vec4 FragColor;

in vec3 fragColor;
in vec2 fragTexCoord;

in vec3 dist;

uniform sampler2D ourTexture;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    float minComponent = min(dist.x, min(dist.y, dist.z));

    FragColor = texture(ourTexture, fragTexCoord) * vec4(fragColor, 1.0f);
    // FragColor = vec4(ourColor, 1.0f);
    // float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    // FragColor = vec4(vec3(depth), 1.0);

    if (minComponent < 0.02)
        FragColor = vec4(0,0,0,1.);

}