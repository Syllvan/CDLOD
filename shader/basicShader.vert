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
uniform vec3 gridDim;

out vec4 fPosition;
out vec3 fWorldPosition;
out vec4 fColor;
out float fDist;

vec2 morphVertex( vec2 gridPos, vec2 worldPos, float morph) {
	vec2 fracPart = fract(gridPos * gridDim.xy * 0.5) * 2.0 / gridDim.xy;
	return worldPos - fracPart * scale * morph;
}

float getHeight(vec2 v) {
	return 20.0*(texture( myTextureSampler, v/100.0).r - 0.5);
}

void main(void)
{
	float morphStart = 0.0;
	float morphEnd = 0.25;
	float morphAvg = (morphStart+morphEnd)/2.0;
	float morphDiff = morphEnd - morphAvg;

	fWorldPosition = scale*position + translation;
	float height = getHeight(fWorldPosition.xz);
	fWorldPosition.y = height;
	float dist = distance(cameraPos, fWorldPosition);
	float nextlevel_thresh = ((range-dist)/scale*gridDim.x/32.0); //positive if within range
	float rangeDist = 1.0 - smoothstep(morphStart, morphEnd, nextlevel_thresh);
	float morphVal = rangeDist;
	fColor = color*(0.7*step(morphDiff,abs(morphAvg - nextlevel_thresh)) + 0.3);//*step(0.01,abs(nextlevel_thresh-0.35));
	fWorldPosition.xz = morphVertex(position.xz, fWorldPosition.xz, morphVal);
	height = getHeight(fWorldPosition.xz);

	//height!
	fWorldPosition.y = height; 
	// * 0.5*(sin(fWorldPosition.x) + sin(fWorldPosition.z));

    fPosition = view * vec4(fWorldPosition,1.0);

    fDist = dist;

    gl_Position = projection * fPosition;
}