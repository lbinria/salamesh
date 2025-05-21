#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 TexCoord[];
out vec2 fragTexCoord;

in vec3 ourColor[];
out vec3 fragColor;

out vec3 dist;

void main() {

	// Bary
	vec3 b = vec3(0.);
	for (int i = 0; i < 3; i++) {
		b += vec3(gl_in[i].gl_Position);
	}
	b /= 3.f;

	// Compute side lengths
	float a = length(vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position));
	float c = length(vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position));
	float d = length(vec3(gl_in[0].gl_Position) - vec3(gl_in[2].gl_Position));
	float s = a + c + d;
	float area = sqrt(s * (s - a) * (s - c) * (s - d));

	// 1/2*b*h=area => h = area/(.5*b)
	float h[3] = float[3](area / (.5f * a), area / (.5f * c), area / (.5f * d));


	for (int i = 0; i < 3; i++) {
		vec3 d = vec3(gl_in[i].gl_Position) - b;
		gl_Position = gl_in[i].gl_Position - vec4(d * .01f, 0.);
		fragTexCoord = TexCoord[i];
		fragColor = ourColor[i];

		if (i == 0)
			dist = vec3(0, h[1], 0);
		else if (i == 1)
			dist = vec3(0, 0, h[2]);
		else 
			dist = vec3(h[0], 0, 0);

	    EmitVertex();
	}
    
    EndPrimitive();
}    