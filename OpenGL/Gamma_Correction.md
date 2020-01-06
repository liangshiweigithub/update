### Gamma Correction

The CRT monitors had the physical property that twice the input voltage did not result in twice the amount of brightness. Doubling the input voltage resulted in a brightness equal to an exponential relationship of roughly 2.2 also known as the gamma of a monitor. This happens to closely match how human beings measure brightness as brightness is also displayed with a similar power relationship.

![avatar](../image/gamma_correction_brightness.png)

The human eyes prefer to see brightness colors according to the top scale, monitors use a relationship for displaying output colors so that the original physical brightness colors are mapped to the non-linear brightness colors in the top scale.

The non-linear mapping of monitors does indeed make the brightness look better in our eyes, but with this problem: all the color and brightness options we configure in our applications are based on what we perceive from the monitor and thus all the options are actually non-linear brightness/color options.

Because colors are configured based on the monitor's display all intermediate (lighting) calculations in linear-space are physically incorrect.

#### Gamma correction

 The ideal of gamma correction is to apply the inverse of the monitor's gamma to the final output color before displaying to the monitor. We multiply each of the linear output colors by this inverse gamma curve and as soon as the colors are displayed on the monitor, the monitor's gamma curve is applied and the resulting colors become linear. For example, we first scale the color by a power of 1/2.2. The color is right now.
$$
(0.5,0.0, 0.0)^{1/2.2} = (0.73, 0.0, 0.0);\\
(0.73, 0.0, 0.0)^{2.2} = (0.5, 0.0, 0.0);
$$
Tow ways of apply gamma correction:

##### OpenGL's built-in sRGB framebuffer support

This option is the easiest, but less control. By enabling **GL_FRAMEBUFFER_SRGB**, the OpenGL knows that each subsequent drawing commands should first gamma correct colors from the sRGB color space before storing them in color buffer. The **sRGB** is a color space that roughly corresponds to a gamma of 2.2 and a standard for most home devices. After enabling **GL_FRAMEBUFFER_SRGB** OpenGL will automatically perform gamma correction after each fragment shader run to all subsequent framebuffers, including the default framebuffer.

```
glEnable(GL_FRAMEBUFFER_SRGB);
```

##### Doing the gamma correction in the fragment shaders

This approach gives us complete control over the gamma operations.

```c
void main()
{
	[....];
    // apply gamma correction
    FragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
}
```

Issue: In order to be consistent you have to apply gamma correction to each fragment shader that contributes to the final output so if you have a dozen fragment shaders for multiple objects, you have to add the gamma correction code to each of these shaders. Add post-processing stage in render loop is a good way to fix this.

#### sRGB textures

Texture artists create all the textures in sRGB space so if we use those textures as they are in our rendering application we have to take this into account. If apply gamma correction again, we applied gamma correction two times. The solution is to re-correct or transform these sRGB textures back to linear space before any calculation as:

```
float gamma = 2.2;
vec3 diffuseColor = pow(texture(diffuse, texCoords).rgb, vec3(gamma));
```

OpenGL provides **GL_SRGB** and **GL_SRGB_ALPHA** (with alpha component)as internal texture formats. Using the format OpenGL will automatically correct the colors to linear space.

```c
glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
```

Be careful when specifying textures in sRGB space. Textures used for coloring objects like diffuse textures are almost always in sRGB space. Texture used for retrieving lighting parameters like specular maps and normal maps are almost always in linear space.

#### Attenuation

The lighting attenuation is different with gamma correction.

```
float attenuation = 1.0 / (distance * distance);
```

This attenuation is way too strong. The light attenuation functions change brightness, and we weren't visualizing our scene in linear space. If without gamma correction, the attenuation function effectively becomes $(1.0/distance^2)^{2.2}$ when display on a monitor. This creates a much larger attenuation.

Gamma correction allows to work/visualize you renders in linear space. Because linear space makes sense in the physical world, most physical equations now actually give good results like read light attenuation. 