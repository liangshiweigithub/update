#version 330 core

out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
	vec3 N = normalize(WorldPos);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, N));
	up = normalize(cross(N, right));
	
	vec3 irradiance = vec3(0.0);
	
	float sampleDelta = 0.025;
	float nSamples = 0.0;
	
	for(float phi = 0; phi < 2 * PI; phi += sampleDelta)
	{
		for(float theta = 0; theta < PI * 0.5; theta += sampleDelta)
		{
			vec3 tangentSample = vec3(cos(phi) * sin(theta), cos(theta), sin(phi) * sin(theta));
			vec3 sampleVec = tangentSample. x * right + tangentSample.y * up + tangentSample.z * N;
			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			++nSamples;
		}
	}
	irradiance = PI * irradiance * (1.0 / nSamples);
	FragColor = vec4(irradiance, 1.0);
}