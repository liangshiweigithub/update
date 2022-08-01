#version 130
in vec3		vertex;
in vec3		color;
out vec4	outColor;
in	vec2	texCoordIn;	// incoming texcoord from the texcoord array
out	vec2	texCoord;	// outgoing interpolated texcoord to fragshader
in  vec3	normalIn;		// The normalIn attribute
out vec3	normal;			// Sent to fragment shader
out vec3	modelViewPosition;		// Sent to fragment shader
uniform mat4 normalMatrix;	 
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix; 
uniform mat4 lightMatrix; 
out vec4 shadowMapCoord;


void main() 
{
	gl_Position = modelViewProjectionMatrix * vec4(vertex,1);
	outColor = vec4(color,1); 
	texCoord = texCoordIn; 
	normal = (normalMatrix * vec4(normalIn,0.0)).xyz;
	modelViewPosition = (modelViewMatrix * vec4(vertex, 1)).xyz;
	shadowMapCoord = lightMatrix * vec4(modelViewPosition, 1.0);
	shadowMapCoord.xyz *= vec3(0.5f, 0.5f, 0.5f);	
	shadowMapCoord.xyz += shadowMapCoord.w * vec3(0.5f, 0.5f, 0.5f);

}