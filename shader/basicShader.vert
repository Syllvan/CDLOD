#version 150

in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform vec4 color;

out vec4 fPosition;
out vec4 fColor;

void main(void)
{
	fColor = color;
    fPosition = view * vec4(position,1.0);

    gl_Position = projection * fPosition;
}
