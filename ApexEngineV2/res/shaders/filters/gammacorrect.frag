#version 330 core

varying vec2 v_texcoord0;
uniform sampler2D u_texture;

const float gamma = 1.5;

void main()
{
  vec4 tex = texture2D(u_texture, v_texcoord0);
  
  gl_FragColor.rgb = pow(tex.rgb, vec3(1.0/gamma));
  gl_FragColor.a = 1.0;
}