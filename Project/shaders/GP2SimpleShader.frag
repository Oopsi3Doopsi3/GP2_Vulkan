#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
layout (location = 3) in vec2 fragUV;
layout (location = 4) in mat3 fragTBN;

layout (location = 0) out vec4 outColor;

struct PointLight
{
	vec4 position;
	vec4 color;
};

layout (set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projection;
	mat4 view;
	mat4 invView;
	vec4 ambientLightColor;
	vec4 lightDirection;
	PointLight pointLights[10];
	int numLights;
} ubo;

layout (set = 0, binding = 1) uniform sampler2D diffuseMap;
layout (set = 0, binding = 2) uniform sampler2D normalMap;
layout (set = 0, binding = 3) uniform sampler2D glossinessMap;
layout (set = 0, binding = 4) uniform sampler2D specularMap;

layout(push_constant) uniform Push
{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

float PI = 3.14159265358979323846f;

float observedArea(vec3 L, vec3 N)
{
	return max(0.f, dot(N, L));
}

float specularValue(vec3 L, vec3 N, vec3 V, float specColor, float phong)
{
	vec3 reflection = L - (2.f * dot(L, N) * N);
	float cosAlpha = max(0.f, dot(reflection, V));
	return specColor * pow(cosAlpha, phong);
}

void main() 
{
	float totalObservedArea = 0.0;
	vec3 totalSpecularValue = vec3(0.0);

	vec3 cameraPosWorld = ubo.invView[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

	//Diffuse color
	float diffuseReflectance = 7.f;
	vec3 diffuseColor = texture(diffuseMap, fragUV).rgb;
	vec3 lambertDiffuse = diffuseColor * diffuseReflectance / PI;

	//Normal
	vec3 normalMapSample = texture(normalMap, fragUV).rgb;
	normalMapSample = normalize(normalMapSample * 2.0 - 1.0);
	vec3 surfaceNormal = normalize(fragTBN * normalMapSample);

	//Specularity values
    float specularColor = texture(specularMap, fragUV).r;
    float phongExponent = texture(glossinessMap, fragUV).r;
	float shininess = 25.f;
	phongExponent *= shininess;

	//Observed area
	vec3 lightDir = normalize(-ubo.lightDirection.xyz);
	totalObservedArea += observedArea(lightDir, surfaceNormal);
	
	//Specularity
	totalSpecularValue += specularValue(lightDir, surfaceNormal, viewDirection, specularColor, phongExponent);

	//Point lights
	for(int i = 0; i < ubo.numLights; ++i)
	{
		PointLight light = ubo.pointLights[i];
		vec3 directionToLight = light.position.xyz - fragPosWorld;
		float attenuation = 1.0 / dot(directionToLight, directionToLight);
		directionToLight = normalize(directionToLight);
		
		//Observed area
		float cosAngIncidence = observedArea(directionToLight, surfaceNormal);
		totalObservedArea += cosAngIncidence * attenuation;
		vec3 intensity = light.color.xyz * light.color.w * attenuation;
		lambertDiffuse += intensity * cosAngIncidence;
	
		//Specularity
		totalSpecularValue += specularValue(directionToLight, surfaceNormal, viewDirection, specularColor, phongExponent) * intensity;
	}
	
	//observedArea
	//outColor = vec4(vec3(1.0) * totalObservedArea, 1.0);
	//diffuseColor
	//outColor = vec4(diffuseColor * totalObservedArea, 1.0);
	//specular
	//outColor = vec4(vec3(1.0) * totalSpecularValue * totalObservedArea, 1.0);
	//combined
	outColor = vec4((vec3(1.0) * totalSpecularValue + lambertDiffuse) * totalObservedArea + ubo.ambientLightColor.xyz * ubo.ambientLightColor.w, 1.0);
}