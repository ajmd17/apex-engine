#version 420

uniform mat4 u_modelMatrix;
uniform mat4 u_projMatrix;
uniform mat4 u_viewMatrix;


layout(triangles) in;
layout(triangle_strip, max_vertices = 24) out;
// input
in VSOutput
{
	vec3 normal;
	vec3 texcoord0;
} vs_out[];

// output
out GSOutput
{
	vec3 normal;
	vec3 texcoord0;
	vec3 offset;
} gs_out;


float cubeScale = 0.2;

void main()
{

	vec4 centerPos = gl_in[0].gl_Position;
	mat4 mvp = u_projMatrix * u_viewMatrix * u_modelMatrix;

	// -X
	{
		gl_Position = mvp * (centerPos + cubeScale * vec4(-.5, .5, .5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(-.5, .5, .5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();

		gl_Position = mvp * (centerPos + cubeScale * vec4(-.5, .5, -.5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(-.5, .5, -.5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();

		gl_Position = mvp * (centerPos + cubeScale * vec4(-.5, -.5, .5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(-.5, -.5, .5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();
		
		gl_Position = mvp * (centerPos + cubeScale * vec4(-.5, -.5, -.5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(-.5, -.5, -.5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();
	}
	EndPrimitive();
	
	// +X
	{
		gl_Position = mvp * (centerPos + cubeScale * vec4(.5, .5, -.5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(.5, .5, -.5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();

		gl_Position = mvp * (centerPos + cubeScale * vec4(.5, .5, .5, 0.0));			
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(.5, .5, .5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();

		gl_Position = mvp * (centerPos + cubeScale * vec4(.5, -.5, -.5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(.5, -.5, -.5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();
		
		gl_Position = mvp * (centerPos + cubeScale * vec4(.5, -.5, .5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(.5, -.5, .5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();
	}
	EndPrimitive();
	// -Y
	{
		gl_Position = mvp * (centerPos + cubeScale * vec4(-.5, -.5, -.5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(-.5, -.5, -.5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();

		gl_Position = mvp * (centerPos + cubeScale * vec4(.5, -.5, -.5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(.5, -.5, -.5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();

		gl_Position = mvp * (centerPos + cubeScale * vec4(-.5, -.5, .5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(-.5, -.5, .5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();
		
		gl_Position = mvp * (centerPos + cubeScale * vec4(.5, -.5, .5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(.5, -.5, .5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();
	}
	EndPrimitive();
	// +Y
	{
		gl_Position = mvp * (centerPos + cubeScale * vec4(.5, .5, -.5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(.5, .5, -.5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();

		gl_Position = mvp * (centerPos + cubeScale * vec4(-.5, .5, -.5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(-.5, .5, -.5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();

		gl_Position = mvp * (centerPos + cubeScale * vec4(.5, .5, .5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(.5, .5, .5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();
		
		gl_Position = mvp * (centerPos + cubeScale * vec4(-.5, .5, .5, 0.0));			
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(-.5, .5, .5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();
	}
	EndPrimitive();
	// -Z
	{
		gl_Position = mvp * (centerPos + cubeScale * vec4(-.5, -.5, -.5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(-.5, -.5, -.5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();

		gl_Position = mvp * (centerPos + cubeScale * vec4(-.5, .5, -.5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(-.5, .5, -.5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();

		gl_Position = mvp * (centerPos + cubeScale * vec4(.5, -.5, -.5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(.5, -.5, -.5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();
		
		gl_Position = mvp * (centerPos + cubeScale * vec4(.5, .5, -.5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(.5, .5, -.5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();
	}
	EndPrimitive();
	// +Z
	{
		gl_Position = mvp * (centerPos + cubeScale * vec4(.5, -.5, .5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(.5, -.5, .5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();

		gl_Position = mvp * (centerPos + cubeScale * vec4(.5, .5, .5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(.5, .5, .5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();

		gl_Position = mvp * (centerPos + cubeScale * vec4(-.5, -.5, .5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(-.5, -.5, .5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();
		
		gl_Position = mvp * (centerPos + cubeScale * vec4(-.5, .5, .5, 0.0));
		gs_out.texcoord0 = vs_out[0].texcoord0;
		gs_out.offset = vec3(-.5, .5, .5);
		gs_out.normal = vs_out[0].normal;
		EmitVertex();
	}
	EndPrimitive();

}
