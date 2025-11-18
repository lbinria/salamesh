#version 440 core

// Color output
out vec4 FragColor;

uniform bool is_light_enabled;

uniform mat4 inverse_projection_view_model;
uniform mat4 model_view_projection_matrix;

// Point
flat in vec3 pos_world_space;
flat in float radius;
flat in vec2 fragViewport;

struct Ray {
    vec3 O; // Origin
    vec3 V; // Direction vector
};

// Notes: GLUP.viewport = [x0,y0,width,height]
// clip-space coordinates are in [-1,1] (not [0,1]) !

// Computes the ray that passes through the current fragment
// The ray is in world space.
Ray glup_primary_ray() {
	vec4 near = vec4(
		2.0 * ( (gl_FragCoord.x - 0.) / fragViewport.x - 0.5),
		2.0 * ( (gl_FragCoord.y - 0.) / fragViewport.y - 0.5),
		0.0, 1.0
	);
	near = inverse_projection_view_model * near;
	vec4 far = near + inverse_projection_view_model[2];
	near.xyz /= near.w;
	far.xyz /= far.w;
	return Ray(near.xyz, far.xyz-near.xyz);
}

void glup_update_depth(in vec3 M_world_space) {
    vec4 M_clip_space = model_view_projection_matrix * vec4(M_world_space,1.0);
    float z = 0.5*(1.0 + M_clip_space.z/M_clip_space.w);
    gl_FragDepth = (1.0-z)*gl_DepthRange.near + z*gl_DepthRange.far;
}

void main()
{
	vec3 col = vec3(0.f, 0.f, 0.f);

	Ray R = glup_primary_ray();
	vec3 M,N;

	vec3 D = R.O-pos_world_space;    
	float a = dot(R.V,R.V);
	float b = 2.0*dot(R.V,D);
	float c = dot(D,D)-radius*radius;
	float delta = b*b-4.0*a*c;

	if (delta < 0.0) {
		discard;
	}

	float t = (-b-sqrt(delta))/(2.0*a);
	M = R.O + t*R.V;
	N = M-pos_world_space;

	// glup_update_depth(M);

	col = vec3(1.f, 0.8f, 0.35f) /** (1. - dot(N, vec3(-0.5f, -0.8f, 0.2f)))*/;
	// col = vec3(1.f, 1.f, 1.f) /** (1. - dot(N, vec3(-0.5f, -0.8f, 0.2f)))*/;

	FragColor = vec4(col, 1.f);
}