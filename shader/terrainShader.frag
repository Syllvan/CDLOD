#version 150

in vec4 fPosition;
in vec3 fWorldPosition;
in float fDist;
in vec3 fNormal;

// output
out vec4 color;

// uniform
uniform sampler2D myTextureSampler;

vec3 sundir = normalize(vec3(0.0,1.0,1.0));

float fracNoise(vec2 v) {
	return texture( myTextureSampler, v).r;
}

void main(void)
{   
	float fog = 1.0 - smoothstep(60.0, 80.0, fDist);
	
	float a = dot(fNormal,sundir);
	vec3 c = a*vec3(0.5) + vec3(0.5);
	color = vec4(c,fog)*fog;
}
