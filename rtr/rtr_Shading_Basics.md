### Shading Basics

#### Shading Models

The first step in determining the appearance of a rendered object is to choose a shading model to describe how the object's color should vary based on factors such as surface orientation, view direction, and lighting. Shading model often have properties used to control appearance variation. Setting the values of these properties is the next step in determining object appearance.

The ***Gooch shading model*** was designed to increase legibility of details in technical illustrations. The mathematical definition of the model is:

$$
c_{shaded} = s*c_{highlight} + (1-s)(tc_{warm}+(1-t)*c_{cool})\\
c_{cool}=(0,0,0.55)+0.25*c_{surface}\\
c_{warm}=(0.3,0.3,0)+0.23*c_{surface}\\
c_{highlight}=(1,1,1)\\
t=\frac{n*l+1}{2}\\
r=2(n*l)n-l\\
s=(100(r*v)-97)^{\overline+}
$$
Linear interpolation appears so often in shaders that it is a built-in function, called ***lerp*** and ***mix***, in every shader language we have seen.

#### Light Sources

The impact of lighting on our example shading model was quite simple; it provided a dominant direction for shading. The next step in the lighting complexity is for the shading model to react to the presence or absence of light in a binary way.

It is a small step from the binary presence or absence of light to a continuous scale of light intensities. This could be expressed as a simple interpolation between absence and full presence, which implies a bounded range for the intensity, perhaps 0 to 1, or as an unbounded quality that affects the shading in some other way. A common option for the latter is to factor the shading model into lit and unlit parts, with the light intensity k<sub>light</sub> linearly scaling the lit part:
$$
c_{shaded} = f_{unlit}(n,v) + k_{light}f_{lit}(l,n,v)
$$
This easily extends to an RGB light color c<sub>light</sub>,
$$
c_{shaded}=f_{unlit}(n,v)+c_{light}f_{lit}(l,n,v)
$$
and to multiple light sources:
$$
c_{shaded}=f_{unlit}(n,v)+\sum\limits_{i=1}^{n}c_{light_{i}}f_{lit}(l_{i},n,v)
$$
**The unlit part f<sub>unlit</sub>(n, v) of shading model often expresses some form of lighting that does not come directly from explicitly placed light sources, such as light from the sky or light bounced from surrounding objects.**

The effect of light on a surface can be visualized as a set of rays, with the density of rays hitting the surface corresponding to the light intensity for surface shading purposes. The ray density is proportional to the dot product of **l** and **n** when it is positive. Negative values corresponds to light ray coming from behind the surface, which has no effect. So, before multiplying the light's shading by the lighting dot product, we first clamp the dot product to 0:
$$
c_{shaded}=f_{unlit}(n,v)+\sum\limits_{i=1}^{n}(l_{i}*n)^{+}c_{light_{i}}f_{lit}(l_{i},n,v)
$$
The simplest possible choice for the function f<sub>lit</sub>, is to make it a constant color,
$$
f_{lit}() = c_{surface}
$$
The final function is:
$$
c_{shaded}=f_{unlit}(n,v)+\sum\limits_{i=1}^{n}(l\cdot n)^+c_{light}c_{surface}
$$
The lit part of this model is corresponds to the ***Lambertian*** shading model. This model works in the context of ideal diffusely reflecting surfaces. In the equations we can see that a light source interacts with the shading model via two parameters: the vector **l** pointing toward the light and the light color c<sub>light</sub>.

##### Directional Lights

Both **l** and c<sub>light</sub> are constant in directional light, except that c<sub>light</sub> may be attenuated by shadowing.

##### Punctual Lights

A punctual light is not one that is on time for its appointments, but rather a light that has a location, unlike directional light. Such lights also have no dimensions to them, no shape or size. Point light (shines light equally in all direction) and spotlight are two different forms of punctual lights. The light direction is:
$$
l=\frac{P_{light}-p_0}{||p_{light}-p_0||}
$$
Also, c<sub>light</sub> is a function of distance.

##### Point/Omni Lights

Punctual lights that emit light uniformly in all directions are known as point lights or omni lights. For point lights, c<sub>light</sub> varies as a function of the distance r. At a given surface, the spacing between rays from a point light is proportional to the distance from the surface to the light. The spacing increase happens along both dimensions of the surface, so the ray density is proportional to 1/(r*r). The equation is:
$$
c_{light}(r)=c_{light_0}(\frac{r_{0}}{r})^2
$$
Problems:

+  r is very small, error occurs. To avoid this, Unreal add a small value to r, which is:

$$
c_{light}(r) = c_{light_0}\frac{r_{0}^2}{r^2+\epsilon}
$$

​		CryEngine and Frostbite use this:
$$
c_{light}(r)=c_{light_0}(\frac{r_0}{max(r, r_{min})})^2
$$
​		where r<sub>min</sub> is the radius of object emitting light.

+ r is very large. For efficient rendering, it is desirable for lights to reach 0 intensity at some finite distance. One solution is to multiply the inverse-square equation by a ***windowing function*** with the desired properties. One common function is:
  $$
  f_{win}(r)=(1-(\frac{r}{r_{max}})^4)^{+2}
  $$
  

At last the equation is
$$
c_{light}(r)=c_{light_0}f_{dist}(r)
$$
where f<sub>dist</sub>(r) is some function of distance. Such functions are called ***distance falloff functions***.

##### Spotlights

Illumination from nearly all real world light sources varies by direction as well as distance. This variation can be expressed as a directional falloff function f<sub>dir</sub>(l), 
$$
c_{light}=c_{light_0}f_{dist}(r)f_{dir}(l)
$$
Most spotlight functions use expressions composed of the cosine of $\theta_{s}$, which is the most common form for anges in shading. Spotlights typically have an **umbra angle** $\theta_{u}$, which bounds the light such that $f_{dir}(l)=0$ for all $\theta_s>=\theta_u$. Spotlights can also have **penumbra angle** $\theta_p$, which define an inner cone where the light is at its full intensity. Sample functions:
$$
t=(\frac{cos\theta_s - cos\theta_u}{cos\theta_p-cos\theta_u})^{\overline+}\\
f_{dir}(l)=t^2\\
f_{dir}(l) = smoothstep(t)=t^2(3-2t)
$$
Two type of direction function of spotlight.

#### Implementing Shading Models

##### Frequency of Evaluation

When designing a shading implementation, the computations need to be divided according to their ***frequency of evaluation***. 

 First, determine whether the result of a given computation is always constant over an entire draw call. In this case, the compuation can be performed by the application on the CPU. The result are passed to the graphics API via uniform shader inputs.

If the result of a shading computation changes within a draw call, it can't be passed to the shader through a uniform shader input. Instead, it must be computed by one of the programmable shader stages and, if needed, passed to other stages via varying shader inputs.

In principle, it would be possible to compute only the ***specular highlight*** part of the shading model in the pixel shader, and calculate the rest in the vertex shader. But in practice, this kind of hybrid implementation is often not opitmal. So in most implementations the vertex shader is responsible for non-shading operation such as geometry transfomation and deformation. The resulting geometric surface properties, transformed into the appropriate coordinate system, are written out by the vertex shader, linearly interpolated over the triangle, and passed into the pixel shader as varying shader inputs.

##### Implementation Example

 The function used is:
$$
c_{shaded}=\frac{1}{2}c_{cool}+\sum\limits_{i=1}^{n}(l_i\cdot n)^+c_{light_{i}}(s_ic_{highlight}+(1-s_i)c_{warm})\\
c_{cool}=(0,0,0.55)+0.25c_{surface}\\
c_{warm}=(0.3,0.3,0)+0.25c_{surface}\\
c_{highlight}=(2,2,2)\\
r_i=2(n\cdot l_i)*n-l_i\\
s_i=(100*(r_i\cdot v)-97)^{\overline{+}}
$$
This function fits the multi-light structure equation
$$
c_{shaded}=f_{unlit}(n,v)+\sum\limits_{i=1}^n(l\cdot n)^+c_{light_i}f_{lit}(l_i,n,v)\\
f_{unlit}(n,v)=\frac{1}{2}c_{cool}\\
f_{lit}(l_i,n,v)=s_ic_{highlight}+(1-s_i)c_{warm}
$$
Two categories of shader inputs:

+ **unifrom inputs**: It have values set by the application and these values remain constant over  a draw call.
+ **varying inputs**: It have values that can change between shader invocations (pixel or vertices)

The detail shader code is:

```c
					Pixel shader
in vec3 vPos;
in vec3 vNormal;
out vec4 outColor;


struct Light{
	vec4 position;
	vec4 color;
};

uniform LightUBlock{
	Light uLights[MAXLIGHS];
};

uniform uint uLightCount;
vec3 lit(vec3 l, vec3 n, vec3 v){
	vec3 r_l = reflect(-l,n);
	float s = clamp(100 * dot(r_l, v)-97, 0.0, 1.0);
	vec3 hightlightColor = vec3(2,2,2);
	return mix(uWarmColor, hightlightColor, s);
}

void main(){
	vec3 n = normalize(vNormal);
	vec3 v = normalize(uEyePosition.xyz-vPos);
	outColor = vec4(uFunlit, 1.0);
	
	for(uint i=0u; i<uLightCount; i++){
		vec3 l = normalize(uLights[i].position.xyz - vPos);
		float Ndl = clamp(dot(n, l),0.0,1);
		outColor.rgb += Ndl * uLights[i].color.rgb * lit(l, n, v);
	}
}
```

Code explanation:

+ The position and color are defined as vec4 instead of vec3 simplifies the task of ensuring consistent data layout between CPU and GPU.
+ The array of Light structs is defined inside a named uniform block, which is a GLSL feature for binding a group of uniform variables to a buffer object for faster data transfer.
+ The ***reflect()*** function calculate a reflected vector by the second vector.
+ The ***clamp()*** clamps the first parameter in between the second and third parameter. It is like HLSL ***saturate()*** when the range is [0.0, 1.0]
+ The ***mix()*** corresponding to ***lerp()*** in HLSL.

```c
layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
out vec3 vPos;
out vec3 vNormal;

void main(){
	vec4 worldPosition = uModel * position;
	vPos = worldPosition.xyz;
	vNormal = (uModel * normal).xyz;
	gl_position = viewProj * worldPosition;
}
```

In the last step, the surface position is transformed into clip space and passed into **gl_Position**, a special system-defined variable used by the rasterizer. The **gl_Position** variable is the one required output from any vertex shader.

##### Material Systems

Material is an artist-facing encapsulation of the visual appearance of a surface. While materials are implemented via shaders, the same material may use different shaders. The most common case is parameterized materials. In its simplest form, material parameterization requires two types of material entities: 

+ **material templates** 

  Each material template describes a class of materials and has a set of parameters that can be assigned numerical, color, or texture values depending on the parameter type.

+ **material instances**

  Each material instance corresponds to a material template plus a specific set of values for all of its parameters.

Parameters may be resolved at runtime, by passing uniform inputs to the shader parameter to the shader program, or at compile time, by substituting values before the shader is compiled.

While the material parameters may correspond one-to-one with the parameters of the shading model, this is not always the case. A material may fix the value of a given shading model parameter, such as the surface color, to a constant value.

One of the most important tasks of a material system is dividing various shader functions into separate elements and controlling how these are combined. On GPU, the program for each shader stage is compiled as a unit. The separation between shader stages does offer some limited modularity.

To resolve the large use of variants, the method includes:

+ Code reuse. Implementing function in shared files, using #include preprocessor directives to access those functions from any shader that needs them
+ Subtractive: A shader, often referred to as an supershader, that aggregates a large set of functionality, using a combination of compile-time preprocessor conditionals and dynamic branching to remove unused parts and to swith between mutually exclusive alternatives.
+ Additive: Various bits of functionality are defined as nodes with input and ouput connectors, and these are composed together.
+ Template-based: An interface is defined, into which different implementations can be plugged as long as they conform to that interface. 

#### Aliasing and Antialiasing

##### Sampling and Filtering Theory

The process of rendering a images is inherently a sampling task. This is so since the generation of an image is the process of sampling a three-dimensional scene in order to obtain color values for each pixel in the image.

To store continuous data, we sampling it to represent information digitally. However, the sampled signal needs to be reconstructed to recover the original signal. This is down by ***filtering*** the sampled signal, which may incur aliasing.

Common examples of aliasing in computer graphics are the jaggies of a rasterized line or triangle edge, flickering highlights known as "fireflies", and when a texture with a checker pattern is minified. This is because a signal is sampled at too low a frequency.

**For a signal to be sampled properly, the sampling frequency has to be more than twice the maximum frequency of the signal to be samples.** The sampling frequency is called the **Nyquist rate** or **Nyquist limit**. The signal is **band-limited**.

##### Reconstruction

To reconstruct a signal from the sampled signal, a filter must be used. There are three type of filter: ***box, tent, sinc*** according to their shape.

##### Resampling

Resampling is used to magnify or minify a sampled signal. After sample, we want new sample points to be located uniformly with an interval a between samples. For a > 1, minification takes place, and for a < 1, magnification occurs. 

##### Screen-Based Antialiasing

Additional samples are needed when phenomena such as object edges, specular highlights, and sharp shadows that cause abrupt color changes.

This method operate only on the output samples of the pipeline. The general strategy of screen-based antialiasing schemes is to use a sampling pattern for the screen and then weight and sum the samples to produce a pixel color:
$$
p(x,y)=\sum\limits_{i=1}^{n}w_ic(i,x,y)
$$
where n is the number of samples taken for a pixel. The function $c(i,x,y)$ is a sample color and  $w_i$ is a weight that the sample will contribute to the overall pixel color. Antialiasing algorithms that compute more than one full sample per pixel are called **supersampling (or oversampling)** methods.

+  **full-scene antialiasing (FSAA)** also known as **supersampling antialiasing (SSAA)**, renders the scene at a high resolution and then filters neighboring samples to create an image. For example, render a image of 2560*2048 offscreen for an image of 1280  * 1024, then use box filter to map 2*2 grip to one pixel.
+ **Accumulation buffer**: This method uses a buffer that has the same resolution as the desired image, but with more bit of color per channel. To obtain a 2$\times$2 sampling of a screen, four images are generated, with the view moved half a pixel in the screen x- or y-direction as needed.
+ **Multisampling antialiaing (MSAA)**: This method lessens the high computational costs by computing the surface's shade once per pixel and sharing this result among the samples. Pixels may have multiple sample location per fragment. If the fragment covers fewer positional samples, the shading sample's position can be shifted to better represent the positions covered. This position adjustment is called **centroid sampling** or **centroid interpolation**. This is a fast method.

#### Sampling Patterns

