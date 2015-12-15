#version 150

in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform vec4 color;
uniform vec3 translation;
uniform float scale;
uniform float range;
uniform vec3 cameraPos;
uniform sampler2D myTextureSampler;

out vec4 fPosition;
out vec3 fWorldPosition;
out vec4 fColor;

vec2 gridDim = vec2(32.0,32.0);
vec2 morphVertex( vec2 gridPos, vec2 worldPos, float morph) {
	vec2 fracPart = fract(gridPos * gridDim * 0.5) * 2.0 / gridDim;
	return worldPos - fracPart * scale * morph;
}

void main(void)
{
	fColor = color;
	fWorldPosition = scale*position + translation;
	float height = texture( myTextureSampler, vec2(fWorldPosition.x, fWorldPosition.z)/100.0).r;
	float detail = texture( myTextureSampler, vec2(fWorldPosition.x, fWorldPosition.z)).r*0.1;
	height = (height - 0.5)*21.0 + detail;
	height *= 1.0f;
	float dist = distance(cameraPos, fWorldPosition);
	float rangeDist = 1.0 - smoothstep(0.4, 1.0, (range-dist)/scale);
	float morphVal = rangeDist;
	fColor = color;
	fWorldPosition.xz = morphVertex(position.xz, fWorldPosition.xz, morphVal);
    
	//height!
	fWorldPosition.y = height; 
	// * 0.5*(sin(fWorldPosition.x) + sin(fWorldPosition.z));

    fPosition = view * vec4(fWorldPosition,1.0);

    gl_Position = projection * fPosition;
}
