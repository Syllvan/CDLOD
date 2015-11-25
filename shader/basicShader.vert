#version 150

in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform vec4 color;
uniform vec3 translation;
uniform float scale;

out vec4 fPosition;
out vec4 fColor;

void main(void)
{
	fColor = color;
    fPosition = view * vec4(scale*position+translation,1.0);

    gl_Position = projection * fPosition;
}
