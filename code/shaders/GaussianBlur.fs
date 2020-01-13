#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform bool horizontal;
uniform sampler2D image;


void main()
{
	vec3 result = texture(image, TexCoords).rgb;
	vec2 texOffset = 1.0 / textureSize(image, 0);
	
	float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
	result *= weights[0];
	
	if(horizontal)
	{
		for(int i=1;i<5;++i)
		{
			result += texture(image, TexCoords + vec2(texOffset.x * i, 0)).rgb * weights[i];
			result += texture(image, TexCoords - vec2(texOffset.x * i, 0)).rgb * weights[i];
		}
	}
	else
	{
		for(int i=1;i<5;++i)
		{
			result += texture(image, TexCoords + vec2(0, texOffset.y * i)).rgb * weights[i];
			result += texture(image, TexCoords - vec2(0, texOffset.y * i)).rgb * weights[i];
		}
	}
	FragColor = vec4(result, 1.0);
}
	