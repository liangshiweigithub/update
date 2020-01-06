#version 330 core

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
}fs_in;

uniform sampler2D floorTexture;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 viewPos;
uniform bool gamma;

out vec4 FragColor;

vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
	// diffuse
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(lightDir, normal), 0);
	vec3 diffuse = lightColor * diff;
	
	//specular
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfwayDir = normalize(viewDir + lightDir);
	float spec = pow(max(dot(halfwayDir, normal), 0), 64);
	vec3 specular = spec * lightColor;
	
	float max_distance = 1.5;
	float distance = length(lightPos - fragPos);
	float attenuation = 1 / (gamma ? distance * distance : distance);
	
	return (diffuse + specular) * attenuation;
}

void main()
{
	vec3 color = texture(floorTexture, fs_in.TexCoords).rgb;
	vec3 lighting = vec3(0.0);
	for(int i=0;i<4;++i)
	{
		lighting += BlinnPhong(fs_in.Normal, fs_in.FragPos, lightPositions[i], lightColors[i]);
	}
	color *= lighting;
	if(gamma)
		color = pow(color, vec3(1.0/2.2));
	FragColor = vec4(color, 1.0);
}