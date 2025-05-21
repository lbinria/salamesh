#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float size;
layout (location = 6) in vec3 normal;
layout (location = 3) in vec3 aHeights;
// Indexes of the primitive this vertices belongs to
layout (location = 4) in int facetIndex;
layout (location = 5) in int cellIndex;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform samplerBuffer bary;
uniform samplerBuffer attributeData;

uniform float meshShrink;

// Point
flat out vec3 pos_world_space;
flat out float radius;


vec4 row0(in mat4 M) {
	return vec4(M[0][0], M[1][0], M[2][0], M[3][0]);
}

vec4 row1(in mat4 M) {
	return vec4(M[0][1], M[1][1], M[2][1], M[3][1]);
}

vec4 row3(in mat4 M) {
	return vec4(M[0][3], M[1][3], M[2][3], M[3][3]);
}

void main()
{
	vec3 bary = texelFetch(bary, cellIndex).xyz;

	// Shrink
	// vec3 pos = aPos - (aPos - bary) * meshShrink;
	vec3 pos = aPos;

	// Transform to clip space
	gl_Position = projection * view * model * vec4(pos, 1.0);
    
	// Brunooo Point

	// TODO: optimize, directly compute r1,r2,r4
	mat4 T = mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		pos.x/size, pos.y/size, pos.z/size, 1.0/size
	);

	mat4 PMT = projection * view * model * T;
	vec4 r1 = row0(PMT);
	vec4 r2 = row1(PMT);
	vec4 r4 = row3(PMT);

	float r1Dr4T = dot(r1.xyz,r4.xyz)-r1.w*r4.w;
	float r1Dr1T = dot(r1.xyz,r1.xyz)-r1.w*r1.w;
	float r4Dr4T = dot(r4.xyz,r4.xyz)-r4.w*r4.w;
	float r2Dr2T = dot(r2.xyz,r2.xyz)-r2.w*r2.w;
	float r2Dr4T = dot(r2.xyz,r4.xyz)-r2.w*r4.w;

	float discriminant_x = r1Dr4T*r1Dr4T-r4Dr4T*r1Dr1T;
	float discriminant_y = r2Dr4T*r2Dr4T-r4Dr4T*r2Dr2T;
	float screen = max(1024., 768.);
	gl_PointSize = sqrt(max(discriminant_x, discriminant_y)) * screen / (-r4Dr4T);

	pos_world_space = pos.xyz;
	radius = size;
}