#version core 330

out vec4 FragColor;

in VS_OUT{
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
}fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float height_scale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
	float height = texture(depthMap, texCoords).r;
	vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
	return texCoords - p;
}