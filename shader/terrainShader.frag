#version 150

in vec4 fPosition;
in vec3 fWorldPosition;
in float fDist;
in vec3 fNormal;

// output
out vec4 color;

// uniform
uniform sampler2D myTextureSampler;
uniform float theTime;

vec3 sundir = normalize(vec3(0.0,1.0,1.0));

float fracNoise(vec2 v) {
	return texture( myTextureSampler, v).r;
}

float random(float p) {
  return fract(sin(p)*10000.);
}

float noise(vec2 p) {
  return random(p.x + p.y*10000.);
}

vec2 sw(vec2 p) {return vec2( floor(p.x) , floor(p.y) );}
vec2 se(vec2 p) {return vec2( ceil(p.x)  , floor(p.y) );}
vec2 nw(vec2 p) {return vec2( floor(p.x) , ceil(p.y)  );}
vec2 ne(vec2 p) {return vec2( ceil(p.x)  , ceil(p.y)  );}

float smoothNoise(vec2 p) {
  vec2 inter = smoothstep(0., 1., fract(p));
  float s = mix(noise(sw(p)), noise(se(p)), inter.x);
  float n = mix(noise(nw(p)), noise(ne(p)), inter.x);
  return mix(s, n, inter.y);
  return noise(nw(p));
}

float movingNoise(vec2 p) {
  float total = 0.0;
  total += smoothNoise(p     - theTime);
  total += smoothNoise(p*2.  + theTime) / 2.;
  total += smoothNoise(p*4.  - theTime) / 4.;
  total += smoothNoise(p*8.  + theTime) / 8.;
  total += smoothNoise(p*16. - theTime) / 16.;
  total /= 1. + 1./2. + 1./4. + 1./8. + 1./16.;
  return total;
}

float nestedNoise(vec2 p) {
  float x = movingNoise(p);
  float y = movingNoise(p + 100.);
  return movingNoise(p + vec2(x, y));
}

//void main(void)
//{   
//	float fog = 1.0 - smoothstep(60.0, 80.0, fDist);
//	
//	float a = dot(fNormal,sundir);
//	vec3 c = a*vec3(0.5) + vec3(0.5);
//	color = vec4(c,fog)*fog;
//}

void main() {
  vec2 p = fWorldPosition.xz * 6.;
  float brightness = nestedNoise(p);
  color.rgb = vec3(brightness);
  color.a = 1.;
}