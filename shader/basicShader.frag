#version 150

in vec4 fPosition;
in vec3 fWorldPosition;
in vec4 fColor;
in float fDist;
// output
out vec4 color;

// uniform
uniform sampler2D myTextureSampler;

void main(void)
{   
	float fog = 1.0 - smoothstep(80.0, 100.0, fDist);
	color = fColor*fog;
	//color = vec4(1,1,1,1);
}
