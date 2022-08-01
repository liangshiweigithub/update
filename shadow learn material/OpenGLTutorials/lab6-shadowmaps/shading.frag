#version 130
// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;
uniform sampler2D tex0;
uniform sampler2DShadow tex1;
uniform vec3 lightPosition;

in vec4 outColor;
in vec2 texCoord;
in vec3 normal; 
in vec3 eyeVector; 
in vec3 modelViewPosition; 
in vec4 shadowMapCoord; 

out vec4 fragmentColor;


void main() 
{
	vec3 posToLight = normalize(lightPosition - modelViewPosition);
	fragmentColor = max(0, dot(posToLight, normalize(normal))) * texture(tex0, texCoord.xy);
	fragmentColor *= textureProj(tex1, shadowMapCoord); 	
	fragmentColor.a = 1.0;
}