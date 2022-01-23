
#version 330 core
precision highp float;
#include "../include/frag_output.inc"

uniform sampler2D ColorMap;
uniform sampler2D NormalMap;
uniform sampler2D PositionMap;
uniform sampler2D DepthMap;
in vec2 v_texcoord0;

uniform vec2 u_resolution;
float strength = 1.0;

#define $CAP_MIN_DISTANCE 0.0001
#define $CAP_MAX_DISTANCE 0.01

// -------------

/*
SSAO GLSL shader v1.2
assembled by Martins U$PItis (martinsh) (devlog-martinsh.blogspot.com)
original technique is made by Arkano22 (www.gamedev.net/to$PIc/550699-ssao-no-halo-artifacts/)
changelog:
1.2 - added fog calculation to mask AO. Minor fixes.
1.1 - added s$PIral sampling method from here:
(http://www.cgafaq.info/wiki/Evenly_distributed_points_on_sphere)
*/
//uniform sampler2D bgl_RenderedTexture;

#define $PI 3.14159265

float width = u_resolution.x; //texture width
float height = u_resolution.y; //texture height

vec2 texCoord = v_texcoord0;

//------------------------------------------
//general stuff

//user variables
int samples = 64; //ao sample count //64.0
float radius = 5.0; //ao radius //5.0

float aoclamp = 0.125; //depth clamp - reduces haloing at screen edges
bool noise = true; //use noise instead of pattern for sample dithering
float noiseamount = 0.0002; //dithering amount

float diffarea = 0.3; //self-shadowing reduction
float gdisplace = 0.4; //gauss bell center //0.4

bool mist = false; //use mist?
float miststart = 0.0; //mist start
float mistend = $CAP_MAX_DISTANCE; //mist end

bool onlyAO = false; //use only ambient occlusion pass?
float lumInfluence = 0.7; //how much luminance affects occlusion

//--------------------------------------------------------

vec2 rand(vec2 coord) //generating noise/pattern texture for dithering
{
  float noiseX = ((fract(1.0-coord.s*(width/2.0))*0.25)+(fract(coord.t*(height/2.0))*0.75))*2.0-1.0;
  float noiseY = ((fract(1.0-coord.s*(width/2.0))*0.75)+(fract(coord.t*(height/2.0))*0.25))*2.0-1.0;

  if (noise)
  {
    noiseX = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233))) * 43758.5453),0.0,1.0)*2.0-1.0;
    noiseY = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233)*2.0)) * 43758.5453),0.0,1.0)*2.0-1.0;
  }
  return vec2(noiseX,noiseY)*noiseamount;
}

float doMist()
{
  float zdepth = texture(DepthMap,texCoord.xy).x;
  float depth = -$CAP_MAX_DISTANCE * $CAP_MIN_DISTANCE / (zdepth * ($CAP_MAX_DISTANCE - $CAP_MIN_DISTANCE) - $CAP_MAX_DISTANCE);
  return clamp((depth-miststart)/mistend,0.0,1.0);
}

float readDepth(vec2 coord)
{
  if (v_texcoord0.x<0.0||v_texcoord0.y<0.0) return 1.0;
  else {
    float z_b = texture(DepthMap, coord ).x;
    float z_n = 2.0 * z_b - 1.0;
    return (2.0 * $CAP_MIN_DISTANCE) / ($CAP_MAX_DISTANCE + $CAP_MIN_DISTANCE - z_n * ($CAP_MAX_DISTANCE-$CAP_MIN_DISTANCE));
  }
}

int compareDepthsFar(float depth1, float depth2) {
  float garea = 2.0; //gauss bell width
  float diff = (depth1 - depth2)*100.0; //depth difference (0-100)
  //reduce left bell width to avoid self-shadowing
  if (diff<gdisplace)
  {
    return 0;
  } else {
    return 1;
  }
}

float compareDepths(float depth1, float depth2)
{
  float garea = 2.0; //gauss bell width
  float diff = (depth1 - depth2)*100.0; //depth difference (0-100)
  //reduce left bell width to avoid self-shadowing
  if (diff<gdisplace)
  {
    garea = diffarea;
  }

  float gauss = pow(2.7182,-2.0*(diff-gdisplace)*(diff-gdisplace)/(garea*garea));
  return gauss;
}

float calAO(float depth,float dw, float dh)
{
  float dd = (1.0-depth)*radius;

  float temp = 0.0;
  float temp2 = 0.0;
  float coordw = v_texcoord0.x + dw*dd;
  float coordh = v_texcoord0.y + dh*dd;
  float coordw2 = v_texcoord0.x - dw*dd;
  float coordh2 = v_texcoord0.y - dh*dd;

  vec2 coord = vec2(coordw , coordh);
  vec2 coord2 = vec2(coordw2, coordh2);

  float cd = readDepth(coord);
  int far = compareDepthsFar(depth, cd);
  temp = compareDepths(depth, cd);
  //DEPTH EXTRAPOLATION:
  if (far > 0)
  {
    temp2 = compareDepths(readDepth(coord2),depth);
    temp += (1.0-temp)*temp2;
  }

  return temp;
}

void main(void)
{
  vec2 noise = rand(texCoord);
  float depth = readDepth(texCoord);

  float w = (1.0 / width)/clamp(depth,aoclamp,1.0)+(noise.x*(1.0-noise.x));
  float h = (1.0 / height)/clamp(depth,aoclamp,1.0)+(noise.y*(1.0-noise.y));

  float pw = 0.0;
  float ph = 0.0;

  float ao = 0.0;

  float dl = $PI * (3.0 - sqrt(5.0));
  float dz = 1.0 / float(samples);
  float l = 0.0;
  float z = 1.0 - dz/2.0;

  for (int i = 0; i < 64; i++)
  {
    if (i > samples) break;
    float r = sqrt(1.0 - z);

    pw = cos(l) * r;
    ph = sin(l) * r;
    ao += calAO(depth,pw*w,ph*h);
    z = z - dz;
    l = l + dl;
  }


  ao /= float(samples);
  ao *= strength;
  ao = 1.0-ao;

  if (mist)
  {
    ao = mix(ao, 1.0, doMist());
  }

	vec4 colorTexture = texture(ColorMap, v_texcoord0);
	//colorTexture.rgb *= vec3(ao);

  output0 = colorTexture;
  output4 = vec4(0.0, 0.0, 0.0, ao);
}
