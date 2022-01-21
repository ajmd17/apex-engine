#version 420

#include "../include/attributes.inc"

#define $SCALE 5.0

uniform mat4 u_modelMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_viewMatrix;

out vec4 v_position;
out vec4 v_normal;
out vec2 v_texcoord0;
out vec4 ndcPos;

out VSOutput
{
	vec3 normal;
	vec3 texcoord0;
} vs_out;


void main() 
{
    v_position = u_modelMatrix * vec4(a_position, 1.0);
    v_normal = transpose(inverse(u_modelMatrix)) * vec4(a_normal, 0.0);
    ndcPos = (u_modelMatrix*vec4(a_position, 1.0)) * $SCALE;
	

	vs_out.texcoord0 = vec3(a_texcoord0, 0.0);
	vs_out.normal = vec3(a_normal.xyz);

	gl_Position = u_projMatrix * u_viewMatrix * v_position;
}
