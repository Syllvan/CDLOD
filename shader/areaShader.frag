#version 150

in vec4 fPosition;
in vec3 fWorldPosition;
in vec4 fColor;
// output
out vec4 color;

// uniform
uniform sampler2D myTextureSampler;

void main(void)
{   
	color = vec4(0.5);
}
