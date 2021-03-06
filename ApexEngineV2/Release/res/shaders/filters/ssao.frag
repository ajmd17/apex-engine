
#version 150
precision highp float;

uniform sampler2D u_colorMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_positionMap;
uniform sampler2D u_depthMap;

uniform vec3 u_kernel[$KERNEL_SIZE];

uniform sampler2D u_noiseMap;

uniform vec2 u_noiseScale;
uniform mat4 u_view;
uniform mat4 u_projectionMatrix;

uniform float u_radius;

varying vec2 v_texcoord0;

uniform mat4 u_inverseProjectionMatrix;

vec4 getViewPos(vec2 texCoord)
{
	// Calculate out of the fragment in screen space the view space position.

	float x = texCoord.s * 2.0 - 1.0;
	float y = texCoord.t * 2.0 - 1.0;

	// Assume we have a normal depth range between 0.0 and 1.0
	float z = texture(u_depthMap, texCoord).r * 2.0 - 1.0;

	vec4 posProj = vec4(x, y, z, 1.0);

	vec4 posView = u_inverseProjectionMatrix * posProj;

	posView /= posView.w;

	return posView;
}

void main(void)
{
	// Calculate out of the current fragment in screen space the view space position.

	vec4 posView = getViewPos(v_texcoord0);

	// Normal gathering.

	vec3 normalView = normalize(texture(u_normalMap, v_texcoord0).xyz * 2.0 - 1.0);

	// Calculate the rotation matrix for the kernel.

	vec3 randomVector = normalize(texture(u_noiseMap, v_texcoord0 * u_noiseScale).xyz * 2.0 - 1.0);

	// Using Gram-Schmidt process to get an orthogonal vector to the normal vector.
	// The resulting tangent is on the same plane as the random and normal vector.
	// see http://en.wikipedia.org/wiki/Gram%E2%80%93Schmidt_process
	// Note: No division by <u,u> needed, as this is for normal vectors 1.
	vec3 tangentView = normalize(randomVector - dot(randomVector, normalView) * normalView);

	vec3 bitangentView = cross(normalView, tangentView);

	// Final matrix to reorient the kernel depending on the normal and the random vector.
	mat3 kernelMatrix = mat3(tangentView, bitangentView, normalView);

	// Go through the kernel samples and create occlusion factor.
	float occlusion = 0.0;

	for (int i = 0; i < int($KERNEL_SIZE); i++)
	{
		// Reorient sample vector in view space ...
		vec3 sampleVectorView = kernelMatrix * u_kernel[i];

		// ... and calculate sample point.
		vec4 samplePointView = posView + u_radius * vec4(sampleVectorView, 0.0);

		// Project point and calculate NDC.

		vec4 samplePointNDC = u_projectionMatrix * samplePointView;

		samplePointNDC /= samplePointNDC.w;

		// Create texture coordinate out of it.

		vec2 samplePointTexCoord = samplePointNDC.xy * 0.5 + 0.5;

		// Get sample out of depth texture

		float zSceneNDC = texture(u_depthMap, samplePointTexCoord).r * 2.0 - 1.0;

		float delta = samplePointNDC.z - zSceneNDC;

		// If scene fragment is before (smaller in z) sample point, increase occlusion.
		if (delta > $CAP_MIN_DISTANCE && delta < $CAP_MAX_DISTANCE)
		{
			occlusion += 1.0;
		}
	}

    vec3 noiseRgb = texture(u_noiseMap, v_texcoord0 * vec2(40.0)).rgb;
	// No occlusion gets white, full occlusion gets black.
	occlusion = min(max(1.0 - ((occlusion / float($KERNEL_SIZE))), 0.0), 1.0);

	gl_FragColor = vec4(texture(u_colorMap, v_texcoord0).rgb * vec3(occlusion), 1.0);
}
