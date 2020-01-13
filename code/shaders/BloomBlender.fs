#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform bool bloom;
uniform float exposure;

void main()
{
	vec3 hdrColor = texture(scene, TexCoords).rbg;
	vec3 bloomColor = texture(bloomBlur, TexCoords).rbg;
	if(bloom)
		hdrColor += bloomColor;
	
	const float gamma = 2.2;
	hdrColor = vec3(1.0) - exp(-hdrColor * exposure);
	hdrColor = pow(hdrColor, vec3(1.0/2.2));
	
	FragColor = vec4(hdrColor, 1.0);
}