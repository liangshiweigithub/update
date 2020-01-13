### HDR

Brightness and color values are clamped between 0.0 and 1.0 when stored into a framebuffer. If a large number of fragments's color values getting clamped to 1.0 each of the bright fragments have the exact same white color in a large region, losing a significant amount of detail and giving a fake look.

Monitors are limited to display colors in the range of 0.0 and 1.0, but there is no such limitation in lighting equations. By allowing fragment color to exceed 1.0 we have a much higher range of color values available to work in known as **high dynamic range (HDR).** 

High dynamic range rendering allow for a much larger range of color values to render to collecting a large range of dark and bright details of a scene, and at the end we transform the HDR values to the **low dynamic range(LDR) of [0.0, 1.0].** This process of converting HDR values to LDR values is called **tone mapping** and a large collection of tone mapping algorithms exist that aim to preserve most HDR details during the conversion process. These tone mapping algorithms often involve an exposure parameter that selectively favors dark or bright regions.

When it comes to real time rendering high dynamic range allows us not only to exceed the LDR range of [0.0, 1.0] and preserve more detail, but also gives us the ability to specify a light source's intensity by their real intensities.

#### Floating point framebuffers

When framebuffers use a normalized fix-point color format (like GL_RGB) as their colorbuffer's internal format OpenGL automatically clamps the value to [0, 1] before storing them in framebuffer.

If the internal format of a framebuffer's color buffer is specified as **GL_RGB16F, GL_RGBA16F, GL_RGB32F or GL_RGBA32F** the framebuffer is known as a **floating point framebuffer** that can store floating point values outside the range. 

```c
glBindTexture(GL_TEXTURE_2D, colorBuffer);
// create a texture with format GL_RGBA16F and bind to framebuffer as color
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SRC_WIDTH, SRC_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
```

#### Tone mapping

Tone mapping is the process of transforming floating point color values to the expected [0.0, 1.0] range known as low dynamic range without losing too much of detail, often accompanied with a specific stylistic color balance.

The simplest tone mapping algorithm is known as **Reinhard tone mapping** and involves dividing the entire HDR color values to LDR color values evenly balancing them all out.

```c
void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrTexture, TexCoords).rbg;
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
}
```

#### Exposure

Tone mapping allows the use of an exposure parameter. It makes sense to use a lower exposure at daylight and a higher exposure at night time. A simple example is:

```c
uniform float exposure;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    
    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    mapped = pow(mapped, vec3(1.0/gamma));
    FragColor = vec4(mapped, 1.0);
}
```

With high exposure values the darker areas  show significantly more detail.

The real benefit of HDR rendering really shows itself in large and complex scenes with heavy light algorithms.



### Bloom

Bloom gives all brightly lit regions of a scene a glow-like effect. Bloom gives noticeable visual cues about the brightness of objects as bloom tends to give the illusion objects are really bright. HDR makes bloom more effective to implement. Steps of implement bloom is:

+ Render a lighted scene as usual. Take this HDR color buffer texture.
+ Extract all the fragments that exceed a certain brightness.
+ Blur the extracted brightness image.
+  Add the blurred result on top of the original HDR scene image.

#### Extracting bright color

**Multiple Render Targets (MRT)** allows to specify more than one fragment shader output. This gives us the option to extract the first two images in a single render pass. By specifying a layout location specifier before a fragment shader's output we can control to which colorbuffer a fragment shader writes to:

```c
layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 BrightColor;

// some calculation
[...]
void main()
{
    [....]; // lighting calculations and output results
    FragColor = vec4(lighting, 1.0);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    // check whether fragment output is higher than threshold
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rbg, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
```

Here we calculate the brightness of a fragment by properly transforming it to grayscale first. The brightness  check shows that bloom works incredibly will with HDR rendering. With HDR, color values can exceed 1.0 which allows to specify a brightness threshold outside  the default range, giving us much more control over what of an image is considered as bright.

For using multiple fragment shader output we need multiple colorbuffers attached to the currently bound framebuffer object. We have to explicitly tell OpenGL we're rendering to multiple colorbuffers via **glDrawBuffers**.

```c
unsigned int attachments[2]={GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
glDrawBuffers(2, attachments);
```

#### Gaussian blur

![avatar](..\image\bloom_gaussian.png)

A Gaussian blur is based on the Gaussian curve which is commonly described as a bell-shaped curve giving high values close to its center that gradually wear off over distance. As the Gaussian curve has a larger area close to its center, using its values as weights to blur an image gives great results as samples close by have a higher precedence.

To implement a Gaussian blur filter we'd need a two dimensional box of weights that we can obtain from a 2 dimensional Gaussian curve equation. The problem is that we need to sample too many times.

The Gaussian equation has a very neat property that allows us to separate the two dimensional equation into two smaller equations: one that describes the horizontal weights and the other describes the vertical weights. We'd then first do a horizontal blur with horizontal weights on an entire texture and then on the resulting texture do a vertical blur. This is only have to do 32 + 32 samples compared to 1024. This is known as two-pass Gaussian blur. The blur code is:

```c
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform bool horizontal;
uniform sampler2D image;


void main()
{
	vec3 result = texture(image, TexCoords);
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
			result += texture(image, TexCoords + vec2(texOffset.y * i, 0)).rgb * weights[i];
			result += texture(image, TexCoords - vec2(texOffset.y * i, 0)).rgb * weights[i];
		}
	}
	FragColor = vec4(result, 1.0);
}
```

#### Blending both textures

The last blend step is combine the two to achieve the imfamous bloom or glow effect.