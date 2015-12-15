#version 150

in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform vec4 color;
uniform vec3 translation;
uniform float scale;
uniform float mnmx;
uniform vec3 cameraPos;

out vec4 fPosition;
out vec3 fWorldPosition;
out vec4 fColor;

void main(void)
{
	fColor = color;
	fWorldPosition = scale*position + translation;

	//height!
	fWorldPosition.y = mnmx; 

    fPosition = view * vec4(fWorldPosition,1.0);

    gl_Position = projection * fPosition;
}