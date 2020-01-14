### Deferred Shading

Forward rendering or forward shading: Render an object, light it according to all light sources in a scene and then render the next object, and so on for each object in the scene. The flaw of this method is:

+ Each rendered object has to iterate over each light source for every rendered fragment which is a lot, this is quite heavy on performance.
+ Tends to waste a lot of fragment shader runs in scenes with a high depth complexity as most fragment shader outputs are overwritten.

Deferred shading is based on the idea that we defer or postpone most of the heavy rendering (like lighting) to a later stage. It consist of two passes:

+ **Geometry pass**: Render the scene once and retrieve all kinds of geometrical information from the objects that we store in a collection of textures called the **G-buffer**. The information includes position vectors, color vectors, normal vectors specular vectors and so on.
+ **Lighting pass**: Render a screen filled quad and calculate the screen's lighting for each fragment using the geometrical information stored in the G-buffer. Instead of taking each object all the way from the vertex shader to the fragment shader we decouple its advanced fragment processes to a later stage. During light calculations, we take all the required input variables from the corresponding G-buffer textures instead of the vertex shader.

The advantages are:

+ Whatever fragment ends up in the G-buffer is the actual fragment information that ends up as a screen pixel. This ensures that for each pixel we process in the lighting pass we only do this once.
+ Allows to render a much larger amount of light sources.

The disadvantage is:

+ G-buffer stores a relatively large amount of scene data in its texture colorbuffers which eats memory, especially position vectors require a high precision.
+ It doesn't support blending and MSAA no longer works.

#### The G-buffer

The G-buffer is the collective term of all textures used to store light-relevant data for the final lighting pass. The pseudocode of the entire pass like:

```c
while(...)
{
    //geometry pass, generate G-buffer
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gBufferShader.use();
    
    for(Object obj : objects)
    {
        ConfigureShaderTransformsAndUniforms();
        obj.Draw();
    }
}
```

To generate G-buffer textures, we use multiple render targets to render to multiple colorbuffers in a single pass. 

The code of G-buffer generate fragment code is:

```c
#version 330 core 
layout(location=0) out vec3 gPosition;
layout(location=1) out vec3 gNormal;
layout(location=2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
	gPosition = FragPos;
	gNormal = normalize(Normal);
	gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
	// store specular intensity in gAlbedoSpec's alpha component.
	gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
}
```

#### The deferred lighting pass

With a large collection of fragment data in the G-Buffer at our disposal we have the option to completely calculate the scene's final lighted colors by iterating over each of the G-buffer texture pixel and use their content as input to the lighting algorithms. The fragment shader is:

```c
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
```

Deferred shading can't use blend. Besides, deferred shading forces you to use the same lighting algorithm for mast of your scene's lighting.

#### Combining deferred rendering with forward rendering

To overcome deferred shading's disadvantages, the rendered is split into two pass, one deferred rendering part, and the other a forward rendering part.

To take any the stored geometry depth of the deferred renderer into account, we need to first copy the depth information stored in the geometry pass into the default framebuffer's depth buffer. The **glBlitFramebuffer** is a function that copy the content of a framebuffer to the content of another framebuffer.

```c
glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
glBindFramebuffer(GL_DRAE_FRAMEBUFFER, 0);

glBlitFramebuffer(0, 0, SRC_WIDTH, SRC_HEIGHT, 0, 0, SRC_WIDTH, SRC_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
```

##### A larger number of lights

Deferred rendering is praised for its ability to render an enormous amount of light sources without a heavy cost on performance. Deferred rendering by itself doesn't allow for a very large amount of light sources as we'd still have to calculate each fragment's lighting component for each of the scene's light sources. What makes a large amount of light sources possible is a very neat optimization we can apply to deferred shading: **light volumes**.

The idea behind light volumes is to calculate the radius or volume of a light source, i.e. the area where its light is able to reach fragments. We calculate the maximum distance or radius the light is able to reach depending on the attenuation form and only do the expensive lighting calculations if a fragment is inside one or more of these light volumes.