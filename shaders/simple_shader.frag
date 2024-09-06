#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPositionWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 0) out vec4 outColor;


struct PointLight {
	vec4 position; // ignore w
	vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projection;
	mat4 view;
	mat4 inverseView;
	vec4 ambientLightColor;
	PointLight pointLight[10];
	int numLight;
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;


void main() {
	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 specularLight = vec3(0.0);
	vec3 surfaceNormal = normalize(fragNormalWorld);

	vec3 cameraWorldSpace = ubo.inverseView[3].xyz;
	vec3 viewDirection = normalize(cameraWorldSpace - fragPositionWorld);

	for (int i = 0; i < ubo.numLight; i++) {
		PointLight light = ubo.pointLight[i];
		vec3 directionToLight = light.position.xyz - fragPositionWorld;
		float attenuation = 1.0 / dot(directionToLight, directionToLight);
		directionToLight = normalize(directionToLight);
		float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
		vec3 intensity = light.color.xyz * light.color.w * attenuation;
		diffuseLight += intensity * cosAngIncidence;

		// specular lighting
		vec3 halfAngle = normalize(directionToLight + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0, 1);
		blinnTerm = pow(blinnTerm, 32.0); // higher values -> shaper highlight
		specularLight += intensity * blinnTerm;
	}



    outColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0);
}