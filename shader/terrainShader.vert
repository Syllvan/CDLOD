#version 150

in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 translation;
uniform float scale;
uniform float range;
uniform vec3 cameraPos;
uniform sampler2D myTextureSampler;
uniform vec3 gridDim;

out vec4 fPosition;
out vec3 fWorldPosition;
out float fDist;
out vec3 fNormal;

vec2 morphVertex( vec2 gridPos, vec2 worldPos, float morph) {
	vec2 fracPart = fract(gridPos * gridDim.xy * 0.5) * 2.0 / gridDim.xy;
	return worldPos - fracPart * scale * morph;
}

float getHeight(vec2 v) {
	return 20.0*(texture( myTextureSampler, v/100.0).r - 0.5);
}

void main(void)
{
	fWorldPosition = scale*position + translation;
	float height = getHeight(fWorldPosition.xz);
	float dist = distance(cameraPos, fWorldPosition);
	float rangeDist = 1.0 - smoothstep(0.15, 0.35, (range-dist)/scale*gridDim.x/32.0); //range-dist is positive if within range
	float morphVal = rangeDist;
	fWorldPosition.xz = morphVertex(position.xz, fWorldPosition.xz, morphVal);
	height = getHeight(fWorldPosition.xz);
    
	//height!
	fWorldPosition.y = height; 
	// * 0.5*(sin(fWorldPosition.x) + sin(fWorldPosition.z));

    fPosition = view * vec4(fWorldPosition,1.0);

    fDist = dist;

    float dx = 0.02;
    float x = getHeight(fWorldPosition.xz + vec2(1.0,0.0)) - getHeight(fWorldPosition.xz + vec2(-1.0,0.0));
    float z = getHeight(fWorldPosition.xz + vec2(0.0,1.0)) - getHeight(fWorldPosition.xz + vec2(0.0,-1.0));
    fNormal = normalize(vec3(x/dx,1.0,z/dx));

    gl_Position = projection * fPosition;
}