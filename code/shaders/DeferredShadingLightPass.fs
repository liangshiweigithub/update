#version 330 core

out vec4 FragColor;

uniform texture gPosition;
uniform texture gNormal;
uniform texture gAlbedoSpec;

struct Light{
	vec3 Position;
	vec3 Color;
};

uniform vec3 viewPos;

in Light lights[16];
in vec2 TexCoords;
const int NR_LIGHTS = 12;


void main()
{
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 normal = texture(gNormal, TexCoords).rgb;
	vec4 tmp = texture(gAlbedoSpec, TexCoords);
	vec3 albedo = tmp.rgb;
	float spec = tmp.a;
	
	vec3 lighting = albedo * 0.1;
	
	for(int i=0; i < NR_LIGHTS; ++i)
	{
		vec3 lightDir = normalize(lights[i].Position - FragPos);
		vec3 diffuse = max(dot(lightDir, normal), 0) * albedo * lights[i].Color;
		lighting += diffuse;
	}
	FragColor = vec4(lighting, 1.0);
}