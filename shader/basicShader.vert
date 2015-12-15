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
out float fDist;

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
	height = (height - 0.5)*20.0;
	float dist = distance(cameraPos, fWorldPosition);
	float rangeDist = 1.0 - smoothstep(0.0, 1.0, (range-dist)/scale); //range-dist is positive if within range
	float morphVal = rangeDist;
	fColor = color;
	fWorldPosition.xz = morphVertex(position.xz, fWorldPosition.xz, morphVal);
	height = texture( myTextureSampler, vec2(fWorldPosition.x, fWorldPosition.z)/100.0).r;
	height = (height - 0.5)*20.0;
    
	//height!
	fWorldPosition.y = height; 
	// * 0.5*(sin(fWorldPosition.x) + sin(fWorldPosition.z));

    fPosition = view * vec4(fWorldPosition,1.0);

    fDist = dist;

    gl_Position = projection * fPosition;
}