#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture1;

void NormalColor()
{
	FragColor = texture(texture1, TexCoords);
}

void InverseColor()
{
	FragColor = vec4(vec3(1 - texture(texture1, TexCoords)), 1.0f); 
}

void GrayScaleV1()
{
	FragColor = texture(texture1, TexCoords);
	float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
	FragColor = vec4(average, average, average, 1.0f);
}

void GrayScaleV2()
{
    FragColor = texture(texture1, TexCoords);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
}

void KernaleEffect()
{
	const float offset = 1.0 / 300.0;
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),
		vec2(0, offset),
		vec2(offset, offset),
		vec2(-offset, 0),
		vec2(0, 0),
		vec2(offset, 0),
		vec2(-offset, -offset),
		vec2(0, -offset),
		vec2(offset, -offset)
	);
	
	float kernal[9] = float[](
		-1, -1, -1,
		-1, 9, -1,
		-1, -1, -1
	);
	vec3 sampleTex[9];
	for(int i=0;i<9;++i)
		sampleTex[i] = vec3(texture(texture1, TexCoords.st + offsets[i]));
	vec3 col = vec3(0.0);
	for(int i=0;i<9;++i)
		col += sampleTex[i] * kernal[i];
	FragColor = vec4(col, 1.0f);
}

void Blur()
{
	const float offset = 1.0 / 300.0;
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),
		vec2(0, offset),
		vec2(offset, offset),
		vec2(-offset, 0),
		vec2(0, 0),
		vec2(offset, 0),
		vec2(-offset, -offset),
		vec2(0, -offset),
		vec2(offset, -offset)
	);
	
	float kernal[9] = float[](
		1.0/16, 1.0/16, 1.0/16,
		2.0/16, 4.0/16, 2.0/16,
		1.0/16, 1.0/16, 1.0/16
	);
	vec3 sampleTex[9];
	for(int i=0;i<9;++i)
		sampleTex[i] = vec3(texture(texture1, TexCoords.st + offsets[i]));
	vec3 col = vec3(0.0);
	for(int i=0;i<9;++i)
		col += sampleTex[i] * kernal[i];
	FragColor = vec4(col, 1.0f);
}


void EdgeDetection()
{
	const float offset = 1.0 / 300.0;
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),
		vec2(0, offset),
		vec2(offset, offset),
		vec2(-offset, 0),
		vec2(0, 0),
		vec2(offset, 0),
		vec2(-offset, -offset),
		vec2(0, -offset),
		vec2(offset, -offset)
	);
	
	float kernal[9] = float[](
		1, 1, 1,
		1, -8, 1,
		1, 1, 1
	);
	vec3 sampleTex[9];
	for(int i=0;i<9;++i)
		sampleTex[i] = vec3(texture(texture1, TexCoords.st + offsets[i]));
	vec3 col = vec3(0.0);
	for(int i=0;i<9;++i)
		col += sampleTex[i] * kernal[i];
	FragColor = vec4(col, 1.0f);
}


void main()
{
    //EdgeDetection();
	NormalColor();
}