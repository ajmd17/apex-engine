#version 330

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord0;
attribute vec3 a_tangent;
attribute vec3 a_bitangent;

varying vec4 v_position;
varying vec4 v_positionCamspace;
varying vec4 v_normal;
varying vec2 v_texcoord0;
varying vec3 v_tangent;
varying vec3 v_bitangent;
varying mat3 v_tbn;

#if SKINNING
#include "include/skinning.inc"
#endif
#include "include/matrices.inc"

void main() {
	v_texcoord0 = a_texcoord0;

    vec3 c1 = cross(a_normal, vec3(0.0, 0.0, 1.0));
    vec3 c2 = cross(a_normal, vec3(0.0, 1.0, 0.0));
    if (length(c1)>length(c2))
      v_tangent = c1;
    else
      v_tangent = c2;

    v_tangent = normalize(v_tangent);
    v_bitangent = cross(a_normal, v_tangent);
    v_bitangent = normalize(v_bitangent);
    
    
    
#if SKINNING
	mat4 skinningMat = createSkinningMatrix();
	
	v_position = u_modelMatrix * skinningMat * vec4(a_position, 1.0);
    v_positionCamspace = u_viewMatrix * u_modelMatrix * skinningMat * vec4(a_position, 1.0);
	v_normal = transpose(inverse(u_modelMatrix * skinningMat)) * vec4(a_normal, 0.0);
#endif
	
#if !SKINNING
  v_position = u_modelMatrix * vec4(a_position, 1.0);
  v_positionCamspace = u_viewMatrix * u_modelMatrix * vec4(a_position, 1.0);
  v_normal = transpose(inverse(u_modelMatrix)) * vec4(a_normal, 0.0);
  //v_tangent = (transpose(inverse(u_modelMatrix)) * vec4(a_tangent, 0.0)).xyz;
  //v_bitangent = (transpose(inverse(u_modelMatrix)) * vec4(a_bitangent, 0.0)).xyz;
#endif


    v_tangent = a_tangent - a_normal * dot( a_tangent, a_normal ); // orthonormalization ot the tangent vectors
    v_bitangent = a_bitangent - a_normal * dot( a_bitangent, a_normal ); // orthonormalization of the binormal vectors to the normal vector 
    v_bitangent = v_bitangent - v_tangent * dot( v_bitangent, v_tangent ); // orthonormalization of the binormal vectors to the tangent vector
    v_tbn = mat3( normalize(v_tangent), normalize(v_bitangent), a_normal );

  //vec3 T = normalize(mat3(u_modelMatrix) * v_tangent);
  //vec3 B = normalize(mat3(u_modelMatrix) * v_bitangent);
  //vec3 N = normalize(mat3(u_modelMatrix) * a_normal);
  
  //vec3 T = (transpose(inverse(u_modelMatrix)) * vec4(a_tangent, 0.0)).xyz;
  //vec3 B = (transpose(inverse(u_modelMatrix)) * vec4(a_bitangent, 0.0)).xyz;
  //vec3 N = v_normal.xyz;
  //v_tangent = a_tangent;
  //v_bitangent = a_bitangent;
  
  
  //v_tbn = mat3(T, B, N);
  
  //v_tangent = a_tangent;
  //v_bitangent = a_tangent;

  gl_Position = u_projMatrix * u_viewMatrix * v_position;
}