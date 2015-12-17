#version 150

in vec4 fPosition;
in vec3 fWorldPosition;
in vec2 screenPos;
in float fDist;

// output
out vec4 color;

// uniform
uniform sampler2D myTextureSampler;
uniform float theTime;
uniform int option;
uniform vec3 cameraPos;

vec3 sundir = normalize(vec3(0.0,1.0,0.5));

float getHeight(vec2 v) {
  return 20.0*(texture( myTextureSampler, v/100.0).r - 0.5);
}

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

float fractalNoise(vec2 p) {
  return 0.0625*texture( myTextureSampler, 8.03*p).r +
         0.125*texture( myTextureSampler, 4.02*p).r +
         0.25*texture( myTextureSampler, 2.01*p).r +
         0.5*texture( myTextureSampler, p).r;
}

float getHeightD(vec2 p) {
  return getHeight(p);// + fractalNoise(2.0*p);
}

void main(void)
{ 
	float fog = min(fDist,70.0)/70.0;

  float dx = 0.1;
  float nx = (getHeightD(fWorldPosition.xz + vec2(dx,0.0)) - getHeightD(fWorldPosition.xz + vec2(-dx,0.0)))/(2.0*dx);
  float ny = 1.0;
  float nz = (getHeightD(fWorldPosition.xz + vec2(0.0,dx)) - getHeightD(fWorldPosition.xz + vec2(0.0,-dx)))/(2.0*dx);
  vec3 normal = vec3(nx,ny,nz);

  vec3 eyedir = normalize(cameraPos - fWorldPosition); 
  vec3 refdir = normalize(-reflect(sundir,normal));

  float ambient = 0.1;
  float diffuse = max(dot(normal,sundir),0.0);
  float specular = max(dot(refdir,eyedir),0.0);

  if (option == 1) {
    float c = ambient + 0.5*diffuse + 0.3*pow(specular,0.5);
    color = mix(vec4(vec3(0.9*c),1.0),vec4(1.0),fog);
  } else if (option == 2) {
    vec3 skyColor = vec3(0.5,0.8,0.95);
    float snowThres = (fWorldPosition.y/20.0 + 0.5) - fractalNoise(0.1*fWorldPosition.xz);
    float snowMorph = 2.0*smoothstep(0.0,0.1,snowThres);
    vec3 snowColor = vec3(0.9);
    vec3 stoneColor = vec3(0.5,0.5,0.5) - 0.5*fractalNoise(0.01*fWorldPosition.xz);
    vec3 diffuseColor = mix(stoneColor,snowColor,snowMorph);
    vec3 c = diffuseColor*diffuse;// + 0.3*pow(specular,0.5);
    color = mix(vec4(c,1.0),vec4(vec3(skyColor),1.0),fog);
  } else if (option == 3) {
    float fresnel = max(dot(normal,eyedir),0.0);
    vec3 c = vec3(0.0,0.0,0.0)*ambient + vec3(0.0,0.1,0.2)*diffuse + vec3(0.5)*pow(fresnel,3.0) + vec3(0.5)*pow(specular,3.0);
    color = mix(vec4(c,1.0),vec4(0.0),fog);
  } else if (option == 4) {
    color = vec4(vec3(fWorldPosition.y/20.0 + 0.5),1.0)*(1.0 - fog);
  } else {
    vec2 p = fWorldPosition.xz * 6.0;
    color = vec4(vec3(nestedNoise(p)),1.0)*(1.0 - fog);
  }
}
/*
void main() {
  vec2 p = fWorldPosition.xz * 6.;
  float brightness = nestedNoise(p);
  color.rgb = vec3(brightness);
  color.a = 1.;
}
*/