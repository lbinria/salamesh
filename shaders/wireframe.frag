#version 440 core

// Color output
out vec4 FragColor;
uniform float meshSize;


void main()
{

    vec3 col = vec3(0.f, 1.f, 0.f);

    FragColor = vec4(col, 1.f);

}