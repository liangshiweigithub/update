#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform bool hdr;
uniform float exposure;
uniform sampler2D hdrTexture;

void normalHDR()
{
	float gamma = 2.2;
	
	vec3 color = texture(hdrTexture, TexCoords).rgb;
	color = pow(color, vec3(1.0/gamma));
	FragColor = vec4(color, 1.0);
}

void ReinhardToneMapping()
{
	float gamma = 2.2;
	vec3 color = texture(hdrTexture, TexCoords).rgb;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/gamma));
	FragColor = vec4(color, 1.0);
}

void ExposureHDR()
{
	const float gamma = 2.2;
	vec3 color = texture(hdrTexture, TexCoords).rgb;
	color = vec3(1.0) - exp(-color * exposure);
	color = pow(color, vec3(1.0/gamma));
	FragColor = vec4(color, 1.0);
}

void main()
{
	if(hdr)
		ExposureHDR();
	else
		normalHDR();
}